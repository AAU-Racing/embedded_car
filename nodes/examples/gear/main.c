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
uint8_t gear = 0;

int main(void) {

    uart_init();

    gpio_input_init(GEARGPIO, GEARUP, GPIO_PULL_DOWN);
    gpio_input_init(GEARGPIO, GEARDOWN, GPIO_PULL_DOWN);

    while(1) {
        if(gpio_get_state(GEARGPIO, GEARUP) && change_up_ok == true) {
            change_up_ok = false;
            gear_up();
            can_transmit(CAN_GEAR_BUTTONS, (uint8_t[]) {CAN_GEAR_BUTTON_UP}, 1);
            printf("gear up: %d\n\r", gear);
            HAL_Delay(200);
        }
        else if(gpio_get_state(GEARGPIO, GEARDOWN) && change_down_ok == true) {
            change_down_ok = false;
            gear_down();
            can_transmit(CAN_GEAR_BUTTONS, (uint8_t[]) {CAN_GEAR_BUTTON_DOWN}, 1);
            printf("gear down: %d\n\r", gear);
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

if gear_up() {
  if (gear < 6) {
    gear++;
  }
}

if gear_down() {
  if (gear > 0) {
    gear--;
  }
}
