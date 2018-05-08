add_library(device_driver
	uart_driver.c
)

target_link_libraries(device_driver
	CMSIS
	STM32f4xx_HAL_Driver
	STM32_USB_Device_Library
	board_driver
)
