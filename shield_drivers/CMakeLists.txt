add_library(shield_drivers
	traction_control/water_temp.c
)

target_link_libraries(shield_drivers
	CMSIS
	STM32f4xx_HAL_Driver
	STM32_USB_Device_Library
)
