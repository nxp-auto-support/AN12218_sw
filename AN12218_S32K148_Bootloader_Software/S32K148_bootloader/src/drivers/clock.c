/*
 * comm.c
 *
 *  Created on: Nov 29, 2017
 *      Author: Agustin Diaz
 */

#include "clock.h"

void clock_initi(void){
	/* system running from reset clock (FIRC) */

	SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV2(0b010); 	/* Enable FIRC divider. FIRC/2 = 24 MHz  */
}

void reset_clock(void){
	SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV2(0b000); 	/* Disable FIRC divider  */
}
