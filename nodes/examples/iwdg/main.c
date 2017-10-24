#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/usb/usb.h>
#include <board_driver/iwdg.h>


int main(void) {
	usb_init();
  HAL_Delay(1000);
  setup_IWDG();
  init_IWDG();
  char start[] = "Starting\r\n";
  usb_transmit(start, sizeof(start)/sizeof(start[0]));

  while (1) {
		char str[] = "Hello usb\r\n";
		usb_transmit(str, sizeof(str)/sizeof(str[0]));
    HAL_Delay(500);
	}
}
