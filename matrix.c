#include <stdint.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/gpio.h>
#include <stdio.h>
#include "matrix.h"
#define ROW1_PORT_BIT 21
#define ROW2_PORT_BIT 22
#define ROW3_PORT_BIT 15
#define ROW4_PORT_BIT 24
#define ROW5_PORT_BIT 19

#define COL1_PORT_BIT 28
#define COL2_PORT_BIT 11
#define COL3_PORT_BIT 31
#define COL4_PORT_BIT 5
#define COL5_PORT_BIT 30

static const struct device *gpio0, *gpio1;		
void set_row1(int state)
{
	gpio_pin_set(gpio0,ROW1_PORT_BIT,state);
}
void set_row2(int state)
{
	gpio_pin_set(gpio0,ROW2_PORT_BIT,state);
}
void set_row3(int state)
{
	gpio_pin_set(gpio0,ROW3_PORT_BIT,state);
}
void set_row4(int state)
{
	gpio_pin_set(gpio0,ROW4_PORT_BIT,state);
}
void set_row5(int state)
{
	gpio_pin_set(gpio0,ROW5_PORT_BIT,state);
}
void set_col1(int state)
{
	gpio_pin_set(gpio0,COL1_PORT_BIT,state);
}
void set_col2(int state)
{
	gpio_pin_set(gpio0,COL2_PORT_BIT,state);
}
void set_col3(int state)
{
	gpio_pin_set(gpio0,COL3_PORT_BIT,state);
}
void set_col4(int state)
{
	gpio_pin_set(gpio1,COL4_PORT_BIT,state);
}
void set_col5(int state)
{
	gpio_pin_set(gpio0,COL5_PORT_BIT,state);
}
int matrix_begin()
{
	int ret;
	// Configure the GPIO's 	
	gpio0=device_get_binding("GPIO_0");
	if (gpio0 == NULL)
	{
		printf("Error acquiring GPIO 0 interface\n");
		return -1;
	}
	gpio1=device_get_binding("GPIO_1");
	if (gpio0 == NULL)
	{
		printf("Error acquiring GPIO 1 interface\n");
 		return -2;
	}
	ret = gpio_pin_configure(gpio0,ROW1_PORT_BIT,GPIO_OUTPUT);
	ret = gpio_pin_configure(gpio0,ROW2_PORT_BIT,GPIO_OUTPUT);
	ret = gpio_pin_configure(gpio0,ROW3_PORT_BIT,GPIO_OUTPUT);
	ret = gpio_pin_configure(gpio0,ROW4_PORT_BIT,GPIO_OUTPUT);
	ret = gpio_pin_configure(gpio0,ROW5_PORT_BIT,GPIO_OUTPUT);
	
	ret = gpio_pin_configure(gpio0,COL1_PORT_BIT,GPIO_OUTPUT);
	ret = gpio_pin_configure(gpio0,COL2_PORT_BIT,GPIO_OUTPUT);
	ret = gpio_pin_configure(gpio0,COL3_PORT_BIT,GPIO_OUTPUT);
	ret = gpio_pin_configure(gpio1,COL4_PORT_BIT,GPIO_OUTPUT);
	ret = gpio_pin_configure(gpio0,COL5_PORT_BIT,GPIO_OUTPUT);
	
	matrix_all_off();	
	return 0;	
}
void matrix_put_pattern(uint8_t rows, uint8_t cols)
{
	set_row1(rows & 1);
	rows = rows >> 1;
	set_row2(rows & 1);
	rows = rows >> 1;
	set_row3(rows & 1);
	rows = rows >> 1;
	set_row4(rows & 1);
	rows = rows >> 1;
	set_row5(rows & 1);
	
	set_col1(cols & 1);
	cols = cols >> 1;
	set_col2(cols & 1);
	cols = cols >> 1;
	set_col3(cols & 1);
	cols = cols >> 1;
	set_col4(cols & 1);
	cols = cols >> 1;
	set_col5(cols & 1);
}
void matrix_all_off()
{
	set_row1(0);
	set_row2(0);
	set_row3(0);
	set_row4(0);
	set_row5(0);
	
	set_col1(1);
	set_col2(1);
	set_col3(1);
	set_col4(1);
	set_col5(1);
	
}










/*
This is the smile emoji we want to show 
each pixel has been labeled with a letter eg X-a. 
This will allow us to easily create the corresponding binary code.
eg for X-a, the rows and cols are:
        rows = 0b00010;	
		cols = 0b00010;
											   rows
		-------------------------------------
		|      |      |      |      |       |   0
		-------------------------------------
		|      |  X-b |      |  X-a |       |   1
		-------------------------------------
		|      |      |      |      |       |   0
		-------------------------------------
		|   X-d|      |      |      |   X-c |   0
		-------------------------------------
		|      |   X-g|  X-f |   X-e|       |   0
		-------------------------------------
cols:      0      0      0      1      0
*/
void matrix_put_pattern_smile()
{
uint8_t rows = 1;
uint8_t cols = 1;
	for (int i = 0; i < 300; i++) //flashing lights for 4,700 milliseconds (2 K_msleep *7 each loop *300 loops in total )
				{
					rows = 0b00010; //X-a
					cols = 0b00010;
					matrix_put_pattern(rows, ~cols);
					k_msleep(2);
					rows = 0b00010; //X-b
					cols = 0b01000;
					matrix_put_pattern(rows, ~cols);
					k_msleep(2);

					rows = 0b01000; //X-c
					cols = 0b00001;
					matrix_put_pattern(rows, ~cols);
					k_msleep(2);
					rows = 0b01000; //X-d
					cols = 0b10000;
					matrix_put_pattern(rows, ~cols);
					k_msleep(2);
					rows = 0b10000; //X-e
					cols = 0b00010;
					matrix_put_pattern(rows, ~cols);
					k_msleep(2);
					rows = 0b10000; //X-f
					cols = 0b00100;
					matrix_put_pattern(rows, ~cols);
					k_msleep(2);
					rows = 0b10000; //X-g
					cols = 0b01000;
					matrix_put_pattern(rows, ~cols);
					k_msleep(2);
				}
				matrix_put_pattern(0, ~0); //turns off all leds at the end of diagonal


}



/*

This is the smile emoji we want to show 
each pixel has been labeled with a letter eg X-a. 
This will allow us to easily create the corresponding binary code.
eg for X-a, the rows and cols are:
        rows = 0b00010;	
		cols = 0b00010;
-------------------------------------
|      |      |      |      |       |
-------------------------------------
|      |  X-b |      |  X-a |       |
-------------------------------------
|      |      |      |      |       |
-------------------------------------
|      |  X-g | X-f  |  X-e |       |
-------------------------------------
|   X-d|      |      |      | X-c   |
-------------------------------------


*/
void matrix_put_pattern_patternUnhappy()
{
uint8_t rows = 1;
uint8_t cols = 1;
	for (int i = 0; i < 300; i++) //flashing lights 
	{
		rows = 0b00001; //X-a
		cols = 0b00010;
		matrix_put_pattern(rows, ~cols);
		k_msleep(2);
		rows = 0b00001; //X-b
		cols = 0b01000;
		matrix_put_pattern(rows, ~cols);
		k_msleep(2);

		rows = 0b10000; //X-c
		cols = 0b00001;
		matrix_put_pattern(rows, ~cols);
		k_msleep(2);
		rows = 0b10000; //X-d
		cols = 0b10000;
		matrix_put_pattern(rows, ~cols);
		k_msleep(2);
		rows = 0b01000; //X-e
		cols = 0b00010;
		matrix_put_pattern(rows, ~cols);
		k_msleep(2);
		rows = 0b01000; //X-f
		cols = 0b00100;
		matrix_put_pattern(rows, ~cols);
		k_msleep(2);
		rows = 0b01000; //X-g
		cols = 0b01000;
		matrix_put_pattern(rows, ~cols);
		k_msleep(2);
	}
	matrix_put_pattern(0, ~0); //turns off all leds at the end of diagonal


}