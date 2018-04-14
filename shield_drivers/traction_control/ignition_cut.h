#ifndef IGNITION_CUT_H
#define IGNITION_CUT_H

#include <board_driver/gpio.h>

#define IGNITION_CUT_PIN	PIN_8
#define IGNITION_CUT_PORT	GPIOE

#define init_ignition_cut()     gpio_output_init(IGNITION_CUT_PORT, IGNITION_CUT_PIN)
#define enable_ignition_cut()   gpio_toogle_on(IGNITION_CUT_PORT, IGNITION_CUT_PIN)
#define disable_ignition_cut()  gpio_toogle_off(IGNITION_CUT_PORT, IGNITION_CUT_PIN)

#endif
