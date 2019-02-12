#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/gpio.h>
#include <board_driver/uart.h>

#include <shield_driver/mainboard/gear.h>

#define GEARGPIO GPIOB
#define GEARUP PIN_5
#define GEARDOWN PIN_4

bool change_up_ok = true;
bool change_down_ok = true;

uint8_t gear = 0;
uint8_t wanted_gear = 0;

uint8_t gear_number() {
    return gear;
}

void wanted_gear_up() {
    if(gear < 6) {
        gear++;
    }
}

void wanted_gear_down() {
    if(gear > 0) {
        gear--;
    }
}

int main(void) {

    uart_init();

    gpio_input_init(GEARGPIO, GEARUP, GPIO_PULL_DOWN);
    gpio_input_init(GEARGPIO, GEARDOWN, GPIO_PULL_DOWN);

    while(1) {
        if(gpio_get_state(GEARGPIO, GEARUP) && change_up_ok == true) {
            change_up_ok = false;
            //wanted_gear_up();
            //change_gear();
            wanted_gear_up();
            printf("gear up: %d\n\r", gear_number());
            HAL_Delay(200);
        }
        else if(gpio_get_state(GEARGPIO, GEARDOWN) && change_down_ok == true) {
            change_down_ok = false;
            //wanted_gear_down();
            //change_gear();
            wanted_gear_down();
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
