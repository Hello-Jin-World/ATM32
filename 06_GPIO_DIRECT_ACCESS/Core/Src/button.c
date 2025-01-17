#include "button.h"
#include "led.h"

void button_check(void);
int get_button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, int button_num);
int read_pin(uint16_t GPIO_Pin);

/*
void (*fp[])() =
{
	led_all_off,
	led_all_on,
	led_up_on,
	led_down_on,
	led_keep_on_up,
	led_keep_on_down,
	flower_on,
	flower_off,

};
 */

unsigned char button_status[BUTTON_NUMBER] = {
		BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE
};


//When press the button, turn on led 1 step. If led all on, turn off 1 step. loop
void button_check(void)
{
	static int led_on = 0;
	static int mode = 0;

	//fp[mode]();

	if (get_button(GPIOC, GPIO_PIN_0, BUTTON0) == BUTTON_PRESS)
	{
		mode++;
		mode %= 4;
	}
	else if (get_button(GPIOC, GPIO_PIN_1, BUTTON1) == BUTTON_PRESS)
	{
		if (mode == 4)
		{
			mode = 5;
		}
		else
		{
			mode = 4;
		}

	}
	else if (get_button(GPIOC, GPIO_PIN_2, BUTTON2) == BUTTON_PRESS)
	{

		if (mode != 6)
		{
			mode = 6;
		}
		else if (mode == 6)
		{
			mode = 7;
		}
	}
	else if (get_button(GPIOC, GPIO_PIN_3, BUTTON3) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
	}
	// check one click

	if (get_button(GPIOC, GPIO_PIN_13, BUTTON4) == BUTTON_PRESS)
	{
		led_on %= 16;
		HAL_GPIO_WritePin(GPIOB, 0x01 << led_on, 1);
		led_on++;

		if (led_on > 8)
		{
			HAL_GPIO_WritePin(GPIOB, 0x80 >> (led_on+6)%15, 0);
		}
	}
}
// if one click status, return 1
int get_button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, int button_num) // ex) GPIOC, GPIO_PIN_13, 0
{
	int state; // store in register

#if 0
	state = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin); // active : 0, inactive : 1
#else
	state = read_pin(GPIO_Pin);

	if (state == BUTTON_PRESS && button_status[button_num] == BUTTON_RELEASE)
	{
		HAL_Delay(60); // For noise remove delay
		button_status[button_num] = BUTTON_PRESS; // Not first button press
		return BUTTON_RELEASE;
	}
	else if (button_status[button_num] == BUTTON_PRESS && state == BUTTON_RELEASE) // Previous, button is pressed but Now, inactive status
	{
		button_status[button_num] = BUTTON_RELEASE;
		HAL_Delay(60);
		return BUTTON_PRESS;
	}
	return BUTTON_RELEASE;
#endif

}

int read_pin(uint16_t GPIO_Pin)
{
	  if ((*(unsigned int *) 0x40020810 & GPIO_Pin) != 0x0000)
	  {
		  return BUTTON_PRESS;
	  }

	  else
	  {
		  return BUTTON_RELEASE;
	  }
}
