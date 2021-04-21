/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "timeout.h"
#include "comm.h"
#include "clock.h"

/* Bootloader definitions */
/* Start address for the application received by the bootloader
 * application vector table should start at this address
 * */
#define APP_START_ADDRESS	0x1000

/* Global variables */
uint8_t boot_from_comm = 0;				/* Used to signal activity on the comm channel */

/* Prototype */
void JumpToUserApplication( unsigned int userSP,  unsigned int userStartup);

/* Main Application*/
int main(void)
{

	/* Initialize clock */
	clock_initi();

	/* Initialize communication interfaces */
	init_comm();

	/* Initialize timeout */
	init_timeout();

	/* Check if boot start has been received or timeout occurred */
	do{
		uint8_t word_received = comm_status_rx();
		if(word_received){
			boot_from_comm = 1;
			comm_download_app();
		}
	} while((!timeout()) & (!boot_from_comm));

	/* Disable all systems and leave device as if coming out of reset */
	disable_timeout();
	disable_comm();
	reset_clock();

	/* Check if a valid application is loaded and jump to it */
	JumpToUserApplication(*((uint32_t*)APP_START_ADDRESS), *((uint32_t*)(APP_START_ADDRESS + 4)));

    /* Should never return from application code */
    for (;;) {};
    /* Never leave main */
    return 0;
}

/**
 * Used to jump to the entry point of the user application
 * The Vector table of the user application must be located at 0x1000
 *
 * */
void JumpToUserApplication( unsigned int userSP,  unsigned int userStartup)
{
	/* Check if Entry address is erased and return if erased */
	if(userSP == 0xFFFFFFFF){
		return;
	}

	/* Set up stack pointer */
	__asm("msr msp, r0");
	__asm("msr psp, r0");

	/* Relocate vector table */
	S32_SCB->VTOR = (uint32_t)APP_START_ADDRESS;

	/* Jump to application PC (r1) */
	__asm("mov pc, r1");
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
