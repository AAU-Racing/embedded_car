#include "board_driver/bootloader_protocol.h"

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

int uart_driver_receive_packet(Packet* packet);
int uart_driver_transmit_packet(Packet packet);

#endif