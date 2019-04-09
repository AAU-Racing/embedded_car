#include <stm32f4xx_hal.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <board_driver/uart.h>
#include <board_driver/can.h>
#include <board_driver/gpio.h>

#include <shield_driver/mainboard/gear.h>
int main(void) {
	uart_init();
	//Setting up CAN and it's filters
	if (can_init(CAN_PD0) != CAN_OK) {
	  printf("Error initializing CAN\n");
	}
	else {
	  printf("CAN init complete\n");
	}

	if (can_start() != CAN_OK) {
	  printf("Error starting CAN\n");
	}
	else {
	  printf("CAN started\n");
	}

	HAL_Delay(100);

	gpio_output_init(GPIOB, PIN_5);
	//gpio_output_init(GPIOD, PIN_13);

	//init_actuator();
	//printf("actuator init complete\n");

	HAL_Delay(1000);

	gpio_toggle_on(GPIOB, PIN_5);
	HAL_Delay(20);
	gpio_toggle_off(GPIOB, PIN_5);

	HAL_Delay(1000);

	//gpio_toggle_on(GPIOD, PIN_13);
	//HAL_Delay(20);
	//gpio_toggle_off(GPIOD, PIN_13);

}
