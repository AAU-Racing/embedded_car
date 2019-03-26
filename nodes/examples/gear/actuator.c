#include <stm32f4xx_hal.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <board_driver/uart.h>
#include <board_driver/can.h>
#include <board_driver/gpio.h>

#include <shield_driver/mainboard/gear.h>
int main(void) {
	uart_init();
	printf("uart init complete\n");

	HAL_Delay(100);

	//init_actuator();
	//printf("actuator init complete\n");

	HAL_Delay(5);
    gpio_output_init(GPIOD, PIN_15);
    gpio_input_init(GPIOD, PIN_15, GPIO_PULL_UP);
    gpio_toggle_on(GPIOD, PIN_15);
    SET_BIT(GPIOD->ODR, 15);
    printf("%d\n", gpio_get_state(GPIOD, 15));
    //actuator_backward_start();
    while (1) {

    }
}
