#include <stm32f4xx_hal.h>
#include <stdbool.h>

bool get_water_temp_in(uint16_t* temp);
bool get_water_temp_out(uint16_t* temp);
HAL_StatusTypeDef water_temp_init(void);
