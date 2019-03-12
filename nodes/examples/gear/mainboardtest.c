#include <stm32f4xx_hal.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <board_driver/uart.h>
#include <board_driver/can.h>

#include <shield_driver/mainboard/gear.h>

void setup(void);
void loop(void);

// A main function in the philosophy of Arduino (setup + loop)
int main(void) {
  setup();

  while(1){
    loop();
  }

  return 0;
}

void setup(void){
  uart_init();
  printf("\n\n");
  printf("UART init complete\n");

  //Setting up CAN and it's filters
  if (can_init(CAN_PD0) != CAN_OK) {
    printf("Error initializing CAN\n");
  }
  else {
    printf("CAN init complete\n");
  }

  init_gear();
  printf("Init gear complete\n");

  if (can_start() != CAN_OK) {
    printf("Error starting CAN\n");
  }
  else {
    printf("CAN started\n");
  }

  // Wait for everything to be ready
  HAL_Delay(25);

  read_initial_gear();
  printf("Initial gear is %d\n", gear_number());

  // Signal all others that the main board is ready
  can_transmit(CAN_MAIN_BOARD_STARTED, (uint8_t[]) { 1 }, 1);
}

void loop(void){
  change_gear();

  //printf("Current gear: %d\n", gear_number());
  HAL_Delay(100);
}
