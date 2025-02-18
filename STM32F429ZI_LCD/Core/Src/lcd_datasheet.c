#if 0

#include "lcd_datasheet.h"

/*
 *(unsigned int *) 0x40020414 &= ~0x0800; // RS
 *(unsigned int *) 0x40020414 &= ~0x0400; // RW
 *(unsigned int *) 0x40020414 &= ~0x1000; // ENABLE
 *(unsigned int *) 0x40021014 |= 0x0100; // D0 PORTE 8
 *(unsigned int *) 0x40021014 &= ~0x0200; // D1 PORTE 9
 *(unsigned int *) 0x40021014 &= ~0x0400; // D2 PORTE 10
 *(unsigned int *) 0x40021014 &= ~0x0800; // D3 PORTE 11
 *(unsigned int *) 0x40021014 &= ~0x1000; // D4 PORTE 12
 *(unsigned int *) 0x40021014 &= ~0x2000; // D5 PORTE 13
 *(unsigned int *) 0x40021014 &= ~0x4000; // D6 PORTE 14
 *(unsigned int *) 0x40021014 &= ~0x8000; // D7 PORTE 15
 */

void LCD_pulse_enable(void)
{
	*(unsigned int *) 0x40020414 |= 0x10; // ENABLE
	HAL_Delay(1);
	*(unsigned int *) 0x40020414 &= ~0x10; // ENABLE
	HAL_Delay(1);
}

void LCD_write_data(uint8_t data)
{
	*(unsigned int *) 0x40020414 |= 0x08; // RS
	*(unsigned int *) 0x40021014 |= data; // D0 PORTE 8
	LCD_pulse_enable();
	HAL_Delay(2);
}

void LCD_write_command(uint8_t command)
{
	*(unsigned int *) 0x40020414 &= ~0x08; // RS
	*(unsigned int *) 0x40021014 |= command; // D0 PORTE 8
	LCD_pulse_enable();
	HAL_Delay(2);
}

void LCD_clear(void)
{
	LCD_write_command(0x01);
	HAL_Delay(2);
}

void LCD_init(void)
{
	HAL_Delay(50);

	*(unsigned int *) 0x40020414 &= ~0x0400; // RW

	LCD_write_command(0x38);

	uint8_t command = 0x08 | (1 << 2);
	LCD_write_command(command);

	LCD_clear();

	LCD_write_command(0x06);
}

void LCD_write_string(char *string)
{
	uint8_t i;
	for (i = 0; string[i]; i++)
	{
		LCD_write_data(string[i]);
	}
}

void LCD_goto_xy(uint8_t row, uint8_t col)
{
	col %= 16;
	row %= 2;

	uint8_t address = (0x40 * row) + col;
	uint8_t command = 0x80 + address;

	LCD_write_command(command);
}

int LCD_driver_main(void)
{
	LCD_init();

	LCD_write_string("Hello World!!!");

	HAL_Delay(1000);

	LCD_clear();

	while(1);
	return 0;
}



void lcd_init()
{
	*(unsigned int *) 0x40020414 &= ~0x08; // RS
	*(unsigned int *) 0x40020414 &= ~0x04; // RW
	*(unsigned int *) 0x40021014 |= 0x30; // D5, D4 SET HIGH
}

void function_set(void)
{
	lcd_init();
	HAL_Delay(5);
	lcd_init();
	HAL_Delay(1);
	lcd_init();
}

void lcd_datasheet_main(void)
{
	function_set();

	while(1)
	{
		*(unsigned int *) 0x40020414 |= 0x08; // RS
		*(unsigned int *) 0x40020414 &= ~0x04; // RW
		*(unsigned int *) 0x40020414 |= 0x10; // ENABLE

		*(unsigned int *) 0x40021014 |= 0x00; // PORTE
		*(unsigned int *) 0x40021014 |= 0x31; // PORTE
	}

}

/*
void i2c_lcd_init(void){

	lcd_command(0x33);
	lcd_command(0x32);
	lcd_command(0x28);	//Function Set 4-bit mode
	lcd_command(DISPLAY_ON);
	lcd_command(0x06);	//Entry mode set
	lcd_command(CLEAR_DISPLAY);
	HAL_Delay(2);
}
 */
void clear_lcd(void)
{
	*(unsigned int *) 0x40020414 &= ~0x0800; // RS
	*(unsigned int *) 0x40020414 &= ~0x0400; // RW
	*(unsigned int *) 0x40021014 &= ~0x8000; // ENABLE
	*(unsigned int *) 0x40021014 |= 0x4000; // D0 PORTE 14
	*(unsigned int *) 0x40021014 &= ~0x1000; // D1 PORTE 12
	*(unsigned int *) 0x40021014 &= ~0x8000; // D2   PORTF 15
	*(unsigned int *) 0x40021014 &= ~0x2000; // D3 PORTE 13
	*(unsigned int *) 0x40021014 &= ~0x4000; // D4   PORTF 14
	*(unsigned int *) 0x40021014 &= ~0x0800; // D5 PORTE 11
	*(unsigned int *) 0x40021014 &= ~0x0200; // D6 PORTE 9
	*(unsigned int *) 0x40021014 &= ~0x2000; // D7   PORTF 13
}



void send_data(void)
{
	*(unsigned int *) 0x40021014 |= 0x4000; // D0 PORTE 14
	*(unsigned int *) 0x40021014 &= ~0x1000; // D1 PORTE 12
	*(unsigned int *) 0x40021014 |= 0x8000; // D2   PORTF 15
	*(unsigned int *) 0x40021014 &= ~0x2000; // D3 PORTE 13
	*(unsigned int *) 0x40021014 |= 0x4000; // D4   PORTF 14
	*(unsigned int *) 0x40021014 &= ~0x0800; // D5 PORTE 11
	*(unsigned int *) 0x40021014 |= 0x0200; // D6 PORTE 9
	*(unsigned int *) 0x40021014 &= ~0x2000; // D7   PORTF 13
	HAL_Delay(1);
}

#endif
