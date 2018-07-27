#include <stm32f4xx_hal.h>

#include <shield_drivers/dashboard/TLC59116.h>

#include <stdint.h>

#include <shield_drivers/dashboard/rpm.h>
#include <board_driver/can.h>

int main(void) {
	led_driver_init(false);
	CAN_Init(CAN_PD0);
	HAL_Delay(10);
	CAN_Start();

	while(1) {
		for (uint8_t color = 0; color < 3; color++) {
			for (uint8_t led = 0; led < 25; led++) {
				uint8_t red = (color == 0) * 0xFF;
				uint8_t green = (color == 1) * 0xFF;
				uint8_t blue = (color == 2) * 0xFF;

				set_led(led, red, green, blue);

				HAL_Delay(25);
			}
		}
	}
}
