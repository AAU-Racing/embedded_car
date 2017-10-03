#include <stm32f4xx_hal.h>
#include <stdbool.h>

#define POWER_INTERUPT_PIN                    GPIO_PIN_8
#define POWER_INTERUPT_GPIO_PORT              GPIOE
#define POWER_INTERUPT_GPIO_CLK_ENABLE()      __GPIOE_CLK_ENABLE()


void power_interupt_pin_init(void);
bool power_get_state();