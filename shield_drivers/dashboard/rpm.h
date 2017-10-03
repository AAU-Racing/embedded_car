#include <stm32f4xx_hal.h>
#include <stdint.h>
#include <stdbool.h>

#define RPM_CRITICAL 12000
#define RPM_LEVEL_RANGE 800

uint8_t get_rpm_level();
bool get_rpm(uint16_t* rpm);