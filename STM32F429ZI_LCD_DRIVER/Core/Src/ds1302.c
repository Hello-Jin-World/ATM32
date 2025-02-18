#include "ds1302.h"

t_ds1302 ds_time; // memory access
t_set_time set_time;

void ds1302_init_time_date(void);
void ds1302_write(uint8_t addr, uint8_t data);
void ds1302_set_time(void);
void ds1302_main(void);
void ds1302_DataLine_Output(void);
void ds1302_DataLine_Input(void);
void ds1302_clock(void);
void ds1302_init_gpio_low(void);
uint8_t ds1302_read(uint8_t addr);
void ds1302_tx(uint8_t tx);
void ds1302_rx(char *data8);
void ds1302_read_time(void);
void ds1302_read_date(void);
void ds1302_flash_clock(void);

char lcd_buffer_time[40];
char lcd_buffer_date[40];

extern unsigned char dec2bcd(unsigned char byte);
extern unsigned char bcd2dec(unsigned char byte);
extern void flash_export_time_date(void);
extern HAL_StatusTypeDef flash_erase();
extern void flash_import_time_date(void);
extern volatile int TIM4_ds1302_sec_counter;  // ADD_PSJ_0930  // ADD_PSJ_0930

void ds1302_flash_clock(void)
{
	// 1. ds1302 read time
	ds1302_read_time();
	// 2. ds1302 read date
	ds1302_read_date();
	// 3. printf time, date
	if (TIM4_ds1302_sec_counter >= 500)
	{
		TIM4_ds1302_sec_counter = 0;
		sprintf(lcd_buffer_date, "%04d-%02d-%02d",ds_time.year+2000,ds_time.month,ds_time.date);
		sprintf(lcd_buffer_time, "%02d:%02d:%02d",ds_time.hour,ds_time.minutes,ds_time.seconds);
		printf("DATE : %4d - %2d - %2d   TIME : %2d : %2d : %2d\n"
				, ds_time.year + 2000, ds_time.month, ds_time.date, ds_time.hour, ds_time.minutes, ds_time.seconds);
	}
	//
	//	if (TIM4_ds1302_sec_counter >= 10000)
	//	{
	//		TIM4_ds1302_sec_counter = 0;
	//		//flash_erase();
	//		//flash_export_time_date();
	//	}
	// 4. delay 1sec
	//HAL_Delay(1000);
}

void ds1302_main(void)
{
	// initialize
	ds1302_init_gpio_low();
#if 0
	ds1302_set_time();
	// setting clock
	ds1302_init_time_date();
#endif
	//flash_import_time_date();
	//ds1302_write(ADDR_WRITEPROTECTED, ds_time.writeprotected);

	while(1)
	{
		// 1. ds1302 read time
		ds1302_read_time();
		// 2. ds1302 read date
		ds1302_read_date();
		// 3. printf time, date
		printf("DATE : %4d - %2d - %2d   TIME : %2d : %2d : %2d\n"
				, ds_time.year + 2000, ds_time.month, ds_time.date, ds_time.hour, ds_time.minutes, ds_time.seconds);

		if (TIM4_ds1302_sec_counter >= 10000)
		{
			TIM4_ds1302_sec_counter = 0;
			flash_erase();
			flash_export_time_date();
		}
		// 4. delay 1sec
		HAL_Delay(1000);
	}
}

void ds1302_read_time(void)
{
	ds_time.seconds = ds1302_read(ADDR_SECONDS);
	ds_time.minutes = ds1302_read(ADDR_MINUTES);
	ds_time.hour = ds1302_read(ADDR_HOURS);
}

void ds1302_read_date(void)
{
	ds_time.date = ds1302_read(ADDR_DATE);
	ds_time.month = ds1302_read(ADDR_MONTH);
	ds_time.dayofweek = ds1302_read(ADDR_DAYOFWEEK);
	ds_time.year = ds1302_read(ADDR_YEAR);
}

void ds1302_init_time_date(void)
{
	ds1302_write(ADDR_SECONDS, ds_time.seconds);
	ds1302_write(ADDR_MINUTES, ds_time.minutes);
	ds1302_write(ADDR_HOURS, ds_time.hour);
	ds1302_write(ADDR_DAYOFWEEK, ds_time.dayofweek);
	ds1302_write(ADDR_DATE, ds_time.date);
	ds1302_write(ADDR_MONTH, ds_time.month);
	ds1302_write(ADDR_YEAR, ds_time.year);
}

void ds1302_init_gpio_low(void)
{
	HAL_GPIO_WritePin(CE_DS1302_GPIO_Port, CE_DS1302_Pin, 0);
	HAL_GPIO_WritePin(IO_DS1302_GPIO_Port, IO_DS1302_Pin, 0);
	HAL_GPIO_WritePin(CLK_DS1302_GPIO_Port, CLK_DS1302_Pin, 0);
}

void ds1302_write(uint8_t addr, uint8_t data)
{
	// 1. CE HIGH
	HAL_GPIO_WritePin(CE_DS1302_GPIO_Port, CE_DS1302_Pin, 1);
	// 2. Send Address
	ds1302_tx(addr);
	// 3. Send Data
	ds1302_tx(dec2bcd(data));
	// 4. CE LOW
	HAL_GPIO_WritePin(CE_DS1302_GPIO_Port, CE_DS1302_Pin, 0);
}


uint8_t ds1302_read(uint8_t addr)
{
	uint8_t data8bits = 0;

	// 1. CE HIGH
	HAL_GPIO_WritePin(CE_DS1302_GPIO_Port, CE_DS1302_Pin, 1);
	// 2. Send Address
	ds1302_tx(addr + 1); // read = write + 1
	// 3. Receive Data
	ds1302_rx(&data8bits);
	// 4. CE LOW
	HAL_GPIO_WritePin(CE_DS1302_GPIO_Port, CE_DS1302_Pin, 0);

	return bcd2dec(data8bits);
}

// data8 = 0
// 76543210 : Read from LSB
// When current bit is 0, set 0
void ds1302_rx(char *data8)
{
	uint8_t temp = 0;

	ds1302_DataLine_Input();

	for (int i = 0; i < 8; i++)
	{
		// 1. Read Bit
		if(HAL_GPIO_ReadPin(IO_DS1302_GPIO_Port, IO_DS1302_Pin))
		{
			temp |= 1 << i;
		}
		if (i != 7)
		{
			ds1302_clock();
		}
	}

	*data8 = temp;
}

void ds1302_tx(uint8_t tx)
{
	ds1302_DataLine_Output();
	// Write Second
	//    MSB    LSB
	// 80h 10000000
	// Out from LSB to MSB
	// check 0's bit 1 / 0
	// ( 10000000 & 00000001 => 00000000 )
	// ...
	// ( 10000000 & 10000000 => 10000000 )
	for (int i = 0; i < 8; i++)
	{
		if (tx & (1 << i))
		{ // bit is set state
			HAL_GPIO_WritePin(IO_DS1302_GPIO_Port, IO_DS1302_Pin, 1);
		}
		else
		{ // bit is reset state
			HAL_GPIO_WritePin(IO_DS1302_GPIO_Port, IO_DS1302_Pin, 0);
		}
		ds1302_clock(); // After sending commnad | data, Send clock.
	}

}

void ds1302_clock(void)
{
	HAL_GPIO_WritePin(CLK_DS1302_GPIO_Port, CLK_DS1302_Pin, 1);
	HAL_GPIO_WritePin(CLK_DS1302_GPIO_Port, CLK_DS1302_Pin, 0);
}

void ds1302_set_time(void)
{
	ds_time.year = 24;
	ds_time.month = 10;
	ds_time.date = 24;
	ds_time.dayofweek = 5;
	ds_time.hour = 14;
	ds_time.minutes = 17;
	ds_time.seconds = 0;
}


void ds1302_DataLine_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure GPIO pin : PH0 */
	GPIO_InitStruct.Pin = IO_DS1302_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;			//Change Output to Input
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(IO_DS1302_GPIO_Port, &GPIO_InitStruct);

	return;
}


void ds1302_DataLine_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure GPIO pin : PH0 */
	GPIO_InitStruct.Pin = IO_DS1302_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;			//Change Input to Output
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // LOW : 2MHz, HIGH : 25M~100MHz
	HAL_GPIO_Init(IO_DS1302_GPIO_Port, &GPIO_InitStruct);

	return;
}
