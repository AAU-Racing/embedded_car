#include <stm32f4xx_hal.h>
#include <stdint.h>
#include <stdbool.h>

#include "rpm.h"

#include <board_driver/obdii.h>

uint8_t get_rpm_level(){
	uint16_t rpm;
	get_rpm(&rpm);


	if(rpm < RPM_CRITICAL){
		uint8_t rpm_level = 10 - ((RPM_CRITICAL - rpm) / RPM_LEVEL_RANGE);
		if(rpm_level <= 10)
			return rpm_level;
		else
			return 0;
	}
	else
		return 11;	
}

bool get_rpm(uint16_t* rpm){
	OBDII_Mode1_Frame obd_rpm = OBDII_Mode1_Response(EngineRPM);
	*rpm = ((256 * obd_rpm.Msg[0]) + obd_rpm.Msg[1]) / 4;
	return obd_rpm.New;
}