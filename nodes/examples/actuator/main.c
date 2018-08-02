#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/uart.h>
#include <shield_drivers/main_board/actuator.h>

int main(void) {
	uart_init();
	printf("uart init complete\n");

	HAL_Delay(100);

	init_actuator();
	printf("actuator init complete\n");

	HAL_Delay(5);

	while(1) {
		printf("actuator forward slow\n");
		actuator_forward_start();

		HAL_Delay(10000);

		printf("actuator forward stop\n");
		actuator_forward_stop();

		printf("actuator backward start\n");
		actuator_backward_slow();

		HAL_Delay(10000);

		printf("actuator backward stop\n");
		actuator_backward_stop();
	}
}
