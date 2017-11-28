#include <board_driver/dac.h>
#include <board_driver/uart.h>
#include <stdint.h>
#include <stdio.h>
#include <stm32f4xx_hal.h>

int main(void)
{

	uint8_t i = 0;

	uart_init();
	printf("Starting..\n");


	printf("Starting dac channel 1\n");
	init_DAC_channel_1();

	printf("Starting dac channel 2\n");	
	init_DAC_channel_2();

	while(1){
		output_DAC_channel_1(i);
		output_DAC_channel_2(i);

		HAL_Delay(50);
		i++;

	}
}

