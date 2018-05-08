#include "uart_driver.h"
#include "board_driver/bootloader_protocol.h"
#include "board_driver/uart.h"

int (*receiveFunction)(Packet* packet) = &uart_driver_receive_packet;

int uart_driver_receive_packet(Packet* packet) {
	if(uart_read_byte() == 0xA1) {
		int i = 0;
		uint32_t crc = 0;

		packet->startId = 0xA1;

		packet->opId = uart_read_byte();

		for (i = 0; i < PAYLOADLENGTH; i++)
		{
			packet->payload[i] = uart_read_byte();
		}

		for (i = 0; i < CRCLENGTH; i++)
		{
			crc = concatenate(crc, uart_read_byte());
		}

		if(crc_is_valid(*packet)) {
			return 0;
		}
		else {
			return 1;
		}
	}
	else {
		return 1;
	}
}

int uart_driver_transmit_packet(Packet packet) {
	uart_send_byte(packet.startId);

	uart_send_byte(packet.opId);

	uart_send_buf(packet.payload, PAYLOADLENGTH);

	uart_send_buf((uint8_t*)packet.crc, CRCLENGTH);

	return 0;
}
