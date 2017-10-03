#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <shield_drivers/traction_control/water_temp.h>

int main(void) {

	init_water_temp();

	while (1) {
		int in = read_water_in();
		int out = read_water_out();

		HAL_Delay(500);
	}
}
