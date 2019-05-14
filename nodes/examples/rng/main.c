#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stm32f4xx_hal.h>
#include <board_driver/rng.h>
#include <board_driver/uart.h>

#include "../../newlib_calls.h"

int main (void)
{
	debug_uart_init(DEV_DEBUG_UART);
	rng_init();

	printf("Starting Random Generation:\n");

	while(1)
	{
		HAL_Delay(500);
		printf("%u\n", (unsigned int) get_rng(1,101));
	}

	return 0;
}
