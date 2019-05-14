#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/uart.h>
#include <shield_driver/mainboard/actuator.h>
#include "../../newlib_calls.h"

int main(void) {
	debug_uart_init(DEV_DEBUG_UART);
	printf("uart init complete\n");

	HAL_Delay(100);

	init_actuator();
	printf("actuator init complete\n");

	HAL_Delay(5);

	while(1) {
		printf("actuator forward start\n");
		actuator_forward_start();

		HAL_Delay(20);

		printf("actuator forward stop\n");
		actuator_forward_stop();

		uint32_t start = HAL_GetTick();
		uint32_t last_print = 0;

		while (HAL_GetTick() - start < 5000) {
			if (HAL_GetTick() - last_print > 500) {
				printf("waiting\n");
				last_print = HAL_GetTick();
			}
		}

		printf("actuator backward start\n");
		actuator_backward_start();

		HAL_Delay(20);

		printf("actuator backward stop\n");
		actuator_backward_stop();

		start = HAL_GetTick();
		last_print = 0;

		while (HAL_GetTick() - start < 5000) {
			if (HAL_GetTick() - last_print > 500) {
				printf("waiting\n");
				last_print = HAL_GetTick();
			}
		}
	}
}
