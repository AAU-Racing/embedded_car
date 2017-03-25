#include <stdint.h>
#include <board_driver/init.h>
#include <board_driver/interrupts.h>

extern uint32_t _start_addr;


void _init(void) {
	init_board();
	set_system_clock_168mhz();
	set_interrupt_offset((uint32_t)&_start_addr);
	enable_all_interrupts();
}

void _exit(int exitcode) {
	(void)exitcode;
	while(1);
}
