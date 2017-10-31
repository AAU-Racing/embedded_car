#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>
#include <board_driver/usb/usb.h>


int main(void) {
	usb_init();
	HAL_Delay(3000);

	char str[] = "Starting\r\n";
	usb_transmit(str, sizeof(str)/sizeof(str[0]));

	while (1) {
		char str[] = "Hello usb\r\n";
		usb_transmit(str, sizeof(str)/sizeof(str[0]));

		HAL_Delay(500);
	}
}
