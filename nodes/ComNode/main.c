/*
Status on protocol:

Generel behavior:
	Until handshake is established it the car will only react on handshake packages
	Function to package payloads is ready to recive data (must know lenght of payload)
	Checksum is calculated when recieving pakage and also when packaging to send
	Currently setup to send telemetry once handshake is established
		Should this be done regardless of handshakeState?
		Getting telemetry data to send is not yet completed

Recieve handshake:
	Done:
		Will recieve handshake and if the checksum is correct ít will establish handshake
		Send header back - Needs to get the header data thoguh
	
	To be completed:
		Use payload from handshake to set system time
		Templates for these functions are made

Recieve/Send Ack/Nack:
	Done:
		If checksum is correct the ack/nack will be stored
			Else nack is send and Ack/Nack is set to 2 (CAUTION)
		Ack nack can be send with sendAckNack(1/0) depending on ack/nack

Recieving Request:
	Done:
		Revieves request and returns whether it is handeling a request or it sends nack

	To be completed:
		Is ready to read first byte in payload as request type and react
			but no types of request are established
			and can't yet fetch the data that is asked for

		No kind of managment of big datasets to be send as responses
			could be with a buffer...

Recieving Data:
	There is a function to react but no case in which it will be used.
*/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <board_driver/uart.h>

#define COM_TYPE_HANDSHAKE 0
#define COM_TYPE_ACK_NACK 1
#define COM_TYPE_REQ_RES 2
#define COM_TYPE_DATA 3

#define COM_STD_BYTE1 0xA1

#define COM_REQ_HALT 0

typedef struct{
	uint8_t type;
	uint8_t length;
	uint8_t payload[64];
	uint8_t checksum;
}bits_t;

typedef struct
{
	uint8_t typeAndLength;
	uint8_t payload[64];
	uint8_t checksum;
}packageBits_t;

/* Calculating and returning checksum */
uint8_t checksumCalculation(uint8_t payload[], uint8_t bytesRecieved){
	uint8_t calculatedChecksum = 0;

	//Minus one because type and Length is increments bytesrecieved
	for(uint8_t j = 0; j < bytesRecieved; j++){
		calculatedChecksum = calculatedChecksum ^ payload[j];
	}
	printf("\nCalculated sum = %d\n", calculatedChecksum);

	return calculatedChecksum;
}

packageBits_t packaging(uint8_t type, uint8_t data[], uint8_t payloadLength){
	packageBits_t package;

	//Combining type and lenght
	package.typeAndLength 	= type << 6;
	package.typeAndLength 	= package.typeAndLength + payloadLength;

	//Payload
	for (uint8_t i = 0; i < payloadLength; i++){
		package.payload[i] = data[i];
	}

	//Calculating checksum
	package.checksum = checksumCalculation(data, payloadLength);

	/*package.checksum = 0;
	for(uint8_t j = 0; j < payloadLength; j++){
		package.checksum = package.checksum ^ package.payload[j];
	}*/


	return package;
}

void sendPackage(packageBits_t outPackage){
	
	uint8_t payloadLength = (outPackage.typeAndLength & 0x3f);

	//Standard first byte
	printf("%d ",COM_STD_BYTE1);
	printf("%d ",outPackage.typeAndLength);

	//printf("typeAndLength = %d\n", outPackage.typeAndLength);


	for (int i = 0; i < payloadLength; i++)
	{
		printf("%d ", outPackage.payload[i]);
	}

	printf("%d ", outPackage.checksum);
}

void bufferReset(bits_t *rData){
	for(uint8_t i = 0; i < rData->length; i++){
		rData->payload[i] = '\0';
	}
}

uint8_t dataIn(uint8_t *bytesRecieved, uint8_t receivingData, uint8_t c, bits_t *rData){
	/* If recieving isn't in progress, then searching for start sequence */
	if(!receivingData && c == COM_STD_BYTE1){
		receivingData = 1;

		printf("%s", "dataIn if ");
		printf("c = %d\n", c);
	}
	/* Recieving checksum when exceeding the payload length */
	else if(*bytesRecieved == rData->length + 1){
		receivingData = 0;
		rData->payload[*bytesRecieved] = '\0';

		rData->checksum = c;

		printf("\nreciecing data end\n");
	}
	/* Buffering payload */
	else if(receivingData && *bytesRecieved > 0){
		rData->payload[*bytesRecieved - 1] = c;
		++*bytesRecieved;

		printf("dataIn second else if. bRecieved: %d", *bytesRecieved);
		printf("\nc = %d\n", c);
	}
	/* First byte is being recieved and handled */
	else if(receivingData && *bytesRecieved == 0){
		++*bytesRecieved;

		rData->type 	= (c & 0xC0) >> 6;
		rData->length 	= c - (c & 0xC0);

		printf("dataIn third else if \n");
		printf("Type: %d\n", rData->type);
		printf("Lenght: %d\n", rData->length);
		printf("c = %d\n", c);
	}

	return receivingData;
}

void sendAckNack(uint8_t i){
	uint8_t data[1] = {i};	
	
	sendPackage(packaging(COM_TYPE_ACK_NACK, data, 1));

	/* Choose a way to send it */
	//_uart_putc(i);
	printf("\n%d\n", i);
	
	return;
}

void sendResponse(uint8_t payload[], uint8_t payloadLength){
	sendPackage(packaging(COM_TYPE_REQ_RES, payload, payloadLength));
}

void sendData(uint8_t payload[], uint8_t payloadLength){
	sendPackage(packaging(COM_TYPE_DATA, payload, payloadLength));
}

void sendTelemetryData(void){
	uint8_t telemetry[64]; //Max package lenght
	uint8_t i = 0;

	//getTelemetry(telemetry, &i); //Get the telemetry and lenght

	//Placeholder until getTelemetry exists
	telemetry[i++] = 9; //item 1
	telemetry[i++] = 8; //item 2
	telemetry[i++] = 7; //item 3
	//And so on.. Is this the best solution?

	sendData(telemetry, i);
}

void sendAckWithHeader(void){
	uint8_t ackHeaderData[64];


	/* DETTE ERSTATTER NEDENSTÅENDE PLACEHOLDER SENERE
	uint8_t headerLenght;
	uint8_t header[64];

	getHeader(header, &headerLenght); //Get the header and lenght

	//Put the Ack in the first byte
	uint8_t i = 0;
	ackHeaderData[i++] = 1; //This is the Ack

	//Copy the header in after the Ack
	for(i; i <= headerLenght; i++){
		ackHeaderData[i] = headerData[(i - 1)];
	}
	*/

	//Placeholder until getHeader function exists.
	uint8_t i = 0;
	ackHeaderData[i++] = 1; //This is the Ack
	ackHeaderData[i++] = 5;
	ackHeaderData[i++] = 6;

	//Send header as ack/nack type
	sendPackage(packaging(COM_TYPE_ACK_NACK, ackHeaderData, i));
}

uint8_t handshakeReaction(uint8_t handshakeState, uint8_t checksumAccepted, bits_t rData){
	if(checksumAccepted){
		handshakeState = 1;
		sendAckWithHeader();
	}
	else{
		handshakeState = 0;
		sendAckNack(0);
	}

	printf("\nHandshake = %d\n", handshakeState);

	//Set system time

	return handshakeState;
}

uint8_t ackNackReaction(uint8_t checksumAccepted, bits_t rData){

	if(checksumAccepted){
		return rData.payload[0]; //Returns recieved ack/nack
	}
	else{
		sendAckNack(0); //or sends nack
		return 2; //INVALID VALUE. FUNCTIONS MUST BE EXPLICIT IN USING 1 OR 0
	}
}

uint8_t reqResReaction(uint8_t checksumAccepted, bits_t rData, uint8_t handelingReq, uint8_t *request){
	if(checksumAccepted){
		//Pick req apart and respond
		*request = rData.payload[0];

		//CHANGE THIS
		if(1/*Request is possible*/){
			handelingReq = 1;
			sendAckNack(1);
		}
		//Request to stop:
		else if(*request == COM_REQ_HALT){
			sendAckNack(1);
			handelingReq = 0;
		}
		//Invalid request
		else{

			/* Possibly sendResponse to differentiate from wrong checksum
			uint8_t notPossibleArr[] = {0};
			sendResponse(notPossibleArr, 1);*/

			sendAckNack(0);
		}
	}
	else{
		sendAckNack(0);
	}

	return handelingReq;
}

//NOT FINISHED
void getResponse(uint8_t request){

	switch(request) {
		case 1: /*Get package and setup buffer*/ break;
		default: break;
	} 

	return;
}

void dataReaction(uint8_t checksumAccepted, bits_t rData){
	//Do something with the data
	if(checksumAccepted){
		sendAckNack(1);
		//And do something...
	}
	else{
		sendAckNack(0);
	}	
}

int main(void) {

	bits_t rData;
	uint8_t handshakeState = 0;
	uint8_t receivingPayload = 0;
	uint8_t bytesRecieved = 0;
	uint8_t checksumAccepted = 0;
	uint8_t currAckNack = 0;
	uint8_t handelingReq = 0;
	uint8_t requestType = 0;
	uint8_t latestResponse[64];
	uint8_t latestResponseLenght = 0;
//	char arr[] = {"Hel\r\n"};
    while (1){
		//BSP_UARTx_transmit(arr, 5);
		//BSP_UARTx_Receive(arr, 3);


		//Date_Time_t printdate;
		//RTC_Get_Date_Time(&printdate);
		/*printf("%02d:%02d:%02d:%03d ", printdate.hours, printdate.minutes, printdate.seconds, (uint16_t)((float)3.93 * (UINT8_MAX - printdate.subseconds)));
		printf("%02d-%02d-%02d\n", printdate.date, printdate.month, 2000 + printdate.year); */
		//HAL_Delay(1);
    	//continue;
		uint8_t c = 0;
		//uint8_t t[11] = {0};
		c = UARTx_read_byte();
		//d = BSP_UARTx_Receive(t,11);
		//_uart_putc(c);
		//continue;

		printf("c = %02X\n", c);
		
		//if(d){continue;}
		//continue;
		//printf("%d %d %d\n", handshakeState, receivingPayload, bytesRecieved);

		//Handle the byte
		receivingPayload = dataIn(&bytesRecieved, receivingPayload, c, &rData);
		
		//Import buffer and react on data
		if(bytesRecieved && !receivingPayload){
			bytesRecieved -= 1; //To discount typeAndLenght as a payload-byte.
			checksumAccepted = (checksumCalculation(rData.payload, bytesRecieved) == rData.checksum);

			//Tempoary for test
			if(checksumAccepted){
				printf("Checksum correct!\n");
			}
			else{
				printf("Checksum NOT correct\n");
			}

			if(handshakeState){

				switch(rData.type) {
				case COM_TYPE_HANDSHAKE: handshakeState = handshakeReaction(handshakeState, checksumAccepted, rData); break;
				case COM_TYPE_ACK_NACK: currAckNack = ackNackReaction(checksumAccepted, rData); break;
				case COM_TYPE_REQ_RES: handelingReq = reqResReaction(checksumAccepted, rData, handelingReq, &requestType); currAckNack = 1; break; //currAckNack is set to one so that the first package will be sent
				case COM_TYPE_DATA: dataReaction(checksumAccepted, rData); break; 
				default: break;
				}

				//If currently set to respond and currAckNack is valid value
				if(handelingReq && currAckNack < 2){
					if(rData.type == COM_TYPE_ACK_NACK && currAckNack == 0){
						//resend last response
						//sendResponse(latestResponse, latestResponseLenght);
					}
					else if(rData.type == COM_TYPE_ACK_NACK && currAckNack == 1){
						//send next response
						//latestResponse = newestRespons
						//latestResonseLenght = newestResonseLenght
					}
					else if(currAckNack == 1){
						//Only taken after initial request. Send first part of response
						//latestResponse = newestRespons
						//latestResonseLenght = newestResonseLenght						
					}
				}
			}
			else if(rData.type == COM_TYPE_HANDSHAKE){
				handshakeState = handshakeReaction(handshakeState, checksumAccepted, rData);
			}



			//Resetting for next loop
			bufferReset(&rData);
			bytesRecieved = 0;
			currAckNack = 0;
		}

		if(handshakeState){
			sendTelemetryData();			
		}


		/*
		testExportData.typeAndLength 	= payloadType << 6;
		testExportData.typeAndLength 	= testExportData.typeAndLength + payloadLength;

		testExportData.payload[0]	= 168;
		testExportData.payload[1]	= 11;
		testExportData.payload[2]	= 71;
		testExportData.payload[3]	= 88;
		testExportData.payload[4]	= 0;
		testExportData.payload[5]	= 0;
		testExportData.payload[6]	= 0;
		testExportData.payload[7]	= 0;
		testExportData.payload[8]	= 0;

		testExportData.checksum 	= 0;
		*/

	}
}