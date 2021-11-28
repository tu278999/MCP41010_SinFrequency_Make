/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "e_port.h"
#include "main.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbrtu.h"

 
/* ----------------------- extern functions ---------------------------------*/
extern pxMBFrameCB pxMBFrameCBByteReceived;
extern pxMBFrameCB pxMBFrameCBTransmitterEmpty;
 
/* -----------------------    variables     ---------------------------------*/

#if PORT_USE_UART2
void USART2_IRQHandler(void);
#else
void UART5_IRQHandler(void);
#endif
 
/* ----------------------- Start implementation -----------------------------*/

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
  /* If xRXEnable enable serial receive interrupts. If xTxENable enable
  * transmitter empty interrupts.
  */
  
  if (xRxEnable) {        
    __HAL_UART_ENABLE_IT(&huartused, UART_IT_RXNE);
    //HAL_GPIO_WritePin(GPIOC, PC2_Pin, GPIO_PIN_RESET);	//RE ........Receiver Output Enable (Low to enable)
  } else {    
    __HAL_UART_DISABLE_IT(&huartused, UART_IT_RXNE);
   // HAL_GPIO_WritePin(GPIOC, PC2_Pin, GPIO_PIN_RESET);
  }
  
  if (xTxEnable) {    
    __HAL_UART_ENABLE_IT(&huartused, UART_IT_TXE);
    //HAL_GPIO_WritePin(GPIOC, PC3_Pin, GPIO_PIN_SET);   //DE……….Driver Output Enable (high to enable)
  } else {
    __HAL_UART_DISABLE_IT(&huartused, UART_IT_TXE);
    //HAL_GPIO_WritePin(GPIOC, PC3_Pin, GPIO_PIN_RESET);
  }  
  
}
 
BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
  /* 
  Do nothing, Initialization is handled by MX_USART3_UART_Init() 
  Fixed port, baudrate, databit and parity  
  */

	/*
	 * Please note: when user uses EVEN or ODD parity the Data bit value chose
	 * in CubeMx initialize must be 9 bit
	 */
  return TRUE;
}
 
BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
  /* Put a byte in the UARTs transmit buffer. This function is called
  * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
  * called. */
	huartused.Instance->DR = (uint8_t)(ucByte & 0xFFU);
	return TRUE;
  //return (HAL_OK == HAL_UART_Transmit(&huartused, (uint8_t*)&ucByte, 1, 10));
}
 
BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
  /* Return the byte in the UARTs receive buffer. This function is called
  * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
  */
  *pucByte = (uint8_t)(huartused.Instance->DR & (uint8_t)0x00FF);
  return TRUE;
}
 

/**
  * @brief This function handles USART2 global interrupt.
  */

#if PORT_USE_UART2
void USART2_IRQHandler(void){
#else
void UART5_IRQHandler(void){
#endif

  /* USER CODE BEGIN USART2_IRQn 0 */

  uint32_t tmp_flag = __HAL_UART_GET_FLAG(&huartused, UART_FLAG_RXNE);
  uint32_t tmp_it_source = __HAL_UART_GET_IT_SOURCE(&huartused, UART_IT_RXNE);

  if((tmp_flag != RESET) && (tmp_it_source != RESET)) {

	  pxMBFrameCBByteReceived();	  //xMBRTUReceiveFSM() in mbrtu.c

	  __HAL_UART_CLEAR_PEFLAG(&huartused);
    return;
  }

  if((__HAL_UART_GET_FLAG(&huartused, UART_FLAG_TXE) != RESET) &&(__HAL_UART_GET_IT_SOURCE(&huartused, UART_IT_TXE) != RESET)) {

	  pxMBFrameCBTransmitterEmpty(); 	  //xMBRTUTransmitFSM() in mbrtu.c

    return ;
  }

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huartused);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

