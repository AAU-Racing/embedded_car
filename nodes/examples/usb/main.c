#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <board_driver/usb/usb.h>


int main(void) {

	usb_init();

	printf("Starting\n\n");
	printf("Write something in the terminal and hit Enter:\n");

	while (1) {
		char input = getchar();
		printf("getchat: %c\n", input);
		// HAL_Delay(100);
	}
}
