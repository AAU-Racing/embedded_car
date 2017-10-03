set(target_name traction_control)

set(SRC_FILES
	main.c
	send_data.c
)

add_executable(${target_name} ${SRC_FILES})
target_link_libraries(${target_name}
	board_driver
	shield_drivers
)
