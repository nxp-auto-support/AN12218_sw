/*
 * pit.h
 *
 *  Created on: Nov 4, 2016
 *      Author: B52932
 */

#ifndef SOURCES_DRIVERS_INC_PIT_H_
#define SOURCES_DRIVERS_INC_PIT_H_

#include "common.h"

/* Define trigger period for PIT value
 * The period is calculated:
 * (period / clock period) - 1
 * Using 24MHz as bus clock and with a desired period of 2s
 *  48000000 ~ 2s
 *
 * */
#define TRIG_PERIOD		48000000

/* Prototypes */
void init_pit(void);
void disable_pit(void);
uint8_t is_pit_triggered(void);

#endif /* SOURCES_DRIVERS_INC_PIT_H_ */
