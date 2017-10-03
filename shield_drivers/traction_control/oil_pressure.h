#ifndef OIL_PRESSURE_H
#define OIL_PRESSURE_H

#include <stdbool.h>

#include <stm32f4xx_hal.h>

#define OIL_PRESSURE_PIN                    GPIO_PIN_13
#define OIL_PRESSURE_GPIO_PORT              GPIOE
#define OIL_PRESSURE_GPIO_CLK_ENABLE()      __GPIOE_CLK_ENABLE()

bool get_oil_pressure();
void init_oil_pressure();

#endif
