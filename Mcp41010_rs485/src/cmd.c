/*
 * cmd.c
 *
 *  Created on: Nov 1, 2021
 *      Author: tu.lb174310
 */

#include "cmd.h"


#include "main.h"
#include "e_port.h"

#include "user_mb_app.h"
#include "adc.h"
#include "mcp41010.h"

//#include "FreeRTOS.h"
//#include "task.h"

extern USHORT   usSRegInStart;
extern USHORT   usSRegInBuf[S_REG_INPUT_NREGS];

extern USHORT   usSRegHoldStart;
extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS];

extern USHORT   usSCoilStart;
extern UCHAR    ucSCoilBuf[S_COIL_NCOILS/8];

extern USHORT   usSDiscInStart;
extern UCHAR    ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES/8];

extern uint16_t countSinCycle;

TaskHandle_t mcp41_handler;
TaskHandle_t cmd_handler;

static uint16_t xCMDReadCmd(void);
static void vCMDClearCmd(void);



/*********************IMPLEMENTATION CODE************************/

void vCMDPoll(void){

	uint8_t cmdcur = 0;
	float getSinf;

	//read register to check cmd and post event to MCP task
	cmdcur = (uint8_t)xCMDReadCmd();
	vADCEnable();			//enable again the conversion
	getSinf = fGetSinFrequency();
	switch(cmdcur){

		case CMD_BALANCE:
			vMCPEventPost(MCP_BALANCE);
		break;

		case CMD_STEP5:
			vSetAllParameter(5, POSITIVE, getSinf);
			vMCPEventPost(MCP_STEPFUNCTION);
			break;

		case CMD_STEP_5:
			vSetAllParameter(5, NEGATIVE, getSinf);
			vMCPEventPost(MCP_STEPFUNCTION);
			break;

		case CMD_STEP10:
			vSetAllParameter(10, POSITIVE, getSinf);
			vMCPEventPost(MCP_STEPFUNCTION);
			break;

		case CMD_STEP_10:
			vSetAllParameter(10, NEGATIVE, getSinf);
			vMCPEventPost(MCP_STEPFUNCTION);
			break;

		case CMD_DIRAC100:
			vSetAllParameter(10, POSITIVE, getSinf);
			vMCPEventPost(MCP_DIRACFUNCTION100);
			break;

		case CMD_DIRAC200:
			vSetAllParameter(10, POSITIVE, getSinf);
			vMCPEventPost(MCP_DIRACFUNCTION200);
			break;

		case CMD_FREQUENAUTO:
			countSinCycle = 0;
			vSetAllParameter(0, POSITIVE, MIN_FREQUENCY);
			vMCPEventPost(MCP_FREQFUNCTION_AUTO);
			break;

		case CMD_FREQUENMAN:

			getSinf = fSetSinFrequency( fGetSinFrequency() + STEP_FREQUENCY );

			if(getSinf > MAX_FREQUENCY)
			{
				vADCDisable();
				vMCPStopExternISR();
				vMCPEventPost(MCP_BALANCE);
			}
			else
			{
				vSetAllParameter(0, POSITIVE, getSinf);
				vMCPEventPost(MCP_FREQFUNCTION_MANUAL);

			}
			countSinCycle = RESET;

			break;

		case CMD_READ_ADC:

			vADCSetAvegareValue();
			vCMDCompleted();
			xMCPReleaseCmd();
			break;

		case 0:
			vADCDisable();
			xMCPReleaseCmd();
			break;
		default:
			vADCDisable();
			//set error bit
			vCMDError();
			xMCPReleaseCmd();

			break;
	}

}

static uint16_t xCMDReadCmd(void){
	/*
	 * when receive the new command, we clear completed BIT and error BIT
	 */
	ucSDiscInBuf[0] &= ~0x03;
	return usSRegHoldBuf[0];
}

static void vCMDClearCmd(void){
	 usSRegHoldBuf[0] = 0;
}

void vCMDCompleted(void){
	ucSDiscInBuf[0] &= ~0x01;	//clear bit error
	ucSDiscInBuf[0] |= 0x02; 	//set bit mcp worked completely
	vCMDClearCmd();
}

void vCMDError(void){
	ucSDiscInBuf[0] &= ~0x02;
	ucSDiscInBuf[0] |= 0x01;
	vCMDClearCmd();
}

void vCMDMasterReadADC(uint16_t value){
	usSRegInBuf[0] = value;
}









