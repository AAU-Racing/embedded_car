#include "stdint.h"
#include "stdbool.h"

#ifndef BOOTLOADER_PROTOCOL_H
#define BOOTLOADER_PROTOCOL_H

#define CRCLENGTH 4
#define PAYLOADLENGTH 26

#define STARTID 		  0xA1

#define UPDATE			  0
#define STATUS			  1
#define ERROR_CODE		  2

#define DASHBOARD		  0
#define COMMNODE		  1

#define PREPARE_UPDATE	  0
#define TRANSFER_BEGIN    1
#define TRANSFER_CONTINUE 2
#define TRANSFER_END      3
#define RETRANSMIT		  4

#define SET_UPDATE	(UPDATE << 6)
#define SET_STATUS	(STATUS << 6)
#define SET_ERROR	(ERROR 	<< 6)

#define SET_DASHBOARD	(DASHBOARD << 4)
#define SET_COMMNODE	(COMMNODE  << 4)

#define SET_PREPARE_UPDATE		(PREPARE_UPDATE)
#define SET_TRANSFER_BEGIN		(TRANSFER_BEGIN)
#define SET_TRANSFER_CONTINUE	(TRANSFER_CONTINUE)
#define SET_TRANSFER_END		(TRANSFER_END)
#define SET_RETRANSMIT			(RETRANSMIT)

#define IS_UPDATE(ID) ((ID >> 6) & UPDATE)
#define IS_STATUS(ID) ((ID >> 6) & STATUS)
#define IS_ERROR(ID)  ((ID >> 6) & ERROR_CODE)

#define IS_DASHBOARD(ID) ((ID >> 4) & DASHBOARD)
#define IS_COMMNODE(ID) ((ID >> 4) & COMMNODE)

#define IS_PREPARE_UPDATE(ID) (ID & PREPARE_UPDATE)
#define IS_TRANSFER_BEGIN(ID) (ID & TRANSFER_BEGIN)
#define IS_TRANSFER_CONTINUE(ID) (ID & TRANSFER_CONTINUE)
#define IS_TRANSFER_END(ID) (ID & TRANSFER_END)
#define IS_RETRANSMIT(ID) (ID & RETRANSMIT)

typedef struct {
	uint8_t startId;
	uint8_t opId;
	uint8_t payload[PAYLOADLENGTH];
	uint32_t crc;
} Packet;

uint32_t get_image_length(Packet packet);
uint32_t get_rtc_value(Packet packet);
void receive_packet(Packet* packet);
void transmit_packet(Packet packet);
void create_packet(Packet* packet, uint8_t opId, uint8_t* payload);
void get_payload(Packet packet, uint8_t* arr, uint8_t* offset);
bool crc_is_valid(Packet packet);
void encrypt_payload(Packet *packet);
void decrypt_payload(Packet *packet);
uint32_t concatenate(uint32_t a, uint8_t b);

extern int (*receiveFunction)(Packet* packet);
extern int (*transmitFunction)(Packet packet);

#endif
