#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stm32f4xx_hal.h>
#include <shield_drivers/dashboard/ea_dogxl160w-7.h>
#include <board_driver/spi.h>

int main (void)
{
  init_lcd();
  paintString(2, 2, "Hello World", 1, &Font8);

	return 0;
}
