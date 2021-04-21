/*
 * pin_config.c
 *
 *  Created on: Oct 31, 2016
 *      Author: B52932
 */

#include "pin_config.h"

#include "S32K148.h"

void UART_pin_settings(void){
	PCC->PCCn[PCC_PORTC_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTC */

	/* enable the UART port for the opensda of S32K148 */
	PORTC->PCR[6]|=PORT_PCR_MUX(2);           /* Port C6: MUX = ALT2,UART1 TX */
	PORTC->PCR[7]|=PORT_PCR_MUX(2);           /* Port C7: MUX = ALT2,UART1 RX */
}

void UART_pin_settings_reset(void){
	PCC->PCCn[PCC_PORTC_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTC */
	PORTC->PCR[6]|=PORT_PCR_MUX(1);           /* Port C6: MUX = ALT2,UART1 TX */
	PORTC->PCR[7]|=PORT_PCR_MUX(1);           /* Port C7: MUX = ALT2,UART1 RX */
}

