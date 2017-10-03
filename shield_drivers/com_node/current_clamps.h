#include <stm32f4xx_hal.h>
#include <stdbool.h>

bool get_battery_current(uint16_t* current);
bool get_inverter_current(uint16_t* current);
HAL_StatusTypeDef current_clamps_init(void);
