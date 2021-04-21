/*
 * timeout.h
 *
 *  Created on: Oct 31, 2016
 *      Author: B52932
 */

#ifndef SOURCES_DRIVERS_INC_TIMEOUT_H_
#define SOURCES_DRIVERS_INC_TIMEOUT_H_

#include "common.h"
#include "pit.h"

/* Define timeout value, the base is 1s */
#define TIMEOUT_VAL		5

/* Prototype */
void init_timeout(void);
uint8_t timeout(void);
void disable_timeout(void);


#endif /* SOURCES_DRIVERS_INC_TIMEOUT_H_ */
