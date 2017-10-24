#ifndef OIL_PRESSURE_H
#define OIL_PRESSURE_H

#include <stdbool.h>

#include <stm32f4xx_hal.h>

#define OIL_PRESSURE_PIN                    PIN_13
#define OIL_PRESSURE_GPIO_PORT              GPIOE

bool get_oil_pressure();
void init_oil_pressure();

#endif
