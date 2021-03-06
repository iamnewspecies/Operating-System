#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include "multiboot.h"
#include "mmu.c"

/* Check if the compiler thinks if we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
extern int /* the type doesn't matter! */ _end;
/* Hardware text mode color constants. */
enum vga_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

uint16_t make_vgaentry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

size_t strlen(const char* str)
{
	size_t ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 24;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize()
{
	//colors the whole screen blue
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_WHITE, COLOR_BLUE);
	terminal_buffer = (uint16_t*) 0xB8000;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}

}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_putchar(char c)
{
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if ( ++terminal_column == VGA_WIDTH )
	{
		terminal_column = 0;
		if ( ++terminal_row == VGA_HEIGHT )
		{
			terminal_row = 0;
		}
	}
}

void terminal_writestring(const char* data)
{
	size_t datalen = strlen(data);
	for ( size_t i = 0; i < datalen; i++ )
		terminal_putchar(data[i]);
}

void print_numbers(unsigned long i)
{
    int k;
    unsigned long a[100];
    k=0;
    if(i==0)
    {
        terminal_putchar('0');
        return;
    }
    while(i>0)
    {
        a[k] = (i%10);
        i /= 10;
        k++;
    }
    k--;
    while(k>=0)
    {
        switch(a[k])
        {
            case 1:terminal_putchar('1');
                    break;
            case 2:terminal_putchar('2');
                    break;
            case 3:terminal_putchar('3');
                    break;
            case 4:terminal_putchar('4');
                    break;
            case 5:terminal_putchar('5');
                    break;
            case 6:terminal_putchar('6');
                    break;
            case 7:terminal_putchar('7');
                    break;
            case 8:terminal_putchar('8');
                    break;
            case 9:terminal_putchar('9');
                    break;
            case 0:terminal_putchar('0');
                    break;
            default:terminal_putchar('e');
        }
        k--;
    }
}

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(multiboot_info_t* mbd,unsigned int magic)
{

	uint16_t i = 1,j=0,k=0;
	uintptr_t kernel_end = (uintptr_t) &_end;
	terminal_initialize();
	/* Since there is no support for newlines in terminal_putchar yet, \n will
	   produce some VGA specific character instead. This is normal.
	*/
	print_numbers(kernel_end);
	terminal_row++;
	terminal_column=0;
	print_numbers(mbd->mmap_length);//144
	terminal_row++;
	terminal_column=0;
	print_numbers(mbd->mmap_addr);//65704
	terminal_row++;
	terminal_column=0;

	memory_map_t * mm =(memory_map_t *)  mbd->mmap_addr;
	/*
	typedef struct memory_map
     {
       unsigned long size;
       unsigned long base_addr_low;
       unsigned long base_addr_high;
       unsigned long length_low;
       unsigned long length_high;
       unsigned long type;
     } memory_map_t;

	*/
	while(i <= (mbd->mmap_length)/sizeof(memory_map_t))
	{
        if(mm->type == 1 )
        {
            terminal_writestring("string_");
            print_numbers(i);
            terminal_row++;
            terminal_column = 0;
            print_numbers(mm->size);
            terminal_row++;
            terminal_column = 0;
            print_numbers(mm->base_addr_low);
            terminal_row++;
            terminal_column = 0;
            print_numbers(mm->base_addr_high);
            terminal_row++;
            terminal_column = 0;
            print_numbers(mm->length_low);
            terminal_row++;
            terminal_column = 0;
            print_numbers(mm->length_high);
            terminal_row++;
            terminal_column = 0;
            print_numbers(mm->type);
            terminal_row++;
            terminal_column = 0;
            j++;
        }
        if((mm->type != 1))
        {
            k++;
        }
        i++;
        mm++;
    }
    terminal_writestring("count_1 = ");
    print_numbers(j);
    terminal_row++;
    terminal_column = 0;
    terminal_writestring("count_null = ");
    print_numbers(k);
    terminal_row++;
    terminal_column = 0;
    print_numbers(sizeof(unsigned long));
}
