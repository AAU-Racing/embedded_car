#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/uart.h>
#include <board_driver/adc.h>
#include <shield_drivers/traction_control/water_temp.h>

int main(void) {
	uart_init();
	printf("uart init complete\n");

	HAL_Delay(100);

	init_adc(2);
	printf("adc init complete\n");

	init_water_temp();
	printf("water sensor init complete\n");

	HAL_Delay(10);

	start_adc();
	printf("adc started\n");
	HAL_Delay(10);

	uint16_t i = 0;

	while (1) {
		HAL_Delay(2);
		i++;
		int in = read_water_in();
		int out = read_water_out();
		printf("(%d, %d, %d)\n", i, in, out);
	}
}
