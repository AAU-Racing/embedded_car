set(BOOT_START 0x08000000)
set(APP_START 0x08010000)
set(LDADDR ${BOOT_START})

set(ldscript_path ${CMAKE_BINARY_DIR}/nodes/ldscript.ld)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-T${ldscript_path}")

add_custom_command(OUTPUT ldscript.ld
	COMMAND gcc -P -E ${CMAKE_SOURCE_DIR}/nodes/ldscript.c -o ldscript.ld -DLDADDR=${LDADDR}
	MAIN_DEPENDENCY ldscript.c
	COMMENT "Using program entry address: ${LDADDR}"
)

add_custom_target(ldscript DEPENDS ldscript.ld)

# This overwrites the add_executable for all sub executables and generates a
# target for writing to flash and a binary.
# This also links all targets with linkerscript and newlib_calls.c
function(add_executable target_name)
	_add_executable(${target_name}
		${CMAKE_SOURCE_DIR}/nodes/newlib_calls.c
		${ARGN}
	)
	add_dependencies(${target_name} ldscript)
	set_property (
		TARGET ${target_name}
		PROPERTY LINK_DEPENDS ${ldscript_path}
	)
	open_ocd_write_flash(${target_name})
	add_custom_command(OUTPUT ${target_name}.bin
		POST_BUILD COMMAND arm-none-eabi-objcopy ${target_name} -O binary ${target_name}.bin
		DEPENDS ${target_name}
	)
	add_custom_target(${target_name}_bin ALL
		DEPENDS ${target_name}.bin
	)
endfunction()

add_subdirectory(examples/uart)
add_subdirectory(examples/usb)
