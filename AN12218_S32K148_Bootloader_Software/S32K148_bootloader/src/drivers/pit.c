/*
 * pit.c
 *
 *  Created on: Nov 4, 2016
 *      Author: B52932
 */

#include "pit.h"
#include "S32K148.h"

void init_pit(void){

	/* Enable bus clock to PIT */
	PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(3) | PCC_PCCn_CGC_MASK;    /* Clock Src = 3 (FIRC_DIV2_CLK)*/

	/* Turn on PIT */
	LPIT0->MCR |= LPIT_MCR_M_CEN_MASK; /* enable module clock */

	/* Configure Channel 0 to trigger as defined in pit.h */
	LPIT0->TMR[0].TVAL = TRIG_PERIOD;

	/* Enable timer */
	LPIT0->TMR[0].TCTRL = LPIT_TMR_TCTRL_T_EN_MASK;
}

void disable_pit(void){

	/* Disable timer */
	LPIT0->TMR[0].TCTRL = 0x00000000;

	/* Restore register */
	LPIT0->TMR[0].TVAL = 0x00000000;

	/* Turn off PIT */
	LPIT0->MCR &= ~LPIT_MCR_M_CEN_MASK; /* enable module clock */

	/* Disable bus clock to PIT */
	PCC->PCCn[PCC_LPIT_INDEX] &= ~PCC_PCCn_CGC_MASK;

}

uint8_t is_pit_triggered(void){
	uint8_t tmp;

	/* Read status */
	tmp = LPIT0->MSR & LPIT_MSR_TIF0_MASK;

	/* Clear flag */
	if(tmp){
		LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
	}

	return tmp;
}

