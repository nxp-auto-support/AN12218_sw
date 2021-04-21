/*
 * main.c              Copyright NXP 2017
 * Description:  Simple program to exercise GPIO
 * 2017 July 14 S Mihalik/ O Romero / Agustin Diaz - initial version
 *	S32K148 EVB
 */

#include "S32K148.h"    /* include peripheral declarations S32K148 */

#define PTE23  23         /* Port PTE23, bit 0: EVB output to blue LED */
#define PTC12 12        /* Port PTC12, bit 12: FRDM EVB input from BTN0 [SW2] */

void WDOG_disable (void){
  WDOG->CNT=0xD928C520;    /*Unlock watchdog*/
  WDOG->TOVAL=0x0000FFFF;  /*Maximum timeout value*/
  WDOG->CS = 0x00002100;   /*Disable watchdog*/
}

int main(void) {
  int counter = 0;
  WDOG_disable();             /* Disable Watchdog in case it is not done in startup code */
                              /* Enable clocks to peripherals (PORT modules) */
  PCC-> PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock to PORT C */
  PCC-> PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock to PORT E */
                               /* Configure port C12 as GPIO input (BTN 0 [SW2] on EVB) */
  PTC->PDDR &= ~(1<<PTC12);    /* Port C12: Data Direction= input (default) */

  PORTC->PCR[12] |= PORT_PCR_PFE_MASK; /* Port C12: MUX = GPIO, input filter enabled */
  PORTC->PCR[12] |= PORT_PCR_MUX(0b001); /* Port C12: MUX = GPIO, input filter enabled */
                               /* Configure port E23 as GPIO output (LED on EVB) */
  PTE->PDDR |= 1<<PTE23;        /* Port PTE23: Data Direction= output */
  PORTE->PCR[23] = 0x00000100;  /* Port PTE23: MUX = GPIO */

  for(;;) {
    if (PTC->PDIR & (1<<PTC12)) {   /* If Pad Data Input = 1 (BTN0 [SW2] pushed) */
        PTE-> PSOR |= 1<<PTE23;        /* Set Output on port E23 (LED on) */
    }
    else {                          /* If BTN0 was not pushed */
        PTE-> PCOR |= 1<<PTE23;        /* Clear Output on port E23 (LED off) */
    }
    counter++;
  }
}
