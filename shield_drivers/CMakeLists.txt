add_library(shield_drivers
	traction_control/water_temp.c
	traction_control/hbridge.c
	traction_control/gear_feedback.c
	dashboard/TLC59116.c
	dashboard/buttons.c
	dashboard/steering_wheel_buttons.c
)

target_link_libraries(shield_drivers
	CMSIS
	STM32f4xx_HAL_Driver
	STM32_USB_Device_Library
	board_driver
)
