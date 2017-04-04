add_library(board_driver
	init.c
	interrupts.c
	uart.c
	flash.c
	can.c
	obdii.c
)

target_link_libraries(board_driver
	CMSIS
	STM32f4xx_HAL_Driver
)
