/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2013 Freescale Semiconductor, Inc.
 * ALL RIGHTS RESERVED.
 *
 ***************************************************************************
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************//*!
 *
 * @file uart.c
 *
 * @author Freescale
 *
 * @version 0.0.1
 *
 * @date June-6-2013
 *
 * @brief providing common UART API.
 *
 ******************************************************************************/
#include "uart.h"

/******************************************************************************
 * Local variables
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes
 ******************************************************************************/

/******************************************************************************
 * Local functions
 *****************************************************************************/

/*****************************************************************************//*!
*
* @brief read receive buffer
*
* @param[in] pUART       base of UART port
*
* @return unsign char received char
*
*****************************************************************************/
uint8_t UART_ReadDataReg(LPUART_Type *pUART)
{
    /* Return the 8-bit data from the receiver */
    return pUART->DATA;
}
/*****************************************************************************//*!
*
* @brief write transmit buffer
*
* @param[in] pUART       base of UART port
* @param[in] u8Char      char to send
*
* @return none
*
*****************************************************************************/
void UART_WriteDataReg(LPUART_Type *pUART, uint8_t u8Char)
{
    /* Send the character */
    pUART->DATA = (uint8_t)u8Char;
}

/*****************************************************************************//*!
*
* @brief check whether tx is complete,i.e. data has been sent out.
*
* @param[in] pUART  base of UART port
*
* @return
*               1, Tx complete flag is set
*               0, Tx complete flag is clear
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
uint8_t UART_IsTxComplete(LPUART_Type *pUART)
{
    return ((pUART->STAT & LPUART_STAT_TC_MASK)>>LPUART_STAT_TC_SHIFT);
}
/*****************************************************************************//*!
*
* @brief check whether Tx buffer is empty
*
* @param[in] pUART  base of UART port
*
* @return
*               1, Tx buffer is empty
*               0, Tx buffer is not empty
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
 uint8_t UART_IsTxBuffEmpty(LPUART_Type *pUART)
{
    return ((pUART->STAT & LPUART_STAT_TDRE_MASK)>>LPUART_STAT_TDRE_SHIFT);
}
/*****************************************************************************//*!
*
* @brief check whether Rx buffer is full, i.e. receive a character
*
* @param[in] pUART  base of UART port
*
* @return
*               1, Rx buffer is full
*               0, Rx buffer is not full
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
 uint8_t UART_IsRxBuffFull(LPUART_Type *pUART)
{
    return ((pUART->STAT & LPUART_STAT_RDRF_MASK)>>LPUART_STAT_RDRF_SHIFT);
}
/*! @} End of uart_api_list */
/******************************************************************************
 * Global functions
 ******************************************************************************/

/******************************************************************************
 * define UART APIs
 *
 *//*! @addtogroup uart_api_list
 * @{
 *******************************************************************************/

/*****************************************************************************//*!
 *
 * @brief initialize the UART, interrupts disabled, and no hardware flow-control.
 *
 * @param[in] pUART       base of UART port
 * @param[in] pConfig     pointer to UART configuration structure
 *
 * @return none
 *
 * @ Pass/ Fail criteria: none
 *****************************************************************************/
void UART_Init(LPUART_Type *pUART, uint32_t u32SysClk, uint32_t u32Baud)
{
	uint16_t u16Sbr;

	/* Enable the clock to the selected UART */    
	if (pUART == LPUART0)
	{
		PCC->PCCn[PCC_LPUART0_INDEX] |= PCC_PCCn_PCS(0b011)    /* Clock Src= 1 (SOSCDIV2_CLK) */
    	                    		    |PCC_PCCn_CGC_MASK;     /* Enable clock for LPUART1 regs */
	}
	else if (pUART == LPUART1)
	{
		PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(0b011)    /* Clock Src= 1 (SOSCDIV2_CLK) */
    	                    		    |PCC_PCCn_CGC_MASK;     /* Enable clock for LPUART1 regs */
	}
	else if (pUART == LPUART2)
	{
		PCC->PCCn[PCC_LPUART2_INDEX] |= PCC_PCCn_PCS(0b011)    /* Clock Src= 1 (SOSCDIV2_CLK) */
    	                    		    |PCC_PCCn_CGC_MASK;     /* Enable clock for LPUART1 regs */
	}

	/* Make sure that the transmitter and receiver are disabled while we
	 * change settings.
	 */

	pUART->CTRL &= ~(LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);

	/* Configure the UART for 8-bit mode, no parity */
	pUART->CTRL &= ~(LPUART_CTRL_M7_MASK | LPUART_CTRL_M_MASK | LPUART_CTRL_R8T9_MASK | LPUART_CTRL_R9T8_MASK);


	/* Calculate baud settings */
	pUART->BAUD |= LPUART_BAUD_OSR(15);
	u16Sbr = (u32SysClk/u32Baud)/16;
	pUART->BAUD |= LPUART_BAUD_SBR(u16Sbr);

	/* Enable receiver and transmitter */
	pUART->CTRL |= LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK;
}

void UART_Reset(LPUART_Type *pUART)
{
	/* Set to after reset state an disable UART Tx/Rx */
	pUART->CTRL = 0x00000000;
	pUART->BAUD = 0x00000000;

	/* Disable clock to UART */
	PCC->PCCn[PCC_LPUART0_INDEX] &= ~PCC_PCCn_CGC_MASK;     /* Enable clock for LPUART1 regs */
	PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK;     /* Enable clock for LPUART1 regs */
	PCC->PCCn[PCC_LPUART2_INDEX] &= ~PCC_PCCn_CGC_MASK;     /* Enable clock for LPUART1 regs */

}

/*****************************************************************************//*!
 *
 * @brief receive a character.
 *
 * @param[in] pUART       base of UART port
 *
 * @return unsigned char
 *
 *****************************************************************************/
uint8_t UART_GetChar(LPUART_Type *pUART)
{

	/* Wait until character has been received */
	while((pUART->STAT & LPUART_STAT_RDRF_MASK)>>LPUART_STAT_RDRF_SHIFT==0);
	/* Return the 8-bit data from the receiver */
	return pUART->DATA;
}
/*****************************************************************************//*!
 *
 * @brief send a character.
 *
 * @param[in] pUART       base of UART port
 * @param[in] u8Char      char to send
 *
 * @return none
 *
 *****************************************************************************/
void UART_PutChar(LPUART_Type *pUART, uint8_t u8Char)
{    
	/* Wait until space is available in the FIFO */
	while((pUART->STAT & LPUART_STAT_TDRE_MASK)>>LPUART_STAT_TDRE_SHIFT==0);
	/* Send the character */
	pUART->DATA = (uint8_t)u8Char;
}


/*****************************************************************************//*!
 *
 * @brief send a series of characters using polling mode.
 *
 * @param[in] pUART      base of UART port
 * @param[in] pSendBuff  pointer of characters to send
 * @param[in] u32Length  number of characters
 *
 * @return       none
 *
 * @ Pass/ Fail criteria:
 *****************************************************************************/
void UART_SendWait(LPUART_Type *pUART, uint8_t *pSendBuff, uint32_t u32Length)
{
	uint8_t u8TxChar;
	uint32_t  i;

	for (i = 0; i < u32Length; i++)
	{
		u8TxChar = pSendBuff[i];
		while (!UART_IsTxBuffEmpty(pUART))
		{
#if defined(ENABLE_WDOG)
			WDOG_Feed();
#endif
		}
		UART_WriteDataReg(pUART, u8TxChar);
	}
}

/*****************************************************************************//*!
 *
 * @brief receive a series of charecters using polling mode.
 *
 * @param[in] pUART          base of UART port
 * @param[in] pReceiveBuff   pointer of charecters to receive
 * @param[in] u32Length      number of charecters
 *
 * @return       none
 *
 * @ Pass/ Fail criteria:
 *****************************************************************************/
void UART_ReceiveWait(LPUART_Type *pUART, uint8_t *pReceiveBuff, uint32_t u32Length)
{
	uint8_t u8RxChar;
	uint32_t i;

	for (i = 0; i < u32Length; i++)
	{
		while (!UART_IsRxBuffFull(pUART))
		{
#if defined(ENABLE_WDOG)
			WDOG_Feed();
#endif
		}
		u8RxChar = UART_ReadDataReg(pUART);
		pReceiveBuff[i] = u8RxChar;
	}
}

/*****************************************************************************//*!
 *
 * @brief wait tx complete.
 *
 * @param[in] pUART      base of UART port
 *
 * @return       none
 *
 * @ Pass/ Fail criteria: none*****************************************************************************/
void UART_WaitTxComplete(LPUART_Type *pUART)
{
	while (!UART_IsTxComplete(pUART));
}



/*! @} End of uart_api_list */
