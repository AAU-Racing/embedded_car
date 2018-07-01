#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include <stm32f4xx_hal.h>

#include <board_driver/uart.h>
#include <board_driver/adc.h>
#include <board_driver/float_print.h>
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

	for (uint16_t i = 0; ; i++) {
		int in = read_water_in();
		int out = read_water_out();

    float water_in = -27.271952718 * log(10000 * in / (4095.0 - in)) + 240.1851825535;
    float water_out = -27.271952718 * log(10000 * out / (4095.0 - out)) + 240.1851825535;

    printf("%5d, %4d, %4d, ", i, in, out);
    print_double(water_in, 5, 2);
    printf(", ");
    print_double(water_out, 5, 2);
    printf("\n");

		HAL_Delay(2);
	}
}
