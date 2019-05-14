#include <stdbool.h>
#include <stdint.h>
#include <bootloader.h>
#include <board_driver/usb/usb.h>
#include <board_driver/uart.h>

void (*send_func)(uint8_t, uint8_t);
uint8_t (*recv_func)(void);

bool LookForUpdate(void) {
    int i = 0;
    for(i = 0; i < 100; i++) {
        if(usb_recieve() == UPDATEREADY) {
            SetComType(USB);
            send_func(UPDATEACK, 1);
            return true;
        }
        else if(uart_read_byte() == UPDATEREADY) {
            SetComType(UART);
            send_func(UPDATEACK, 1);
            return true;
        }

        HAL_Delay(50);
    }

    return false;
}

void SetComType(ComType type) {
    if (type == USB) {
        send_func = usb_send;
        recv_func = usb_recieve;
    }
    else if (type == UART) {
        send_func = uart_send_byte;
        recv_func = uart_read_byte;
    }
}

void RecievePacket(Packet* packet) {
    while(recv_func() != 0xA1);
    packet->id = recv_func();
    packet->length = recv_func();
    int i;
    for (i = 0; i < packet->length; i++) {
        packet->payload[i] = recv_func();
    }
}

void SendPacket(Packet packet) {
    send_func(0xA1, 1);
    send_func(packet.id, 1);
    send_func(packet.length, 1);
    send_func(packet.payload, packet.length);
}

int convert(int* x, int length) {
  if (length == 2)
    return TWO(x);
  else if (length == 3)
    return THREE(x);
  else if (length == 4)
    return FOUR(x);
  else return -1;
}