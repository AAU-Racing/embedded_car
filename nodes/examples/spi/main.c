#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stm32f4xx_hal.h>
#include <shield_driver/ea_dogxl160w-7.h>
#include <board_driver/spi.h>
#include <shield_driver/dashboard/Fonts/font8.c>

int main (void)
{
  SPI_init(void);
  init_lcd(void);
  paintString(2, 2, "Hello World", 1, sFONT Font8);

	return 0;
}
