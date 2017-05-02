#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <board_driver/usb/usb.h>


int main(void) {

	usb_init();

	// printf("Starting\n\n");
	// printf("Write something in the terminal and hit Enter:\n");

	while (1) {
		char *str = "Hellow usb\r\n";

		usb_transmit(str, sizeof(str)/sizeof(str[0]));

		// USBD_CDC_SetTxBuffer(&USBD_Device, (uint8_t*)str, sizeof(str)/sizeof(str[0]));
		// char input = getchar();
		// printf("getchat: %c\n", input);
		// HAL_Delay(100);
	}
}
