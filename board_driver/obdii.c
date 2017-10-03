#include <stm32f4xx_hal.h>
#include <stdbool.h>

#include "can.h"
#include "obdii.h"

#define FILL 0x55
#define MSG_LEN 8
#define DTC_LEN 2
#define DTC_FIRST_FRAME 2
#define DTC_PR_FRAME 3

static uint8_t OBDII_FilterNum = 0;

typedef enum {
	Single      = 0,
	First       = 1,
	Consecutive = 2,
	Flow        = 3,
	Unknown     = -1,
} MF_CANMode;

typedef struct {
	OBDII_Frame array[20];
	uint8_t count;
} Buffer;

Buffer buffer;

static OBDII_Frame CAN_TO_OBDII(CAN_Frame* can_frame) {
	OBDII_Frame frame = (OBDII_Frame) {
		.Mode   = (OBDII_Mode) can_frame->Msg[1] - 40,
		.Pid    = (OBDII_Pid) can_frame->Msg[2],
		.Length = can_frame->Msg[0] - 2,
	};
	for (uint8_t i = 0; i < frame.Length; i++) {
		frame.Msg[i] = can_frame->Msg[4 + i];
	}
	return frame;
}

static MF_CANMode MultiFrameMode(const CAN_Frame* can_frame) {
	return can_frame->Msg[0] >> 4 & 0xF;
}

static uint8_t SingleFrameMessageAmount(const CAN_Frame* can_frame) {
	return (can_frame->Msg[0] & 0xF) / DTC_LEN;
}

static uint16_t MultiFrameMessageAmount(const CAN_Frame* can_frame) {
	return ((can_frame->Msg[0] & 0xF) << 4 | can_frame->Msg[1]) / DTC_LEN;
}

static DTC_Message ByteToDTC(const uint8_t *message) {
	return (DTC_Message) {
		.FirstChar  = message[0] >> 6 & 0x3,
		.SecondChar = message[0] >> 4 & 0x3,
		.ThirdChar  = message[0] & 0xF,
		.FourthChar = message[1] >> 4 & 0xF,
		.FifthChar  = message[1] & 0xF,
	};
}

void OBDII_Init() {
	OBDII_FilterNum = CAN_Filter(OBDII_RESPONSE_ID, OBDII_RESPONSE_ID);
}

bool GetTroubleCodes(DTC_Message message[], size_t* len) {
	static bool running = false;
	static MF_CANMode frameType = Unknown;
	static size_t length = 0;
	static size_t messageCount = 0;
	static uint8_t frameIndex = 1;
	if (!running) {
		// Diagnostics trouble code request
		CAN_Send(OBDII_REQUEST_ID, (uint8_t[])  {1, DTC, FILL, FILL, FILL, FILL, FILL, FILL}, MSG_LEN);
		running = true;
	}

	CAN_Frame received;
	if (frameType == Unknown) {
		if (CAN_Receive(OBDII_FilterNum, &received)) {
			frameType = MultiFrameMode(&received);
		}
		else {
			return false;
		}
	}

	if (frameType == Single) {
		*len = SingleFrameMessageAmount(&received);

		// Get DTC in single frame
		for (uint8_t i = 0; i < *len; i++) {
			message[i] = ByteToDTC(received.Msg + 1 + i * 2);
		}

		running = false;
		frameType = Unknown;
		return true;
	}
	else if (frameType == First) {
		// Send flow control
		CAN_Send(OBDII_REQUEST_ID, (uint8_t[]) {Flow << 4, 0, 0, FILL, FILL, FILL, FILL, FILL}, MSG_LEN);

		// Handle DTC in first frame
		message[0] = ByteToDTC(received.Msg + 1);
		message[1] = ByteToDTC(received.Msg + 3);

		messageCount += 2;
		length = MultiFrameMessageAmount(&received);
	}

	// While there are more messages
	while (messageCount < length - 1) {
		if (CAN_Receive(OBDII_FilterNum, &received)) {
			if ((received.Msg[0] & 0xFF) != frameIndex) // Check that index order is correct
			{   } // Error
			frameIndex = (frameIndex + 1) % 0xF;

			// For all DTC in frame
			for (uint8_t j = 0; j < DTC_PR_FRAME; j++) {
				if (messageCount < length - 1) {
					message[messageCount++] = ByteToDTC(received.Msg + (j + 1) * DTC_LEN);
				}
			}
		}
		else {
			return false;
		}
	}

	*len = length;

	running = false;
	frameType = Unknown;
	length = 0;
	messageCount = 0;
	frameIndex = 1;
	return true;
}

void ClearTroubleCodes() {
	CAN_Send(OBDII_REQUEST_ID, (uint8_t[]) {1, ClearDTC, FILL, FILL, FILL, FILL, FILL, FILL}, MSG_LEN);
}

void OBDII_Mode1_Request(OBDII_Pid pid) {
	assert_param(pid != FreezeDTC);
	CAN_Send(OBDII_REQUEST_ID, (uint8_t[]) {2, ShowCurrent, pid, FILL, FILL, FILL, FILL, FILL}, MSG_LEN);
}

void OBDII_Mode2_Request(OBDII_Pid pid) {
	assert_param(pid != MonitorStatus);
	CAN_Send(OBDII_REQUEST_ID, (uint8_t[]) {2, ShowFreeze, pid, FILL, FILL, FILL, FILL, FILL}, MSG_LEN);
}

OBDII_Frame OBDII_Response(OBDII_Mode mode, OBDII_Pid pid) {
	OBDII_Frame frame = {0};
	for (uint8_t i = 0; i < buffer.count; i++) {
		if (buffer.array[i].Mode == mode && buffer.array[i].Pid == pid) {
			frame = buffer.array[i];
			buffer.count--;
			for (uint8_t j = i; j < buffer.count; j++)
			buffer.array[j] = buffer.array[j + 1];

			return frame;
		}
	}

	CAN_Frame received_frame;

	while(CAN_Receive(OBDII_FilterNum, &received_frame)) {
		frame = CAN_TO_OBDII(&received_frame);

		if (mode == frame.Mode && pid == frame.Pid) {
			return frame;
		}
		else {
			buffer.array[buffer.count++] = frame;
		}
	}

	return (OBDII_Frame) {0};
}
