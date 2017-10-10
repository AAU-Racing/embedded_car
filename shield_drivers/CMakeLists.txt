add_library(shield_drivers
	traction_control/water_temp.c
	traction_control/wheel_sensor.c
	traction_control/brake_pressure.c
	traction_control/gear_feedback.c
	traction_control/gear.c
	traction_control/oil_pressure.c
	traction_control/hbridge.c
	traction_control/current_clamps.c

	dashboard/buttons.c
	dashboard/steering_wheel_buttons.c
	dashboard/rpm.c
	dashboard/gear.c
	dashboard/oil.c
	dashboard/TLC59116.c
	dashboard/ea_dogxl160w-7.c
	dashboard/Fonts/font8.c
	dashboard/Fonts/font12.c
	dashboard/Fonts/font16.c
	dashboard/Fonts/font20.c
	dashboard/Fonts/font24.c

	#com_node/sd.c
	#com_node/sd_setup.c
	com_node/water_temperature.c
	com_node/current_clamps.c
	com_node/wheel_speed.c
	com_node/xbee.c
	com_node/error.c
)

target_link_libraries(shield_drivers
	CMSIS
	STM32f4xx_HAL_Driver
	STM32_USB_Device_Library
	board_driver
)
