#include <stm32f4xx_hal.h>

#include <shield_drivers/dashboard/TLC59116.h>

#include <stdint.h>

#include <shield_drivers/dashboard/rpm.h>

int main(void) {
	led_driver_init(true);

	while(1) {
		for (uint8_t color = 0; color < 1; color++) {
			for (uint8_t led = 0; led < 20; led++) {
				uint8_t red = (color == 0) * 0xF;
				uint8_t green = (color == 1) * 0xF;
				uint8_t blue = (color == 2) * 0xF;

				set_led(led, red, green, blue);

				HAL_Delay(200);
			}
		}
	}
}
