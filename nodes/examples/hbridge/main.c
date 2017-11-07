#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/uart.h>
#include <shield_drivers/traction_control/hbridge.h>

int main(void) {
	uart_init();
	printf("uart init complete\n");

	HAL_Delay(100);

	init_hbridge();
	printf("hbridge init complete\n");

	HAL_Delay(5);

	printf("all enable and direction pins toggled on\n");
	hbridge1forward();

	HAL_Delay(10);
}
