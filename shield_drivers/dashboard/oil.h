#include <stm32f4xx_hal.h>

#include <board_driver/can.h>

bool oilPressure_OK();
void oil_pressure_Callback(CanRxMsgTypeDef* Msg);
HAL_StatusTypeDef oil_init(void);