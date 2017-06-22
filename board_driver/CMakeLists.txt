add_library(board_driver
	init.c
	uart.c
	adc.c
	flash.c
	can.c
	obdii.c
	usb/usbd_cdc_interface.c
	usb/usbd_conf.c
	usb/usbd_desc.c
	usb/usb.c
)

target_link_libraries(board_driver
	CMSIS
	STM32f4xx_HAL_Driver
	STM32_USB_Device_Library
)

target_compile_options(board_driver PRIVATE -Wno-unused-parameter) # TODO remove this and fix the warnings
