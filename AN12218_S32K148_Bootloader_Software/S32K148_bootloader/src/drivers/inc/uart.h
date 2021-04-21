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
* @file UART.h
*
* @author Freescale
*
* @version 0.0.1
*
* @date Jun 6, 2013
*
* @brief provide commond UART utilities. 
*
*******************************************************************************/
#ifndef _UART_H_
#define _UART_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
* Includes
******************************************************************************/
#include "S32K148.h"

/******************************************************************************
* Constants
******************************************************************************/

/******************************************************************************
* Macros
******************************************************************************/
#define MAX_UART_NO             3

/******************************************************************************
* Types
******************************************************************************/

/******************************************************************************
*define uart setting type
*
*//*! @addtogroup uart_setting_type
* @{
*******************************************************************************/  

/*!
* @brief UART setting type.
*
*/ 

typedef struct
{
    uint32_t bEnable      : 1;                /*!< 1: enable, 0: disable */
    uint32_t resvd        : 31;               /*!< 1: reserved bit field */
} UART_SettingType;
/*! @} End of uart_setting_type */



/******************************************************************************
*define uart config baudrate type
*
*//*! @addtogroup uart_config_baudrate_type
* @{
******************************************************************************/
 /*!
 * @brief UART baudrate type structure.
 *
 */   
typedef struct
{
    uint32_t    u32SysClkHz;        /*!< system clock */
    uint32_t    u32Baudrate;        /*!< UART baudrate */
} UART_ConfigBaudrateType;
/*! @} End of uart_config_baudrate_type */


/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Inline functions
******************************************************************************/




/******************************************************************************
* Global functions declaration
******************************************************************************/
void UART_Init(LPUART_Type *pUART, uint32_t u32SysClk, uint32_t u32Baud);
void UART_Reset(LPUART_Type *pUART);
uint8_t UART_GetChar(LPUART_Type *pUART);
void UART_PutChar(LPUART_Type *pUART, uint8_t u8Char);
void UART_SendWait(LPUART_Type *pUART, uint8_t *pSendBuff, uint32_t u32Length);
void UART_ReceiveWait(LPUART_Type *pUART, uint8_t *pReceiveBuff, uint32_t u32Length);
void UART_WaitTxComplete(LPUART_Type *pUART);



#ifdef __cplusplus
}
#endif
#endif /* #ifndef _UART_H_ */
