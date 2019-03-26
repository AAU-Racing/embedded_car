set(target_name example_gear_dashboard)

set(SRC_FILES
	main.c
)

add_executable(${target_name} ${SRC_FILES})
target_link_libraries(${target_name}
	board_driver
	shield_driver
)

set(target_name example_gear_mainboard)

set(SRC_FILES
  mainboardtest.c
)

add_executable(${target_name} ${SRC_FILES})
target_link_libraries(${target_name}
  board_driver
  shield_driver
)

set(target_name example_gear_actuator)

set(SRC_FILES
  actuator.c
)

add_executable(${target_name} ${SRC_FILES})
target_link_libraries(${target_name}
  board_driver
  shield_driver
)
