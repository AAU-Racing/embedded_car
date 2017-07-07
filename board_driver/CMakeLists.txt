add_library(board_driver
	init.c
	uart.c
	flash.c
	rtc.c
)

target_link_libraries(board_driver
	CMSIS
	STM32f4xx_HAL_Driver
)
