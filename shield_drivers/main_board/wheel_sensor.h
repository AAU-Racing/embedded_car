#ifndef WHEEL_SENSOR_H
#define WHEEL_SENSOR_H

#include <stm32f4xx_hal.h>

enum wheel_input {
	WHEEL_INP_FR, // FRONT RIGHT
	WHEEL_INP_FL, // FRONT LEFT
	WHEEL_INP_BR, // BACK RIGHT
	WHEEL_INP_BL, // BACK LEFT

	WHEEL_INP_N, // Number of connected wheels
};

// union: https://www.ccsinfo.com/forum/viewtopic.php?p=103221
typedef union {
	float fl;
	uint8_t bytes[4];
} serializable_float;

#define RPM1 WHEEL_INP_FR
#define RPM1_GPIO_PORT GPIOD
#define RPM1_PIN PIN_4

#define RPM2 WHEEL_INP_FL
#define RPM2_GPIO_PORT GPIOD
#define RPM2_PIN PIN_3

#define RPM3 WHEEL_INP_BR
#define RPM3_GPIO_PORT GPIOD
#define RPM3_PIN PIN_2

#define RPM4 WHEEL_INP_BL
#define RPM4_GPIO_PORT GPIOD
#define RPM4_PIN PIN_7

void wheel_sensor_init(void);

serializable_float wheel_sensor_fr(void);
serializable_float wheel_sensor_fl(void);
serializable_float wheel_sensor_br(void);
serializable_float wheel_sensor_bl(void);

#endif /* WHEEL_SENSOR_H */
