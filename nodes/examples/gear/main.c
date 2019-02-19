#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/gpio.h>
#include <board_driver/can.h>
#include <board_driver/uart.h>

#include <shield_driver/mainboard/gear.h>

#define GEARGPIO GPIOB
#define GEARUP PIN_5
#define GEARDOWN PIN_4

bool change_up_ok = true;
bool change_down_ok = true;

int main(void) {

    uart_init();

    gpio_input_init(GEARGPIO, GEARUP, GPIO_PULL_DOWN);
    gpio_input_init(GEARGPIO, GEARDOWN, GPIO_PULL_DOWN);

    while(1) {
        if(gpio_get_state(GEARGPIO, GEARUP) && change_up_ok == true) {
            change_up_ok = false;
            //wanted_gear_up();
            //change_gear();
            can_transmit(CAN_GEAR_NUMBER, (uint8_t[]) {gear}, 1);
            printf("gear up: %d\n\r", gear_number());
            HAL_Delay(200);
        }
        else if(gpio_get_state(GEARGPIO, GEARDOWN) && change_down_ok == true) {
            change_down_ok = false;
            //wanted_gear_down();
            //change_gear();
            printf("gear down: %d\n\r", gear_number());
            HAL_Delay(200);
        }
        
        if((!gpio_get_state(GEARGPIO, GEARUP)) && change_up_ok == false) {
            change_up_ok = true;
        }

        if((!gpio_get_state(GEARGPIO, GEARDOWN)) && change_down_ok == false) {
            change_down_ok = true;
        }
    }
}
