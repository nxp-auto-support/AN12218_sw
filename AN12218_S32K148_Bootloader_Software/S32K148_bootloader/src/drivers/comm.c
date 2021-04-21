/*
 * comm.c
 *
 *  Created on: Oct 31, 2016
 *      Author: Manuel Rodriguez
 */
#include "comm.h"

enum comms selected_comm = NONE;
uint32_t start_address = 0;

/* Helper functions */
static uint8_t comm_verify_phrase(BootPhraseStruct *BP);
static void comm_send_acknowledge(void);
static void comm_signal_CRC(void);
static void comm_clear_bf(void);

/* Comm related prototypes */
#if UART_COMM
static void UART_get_phrase(BootPhraseStruct *BP);
#endif

#if SPI_COMM
static void SPI_get_phrase(BootPhraseStruct *BP);
#endif

/**
 * Initializes the communication interfaces for the bootloader
 *
 */
void init_comm(void){

#if UART_COMM
	/* Initialize pin settings for UART communication */
	UART_pin_settings();

	/* Initialize UART communication */
	UART_Init(UART_PORT, SYSCLK, UART_BAUD);
#endif

#if SPI_COMM
	/* Initialize pin settings for SPI communication */
	SPI_pin_settings(SPI_PINS);

	/* Initialize SPI communication */
	SPI_init(SPI_PORT, SLAVE);
#endif
}

/**
 * Reset the communication interfaces for the bootloader
 *
 */
void disable_comm(void){

#if UART_COMM
	/* Reset pin settings for UART communication */
	UART_pin_settings_reset();
	/* Reset UART register */
	UART_Reset(UART_PORT);
#endif

#if SPI_COMM
	/* Reset pin settings for SPI communication */
	SPI_pin_settings_reset();

	/* Reset SPI registers */
	SPI_Reset(SPI_PORT);
#endif

	/* Disable flash */
	mem_man_disable();
}

uint8_t comm_status_rx(void){
	uint8_t status = 0;

	/* Check status of different communication interfaces the first to detect activity returns */
#if UART_COMM
	/* Check if data was received through UART */
	status = UART_IsRxBuffFull(UART_PORT);
	/* If data was received through UART return and select UART as comm */
	if(status){
		selected_comm = UART;
		return status;
	}
#endif

#if SPI_COMM
	/* Check if data was received through SPI */
	status = SPI_IsRxBuffFull(SPI_PORT);

	/* If data was received through SPI return and select SPI as comm */
	if(status){
		selected_comm = SPI;
		return status;
	}
#endif

	return 0;
}

void comm_download_app(void){
	BootPhraseStruct BP;
	uint32_t DataPhrase_counter = 0;
	uint8_t error, end_records = 0;

	/* While data is coming */
	do{
		/* Get phrase */
		comm_get_phrase(&BP);

		/* Verify phrase ( check if a valid SREC was received and calculate CRC and signal if data was received correctly) */
		error = comm_verify_phrase(&BP);

		/* Sent acknowledge or signal error */
		if(ERR_OK == error){
			/* Check type of SREC */
			if((BP.F.PhraseType == '1') | (BP.F.PhraseType == '2') | (BP.F.PhraseType == '3')){
				/* Write phrase */
				mem_man_write(&BP);

				/* Increment data phrase counter */
				++DataPhrase_counter;

			}else if ((BP.F.PhraseType == '5') | (BP.F.PhraseType == '6')){			/* Compare data phrase count */
				uint32_t count = 0;
				/* Get count out of phrase */
				if(BP.F.PhraseType == '5'){					/* 16-bit address */
					/* Get 16-bit count out of array */
					uint32_t *tmp;
					tmp = (uint32_t *) BP.F.PhraseAddress;
					count = (LE2BE_32(*tmp) >> 16) & 0xFFFF;
				} else if (BP.F.PhraseType == '6'){			/* 24-bit address */
					/* Get 24-bit count out of array */
					uint32_t *tmp;
					tmp = (uint32_t *) BP.F.PhraseAddress;
					count = (LE2BE_32(*tmp) >> 8) & 0xFFFFFF;
				}

				/* Compare data phrase count */
				if(count != DataPhrase_counter){
					//TODO Signal error to application
				}

			}else if ((BP.F.PhraseType == '7') | (BP.F.PhraseType == '8') | (BP.F.PhraseType == '9')){

				/* Save start address */
				if(BP.F.PhraseType == '9'){					/* 16-bit address */
					/* Get 16-bit address out of array */
					uint32_t *tmp;
					tmp = (uint32_t *) BP.F.PhraseAddress;
					start_address = (LE2BE_32(*tmp) >> 16) & 0xFFFF;
				} else if (BP.F.PhraseType == '8'){			/* 24-bit address */
					/* Get 24-bit address out of array */
					uint32_t *tmp;
					tmp = (uint32_t *) BP.F.PhraseAddress;
					start_address = (LE2BE_32(*tmp) >> 8) & 0xFFFFFF;
				} else{										/* 32-bit address */
					/* Get 32-bit address out of array */
					uint32_t *tmp;
					tmp = (uint32_t *) BP.F.PhraseAddress;
					start_address = LE2BE_32(*tmp) & 0xFFFFFFFF;
				}

				/* Signal end of records */
				end_records = 1;
			}

			/* Send acknowledge */
			comm_send_acknowledge();
		}else{
			/* Signal ERR_CRC */
			comm_signal_CRC();
		}

	} while(!end_records);

	/* Wait for Tx buffer to clear */
	comm_clear_bf();
}

void comm_get_phrase(BootPhraseStruct *BP){

	/* Get phrase through active channel */
	switch(selected_comm){
#if UART_COMM
		case UART:
			UART_get_phrase(BP);
			break;
#endif
#if SPI_COMM
		case SPI:
			SPI_get_phrase(BP);
			break;
#endif
		default:
			break;
	}
}

static uint8_t comm_verify_phrase(BootPhraseStruct *BP){
	uint32_t checksum = 0;
	uint8_t counter = 0;
	uint8_t CRC_phrase = 0;
	uint8_t i;

	/* Check for a valid SREC type */
	if((BP->F.PhraseType < '0') || (BP->F.PhraseType > '9')){
		return ERR_CRC;
	}

	/* Check for a valid size and return if invalid */
	if(BP->F.PhraseSize > MAX_PHSIZE_BP){
		return ERR_CRC;
	}

	/* Initialize counter */
	counter = BP->F.PhraseSize;

	/* Add byte count to checksum */
	checksum += counter;

	/* Add address */
	/* Check records with a 24-bit address field */
	if((BP->F.PhraseType == '2') | (BP->F.PhraseType == '6') | (BP->F.PhraseType == '8')){
		/* Add address */
		for( i = 0; i < 3; i++){
			checksum += BP->F.PhraseAddress[i];
		}
		counter = counter - 3;
	}else if((BP->F.PhraseType == '3') | (BP->F.PhraseType == '7') ){		/* Check records with a 32-bit address field */
		/* Add address */
		for( i = 0; i < 4; i++){
			checksum += BP->F.PhraseAddress[i];
		}
		counter = counter - 4;
	}else{																	/* All the other records have a 16-bit address field */
		/* Add address */
		for( i = 0; i < 2; i++){
			checksum += BP->F.PhraseAddress[i];
		}
		counter = counter - 2;
	}

	/* Add data */
	for( i = 0; ((i < (counter - 1)) && (counter <= MAX_DATA_BP + 1)); i++){
		checksum += BP->F.PhraseData[i];
	}

	/* Keep least significant byte */
	CRC_phrase = (uint8_t)(0xFF & checksum);

	/* Compute one's complement of LSB */
	CRC_phrase = ~CRC_phrase;

	/* Check if CRC matches */
	if(CRC_phrase == BP->F.PhraseCRC){
		return ERR_OK;
	}else{
		return ERR_CRC;
	}
}

static void comm_send_acknowledge(void){
#if SPI_COMM
	uint8_t signal_send = 0, data;
#endif

	/* Send acknowledge through active channel */
	switch(selected_comm){
#if UART_COMM
		case UART:
			UART_PutChar(UART_PORT, ERR_OK);
			break;
#endif
#if SPI_COMM
		case SPI:
			/* Wait for master request and send data afterwards */
			do{
				data = SPI_read(SPI_PORT);
				/* Master Request received? */
				if(data == MST_RQT){
					/* Send data on next frame */
					SPI_transmit(SPI_PORT, ERR_OK);
					data = SPI_read(SPI_PORT);
					signal_send = 1;
				}
			}while(!signal_send);
			/* Clear Tx buffer */
			SPI_transmit(SPI_PORT, 0x00);
			break;
#endif
		default:
			break;
	}
}

static void comm_signal_CRC(void){
#if SPI_COMM
	uint8_t signal_send = 0, data;
#endif
	/* Send CRC error through active channel */
	switch(selected_comm){
#if UART_COMM
		case UART:
			UART_PutChar(UART_PORT, ERR_CRC);
			break;
#endif
#if SPI_COMM
		case SPI:
			/* Wait for master request and send data afterwards */
			do{
				data = SPI_read(SPI_PORT);
				/* Master Request received? */
				if(data == MST_RQT){
					/* Send data on next frame */
					SPI_transmit(SPI_PORT, ERR_CRC);
					data = SPI_read(SPI_PORT);
					signal_send = 1;
				}
			}while(!signal_send);
			/* Clear Tx buffer */
			SPI_transmit(SPI_PORT, 0x00);
			break;
#endif
		default:
			break;
	}
}

static void comm_clear_bf(void){

	/* Clear buffer of active channel */
	switch(selected_comm){
#if UART_COMM
		case UART:
			UART_WaitTxComplete(UART_PORT);
			break;
#endif
#if SPI_COMM
		case SPI:
			SPI_WaitTxComplete(SPI_PORT);
			break;
#endif
		default:
			break;
	}
}

#if UART_COMM
static void UART_get_phrase(BootPhraseStruct *BP){
	char type = 0;
	uint8_t length = 0;
	uint8_t i;

	/* Get SREC line start ('S') */
	do{
	type = UART_GetChar(UART_PORT);
	if(type != 'S'){
		/* Signal a CRC error to start a new transmission */
		UART_PutChar(UART_PORT, ERR_CRC);
	}
	}while(type != 'S');

	/* Store phrase type */
	BP->F.PhraseType = UART_GetChar(UART_PORT);

	/* Get byte count */
	length = UART_GetChar(UART_PORT);
	BP->F.PhraseSize = length;

	/* Check records with a 24-bit address field */
	if((BP->F.PhraseType == '2') | (BP->F.PhraseType == '6') | (BP->F.PhraseType == '8')){
		/* Get address */
		for( i = 0; i < 3; i++){
			BP->F.PhraseAddress[i] = UART_GetChar(UART_PORT);
		}
		length = length - 3;
	}else if((BP->F.PhraseType == '3') | (BP->F.PhraseType == '7') ){		/* Check records with a 32-bit address field */
		/* Get address */
		for( i = 0; i < 4; i++){
			BP->F.PhraseAddress[i] = UART_GetChar(UART_PORT);
		}
		length = length - 4;
	}else{																	/* All the other records have a 16-bit address field */
		/* Get address */
		for( i = 0; i < 2; i++){
			BP->F.PhraseAddress[i] = UART_GetChar(UART_PORT);
		}
		length = length - 2;
	}

	/* Get data stream */
	for( i = 0; ((i < (length - 1)) && (length <= MAX_DATA_BP + 1)); i++){
		BP->F.PhraseData[i] = UART_GetChar(UART_PORT);
	}

	/* Get CRC */
	BP->F.PhraseCRC = UART_GetChar(UART_PORT);
}
#endif

#if SPI_COMM
static void SPI_get_phrase(BootPhraseStruct *BP){

	uint8_t data = 0, length = 0, sync = 0;

	/* Wait for master request and send data afterwards to synchronize */
	do{
		data = SPI_read(SPI_PORT);
		/* Master Request received? */
		if(data == START_SYNC){
			/* Send data on next frame */
			SPI_transmit(SPI_PORT, START_SYNC);
			data = SPI_read(SPI_PORT);

			/* Clear Tx buffer */
			SPI_transmit(SPI_PORT, 0x00);
			sync = 1;
		}
	}while(!sync);

	/* Get SREC line start ('S') */
	SPI_read(SPI_PORT);

	/* Store phrase type */
	BP->F.PhraseType = SPI_read(SPI_PORT);

	/* Get byte count */
	length = SPI_read(SPI_PORT);
	BP->F.PhraseSize = length;

	/* Check records with a 24-bit address field */
	if((BP->F.PhraseType == '2') | (BP->F.PhraseType == '6') | (BP->F.PhraseType == '8')){
		/* Get address */
		for(uint8_t i = 0; i < 3; i++){
			BP->F.PhraseAddress[i] = SPI_read(SPI_PORT);
		}
		length = length - 3;
	}else if((BP->F.PhraseType == '3') | (BP->F.PhraseType == '7') ){		/* Check records with a 32-bit address field */
		/* Get address */
		for(uint8_t i = 0; i < 4; i++){
			BP->F.PhraseAddress[i] = SPI_read(SPI_PORT);
		}
		length = length - 4;
	}else{																	/* All the other records have a 16-bit address field */
		/* Get address */
		for(uint8_t i = 0; i < 2; i++){
			BP->F.PhraseAddress[i] = SPI_read(SPI_PORT);
		}
		length = length - 2;
	}

	/* Get data stream */
	for(uint8_t i = 0; ((i < (length - 1)) && (length <= MAX_DATA_BP + 1)); i++){
		BP->F.PhraseData[i] = SPI_read(SPI_PORT);
	}

	/* Get CRC */
	BP->F.PhraseCRC = SPI_read(SPI_PORT);
}
#endif
