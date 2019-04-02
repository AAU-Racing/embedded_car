#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/gpio.h>
#include <board_driver/can.h>
#include <board_driver/uart.h>

#include <shield_driver/mainboard/gear.h>

#define GEARGPIO GPIOD
#define GEARUP PIN_12
#define GEARDOWN PIN_13

bool change_up_ok = true;
bool change_down_ok = true;
uint8_t gear = 0;

int main(void) {

    uart_init();
    //Setting up CAN and it's filters
    if (can_init(CAN_PD0) != CAN_OK) {
      printf("Error initializing CAN\n");
    }
    else {
      printf("CAN init complete\n");
    }

    if (can_start() != CAN_OK) {
      printf("Error starting CAN\n");
    }
    else {
      printf("CAN started\n");
    }


    //gpio_input_init(GEARGPIO, GEARUP, GPIO_PULL_DOWN);
    //gpio_input_init(GEARGPIO, GEARDOWN, GPIO_PULL_DOWN);

    gpio_output_init(GEARGPIO, GEARUP);
    gpio_output_init(GEARGPIO, GEARDOWN);

    gpio_toggle_on(GEARGPIO, GEARUP);
    HAL_Delay(200);
    gpio_toggle_on(GEARGPIO, GEARDOWN);
    HAL_Delay(200);


    while(1) {
        if(gpio_get_state(GEARGPIO, GEARUP) && change_up_ok == true) {
            change_up_ok = false;
            //gear_up();
            can_transmit(CAN_GEAR_BUTTONS, (uint8_t[]) {CAN_GEAR_BUTTON_UP}, 1);
            printf("gear up: %d\n\r", gear);
            HAL_Delay(200);
        }
        else if(gpio_get_state(GEARGPIO, GEARDOWN) && change_down_ok == true) {
            change_down_ok = false;
            //gear_down();
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

/*void gear_up() {
  if (gear < 6) {
    gear++;
  }
}

void gear_down() {
  if (gear > 0) {
    gear--;
  }
}*/
