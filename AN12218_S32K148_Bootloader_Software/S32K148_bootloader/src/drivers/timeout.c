/*
 * timeout.c
 *
 *  Created on: Oct 31, 2016
 *      Author: B52932
 */

#include "timeout.h"


void init_timeout(void){

	/* Initialize PIT */
	init_pit();
}

uint8_t timeout(void){
	uint8_t tmp_stat;
	static uint32_t timeout = 0;

	/* Check if PIT has triggered */
	tmp_stat = is_pit_triggered();

	/* Increment timeout variable */
	if(tmp_stat){
		++timeout;
	}

	/* Check if timeout occurred */
	if(timeout < TIMEOUT_VAL){
		return 0;
	}

	/* Restore timeout and signal it */
	timeout = 0;
	return 1;
}

void disable_timeout(void){

	/* Reset PIT */
	disable_pit();
}
