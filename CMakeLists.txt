cmake_minimum_required(VERSION 2.8.8)

include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)

set(CMAKE_SYSTEM_NAME Generic)
project(AAU-Racing C ASM)

# Following flags helps with dead code elimination.
# More info can found at http://stackoverflow.com/a/10809541
set(gc_flags "-ffunction-sections -fdata-sections -Wl,--gc-sections")
set(mcu_flags "-mcpu=cortex-m4 -mtune=cortex-m4 -mthumb -mlittle-endian -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb-interwork")
set(linker_flags "-Wl,-gc-sections -Wl,-LTO -nostartfiles")
set(WARNINGS "-Wall -Wextra")
set(CFLAGS "-O0 -g -LTO -std=c11 ${WARNINGS} ${gc_flags} ${mcu_flags} ${linker_flags}")
set(CMAKE_C_FLAGS "${CFLAGS} ${CMAKE_C_FLAGS}")
add_definitions(-DHSE_VALUE=16000000 -DSTM32F415xx)

include_directories(
	# Root
	${CMAKE_SOURCE_DIR}

	# CMSIS
	libs/CMSIS/Include
	libs/CMSIS/stm32f415xx

	# STM32 HAL
	libs/STM32F4xx_HAL_Driver/Inc
)

# Make it possible to Flash using openOCD
find_program(OPEN_OCD openocd)
get_filename_component(open_ocd_path ${OPEN_OCD} DIRECTORY)
set(OPEN_OCD_CONFIG "${open_ocd_path}/../share/openocd/scripts/board/stm32f4discovery.cfg")
function(open_ocd_write_flash elf_file)
	add_custom_target(${elf_file}_writeflash
		COMMAND ${OPEN_OCD}
			-f ${OPEN_OCD_CONFIG}
			-c "init; reset halt"
			-c "flash write_image erase ${elf_file}"
			-c "reset run; exit;"
		DEPENDS ${elf_file}
		VERBATIM
	)
endfunction()


add_subdirectory(board_driver)
add_subdirectory(libs)
add_subdirectory(bootloader)

# The directory 'nodes' must be added last since it overides
# the 'add_executable' and 'add_library' functions.
add_subdirectory(nodes)
