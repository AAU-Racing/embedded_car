set(target_name update_rtc)

set(SRC_FILES
	updateRTC.c
)

add_executable(${target_name} ${SRC_FILES})
target_link_libraries(${target_name}
	board_driver
	shield_drivers
)
