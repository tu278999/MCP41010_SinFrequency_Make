/*
 * mcpuart.h
 *
 *  Created on: Sep 24, 2021
 *      Author: tu.lb174310
 */

#ifndef INC_MCPUART_H_
#define INC_MCPUART_H_

#include "main.h"
#include <string.h>

#define SET		1
#define RESET 	0
#define PROCESS_DONE	1
#define PROCESS_INWORK	0

#define MAX_COMMAND_MCP	8

#define CMD_BALANCE				0
#define CMD_STEP5_POS			1
#define CMD_STEP5_NEG			2
#define CMD_STEP10_POS			3
#define CMD_STEP10_NEG			4
#define CMD_DIRAC100			5
#define CMD_DIRAC200			6
#define CMD_SWEEP				7
#define CMD_SWEEP_MANUAL		8
#define CMD_READ_ADC			9

/* Sweep manual command */
#define SWEEP_EXIT				0
#define SWEEP_INC1				1
#define SWEEP_INC2				2
#define SWEEP_INC3				3
#define SWEEP_READ_ADC			4


typedef enum eLaptopUartFlag{
	FREE,
	RECEIVEED,
	INVALIDNUM,
	VALIDNUM,
	BUSY
}eFlag;

typedef enum eFunctionMode{
	MENUSELECT,
	AUTOSWEEP,
	MANUALSWEEP
}eFuncMode;

typedef enum eControlFrequency{
	NONE,
	STOPFREQMODE,
	STARTFREQMODE
}eCtrFreq;

typedef struct LaptopUart_Handle{
	uint8_t countChar;
	eFuncMode mode;
	eFlag	flag;
	uint8_t	process;		//for processing function 5,6,7 which need to set timer7
	char 	recChar;
	uint8_t cmdNumber;
	eCtrFreq ControlFreq;

}MCP_UartHandle_t;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);

void vMCPUartStart(void);
void vMCPUartPoll(void);


#endif /* INC_MCPUART_H_ */
