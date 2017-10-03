#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>

#include <stm32f4xx_hal.h>

typedef enum {
	GPIO_NORMAL,
	GPIO_EXTI,
} gpio_mode;

typedef void (*gpio_callback)(uint16_t);

void gpio_input_init(GPIO_TypeDef *port, uint32_t pin);
void gpio_exti_init(GPIO_TypeDef *port, uint32_t pin, uint32_t mode, gpio_callback callback);
void gpio_output_init(GPIO_TypeDef *port, uint32_t pin);
bool gpio_get_state(GPIO_TypeDef *port, uint32_t pin, GPIO_PinState* state);
bool gpio_toogle_on(GPIO_TypeDef *port, uint32_t pin);
bool gpio_toogle_off(GPIO_TypeDef *port, uint32_t pin);

#endif /* GPIO_H */
