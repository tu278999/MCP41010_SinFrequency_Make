#include "uart_mcp.h"
#include "mcp41010.h"
#include <stdlib.h>
#include <string.h>
#include "adc.h"

static void prvvMCPUartMenuSelect(void);
static void prvvMCPUartManualSweep(void);
static void prvvFloatToString(float fnum, char* buffer);

extern adchandler_t adchandle;

char *uartInit 			= 	"Initializing MCP41010.......\n";

char *uartInvalidNotify = 	"\n~~~~~Invalid entered number! please try again~~~~~\n"
							"NOTE: you must enter correctly the command number in the list of following menu!\n";

char *uartIntroChar 	= 	"\n"
							" -------------------------------\n"
							"|   List Command for MCP41010   |\n"
							" -------------------------------\n"
							"0: Balance       - Reset MCP410 to balance value\n"
							"1: STEP 5        - amplify 1.05 * Vin\n"
							"2: STEP -5       - amplify 0.95 * Vin\n"
							"3: STEP 10       - amplify 1.1 * Vin\n"
							"4: STEP -10      - amplify 0.9 * Vin\n"	//0.909 is actually min
							"5: DIRAC 100     - Dirac pulse 100 ms\n"
							"6: DIRAC 200     - Dirac pulse 200 ms\n"
							"7: AUTO-SWEEP    - Step frequency 0.5-3 Hz\n"
							"8: MANUAL-SWEEP  - Step frequency 0.5-3 Hz\n"
							"9: READ ADC VALUE \n"
							"NOTE: each number correspond to the command.\n"
							"Enter your number here (0 - 9): ";

char *uartManualSweep	= 	"\n"
							" -------------------------------\n"
							"|      Manual Sweep MCP41010     |\n"
							" -------------------------------\n"
							"1: INC 0.1 Hz of Creating Sin Frequency \n"
							"2: INC 0.2 Hz of Creating Sin Frequency\n"
							"3: INC 0.3 Hz of Creating Sin Frequency\n"
							"4: READ_ADC\n"
							"0: Exit.\n"
							"MIN_FREQUENCY = 0.5 Hz\n"
							"MAX_FREQUENCY = 3.0 Hz\n"
							"Enter your command number here (0 - 3): ";

char *uartMaxFrequency = "\n~~~~Exceed the MAX creating frequency - 3.0 Hz~~~\n";

char *uartManualContinue =  " Hz\nMANUAL SWEEP MODE is running.............\n"
							"Continue entering your command number here ( 0 - 4): ";



char uartCmdDisplay[MAX_COMMAND_MCP][100]	= { "processing 0 - Balance command....................\n",
												"processing 1 - STEP 5 command....................\n",
												"processing 2 - STEP -5 command....................\n",
												"processing 3 - STEP 10 command....................\n",
												"processing 4 - STEP -10 command....................\n",
												"processing 5 - DIRAC 100 command....................\n",
												"processing 6 - DIRAC 200 command....................\n",
												"processing 7 - SWEEP AUTO command....................\n"};

char *uartContinue	=	"\nThe process is done.\n"
						"Let's enter the other command number (0-9): ";

char *uartContinueMenu	=	"\nYou are in Menu option\n"
						"Let's enter the other command number (0-9): ";

char *uartContinueSweep	=	"\nYou are in Frequency Sweep Manual option\n"
						"Let's enter the other command number (0-4): ";


char *uartStepRunning	=	"\nThe Step function is running...............\n"
							"Let's enter the other command number (0-9): ";

char *uartMcpBusy	=	"\nThe MCP41010 is busy now\n"
						"Please wait for a moment..... ";

char *uartADC		= "\nADC conversion average:  ";

char *uartStopFreqAuto = "\nPress 0 to Stop Frequency Auto Mode......:";

MCP_UartHandle_t UartHandle;
extern UART_HandleTypeDef huart2;
extern eFuncCiruitRun eFuncRun;
extern uint16_t countSinCycle;

/*************************	IMPLEMENT FUNCTION	****************************************/

void vMCPUartStart(void){

	  UartHandle.process = PROCESS_INWORK;
	  UartHandle.countChar = RESET;
	  UartHandle.mode = MENUSELECT;
	  UartHandle.flag = FREE;
	  UartHandle.ControlFreq = NONE;
	  HAL_UART_Receive_IT(&huart2, (uint8_t*)&UartHandle.recChar, 1);
	  HAL_UART_Transmit_IT(&huart2, (uint8_t*)uartIntroChar, strlen(uartIntroChar));

}

void vMCPUartPoll(void){
	ewriteMCP eEvent;
	xMCPEventGet(&eEvent);

	if( UartHandle.flag == RECEIVEED)
	{
		//check weather MCPpoll is busy or not
//		if( 	(((eEvent != MCP_WAIT) || (eFuncRun != NONEFUNCTION))
//				&& (UartHandle.mode != MANUALSWEEP))
//				|| (UartHandle.ControlFreq != STARTFREQMODE) )
//		{
//			HAL_UART_Transmit_IT(&huart2, (uint8_t*)uartMcpBusy, strlen(uartMcpBusy));
//		}
//		else
		{
			if( UartHandle.countChar == 3 )
			{
				if(UartHandle.mode == MENUSELECT)
				{
					prvvMCPUartMenuSelect();
				}
				else if(UartHandle.mode == MANUALSWEEP)
				{
					prvvMCPUartManualSweep();
				}
			}
			else
			{
				UartHandle.flag = INVALIDNUM;
				HAL_UART_Transmit_IT(&huart2,(uint8_t*)uartInvalidNotify, strlen(uartInvalidNotify));

			}

		}

		UartHandle.countChar = RESET;
	}

}

static void prvvMCPUartManualSweep(void){
	float getSinf;


	if(UartHandle.cmdNumber == SWEEP_INC1 || UartHandle.cmdNumber == SWEEP_INC2 || UartHandle.cmdNumber == SWEEP_INC3 )
	{
		//Set new sin frequency
		getSinf = fGetSinFrequency();
		vSetSinFrequency(getSinf + UartHandle.cmdNumber * 0.1 );

		if(getSinf > MAX_FREQUENCY)
		{
			HAL_UART_Transmit_IT(&huart2,(uint8_t*)uartMaxFrequency, (uint16_t)strlen(uartMaxFrequency));
			UartHandle.flag = BUSY;
			UartHandle.mode = MENUSELECT;
			vMCPStopExternISR();
			vMCPEventPost(MCP_BALANCE);
		}
		else
		{
			char freqValue[4];
			char *intro = "\nThe Running Frequency is: ";
			//display the running frequency
			getSinf = fGetSinFrequency();
			prvvFloatToString(getSinf, freqValue);

			HAL_UART_Transmit(&huart2, (uint8_t*)intro, (uint16_t)strlen(intro), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t*)freqValue, 4, HAL_MAX_DELAY);
			HAL_UART_Transmit_IT(&huart2,(uint8_t*)uartManualContinue, (uint16_t)strlen(uartManualContinue));

			vADCEnable();			//enable again the conversion
			vStartTimerFreqManual();
			vMCPStartExternISR();

			UartHandle.flag = FREE;	//wait to continue receiving the character
			HAL_UART_Receive_IT(&huart2, (uint8_t*)&UartHandle.recChar, 1);

		}
		countSinCycle = RESET;

	}
	else if(UartHandle.cmdNumber == SWEEP_READ_ADC)
	{
		//display average value of conversion
		//display average value of conversion
		vADCSetAvegareValue();
		HAL_UART_Transmit(&huart2, (uint8_t*)uartADC, (uint16_t)strlen(uartADC), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*)adchandle.stringValue, (uint16_t)strlen(adchandle.stringValue), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*)uartContinueSweep, strlen(uartContinueSweep), HAL_MAX_DELAY);
		UartHandle.flag = FREE;	//wait to continue receive the character
		HAL_UART_Receive_IT(&huart2, (uint8_t*)&UartHandle.recChar, 1);

	}
	else if(UartHandle.cmdNumber == SWEEP_EXIT)
	{
		HAL_UART_Transmit_IT(&huart2, (uint8_t*)uartIntroChar, strlen(uartIntroChar));
		countSinCycle = RESET;
		vMCPStopExternISR();
		vMCPEventPost(MCP_BALANCE);
	}
	else{
		UartHandle.flag = INVALIDNUM;
		HAL_UART_Transmit_IT(&huart2,(uint8_t*)uartInvalidNotify, strlen(uartInvalidNotify));

	}

}


extern uint16_t CountTimer100ms;
extern uint16_t CountNumStepFreq;
static void prvvMCPUartMenuSelect(void){


	uint16_t len;

	switch(UartHandle.cmdNumber)
	{
	case CMD_BALANCE:
		if(UartHandle.ControlFreq == STARTFREQMODE)
		{
			vStopAllTimerFunc();
			vMCPStopExternISR();

			UartHandle.ControlFreq = STOPFREQMODE;
			CountTimer100ms = 0;
			CountNumStepFreq = 0;
			countSinCycle = 0;

		}
		UartHandle.flag = VALIDNUM;
		vMCPEventPost(MCP_BALANCE);

		break;

	case CMD_STEP5_POS:
		UartHandle.flag = VALIDNUM;
		vSetAllParameter(5, POSITIVE, 0);
		vMCPEventPost(MCP_STEPFUNCTION);
		break;

	case CMD_STEP5_NEG:
		UartHandle.flag = VALIDNUM;
		vSetAllParameter(5, NEGATIVE, 0);
		vMCPEventPost(MCP_STEPFUNCTION);
		 break;

	case CMD_STEP10_POS:
		UartHandle.flag = VALIDNUM;
		vSetAllParameter(10, POSITIVE, 0);
		vMCPEventPost(MCP_STEPFUNCTION);
		break;

	case CMD_STEP10_NEG:
		UartHandle.flag = VALIDNUM;
		vSetAllParameter(10, NEGATIVE, 0);
		vMCPEventPost(MCP_STEPFUNCTION);
		break;

	case CMD_DIRAC100:
		UartHandle.flag = VALIDNUM;
		vSetAllParameter(10, POSITIVE, 0);
		vMCPEventPost(MCP_DIRACFUNCTION100);
		break;

	case CMD_DIRAC200:
		UartHandle.flag = VALIDNUM;
		vSetAllParameter(10, POSITIVE, 0);
		vMCPEventPost(MCP_DIRACFUNCTION200);
		break;

	case CMD_SWEEP:
		UartHandle.ControlFreq = STARTFREQMODE;
		UartHandle.flag = VALIDNUM;
		vSetAllParameter(0, POSITIVE, MIN_FREQUENCY);
		vMCPEventPost(MCP_FREQFUNCTION_AUTO);
		break;

	case CMD_SWEEP_MANUAL:
		HAL_UART_Transmit_IT(&huart2,(uint8_t*)uartManualSweep, strlen(uartManualSweep));
		HAL_UART_Receive_IT(&huart2, (uint8_t*)&UartHandle.recChar, 1);
		UartHandle.mode = MANUALSWEEP;
		//enable ADC conversion
		vADCEnable();

		UartHandle.flag = FREE;	//wait to continue receive the character
		vSetAllParameter(0, POSITIVE, MIN_FREQUENCY);
		vMCPEventPost(MCP_FREQFUNCTION_MANUAL);
		break;

	case CMD_READ_ADC:

		//display average value of conversion
		vADCSetAvegareValue();
		len = (uint16_t)strlen(adchandle.stringValue);
		HAL_UART_Transmit(&huart2, (uint8_t*)uartADC, (uint16_t)strlen(uartADC), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*)adchandle.stringValue, len, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*)uartContinueMenu, strlen(uartContinueMenu), HAL_MAX_DELAY);
		UartHandle.flag = FREE;	//wait to continue receive the character
		HAL_UART_Receive_IT(&huart2, (uint8_t*)&UartHandle.recChar, 1);

		break;

	default:
		UartHandle.flag = INVALIDNUM;
		HAL_UART_Transmit_IT(&huart2, (uint8_t*)uartInvalidNotify, strlen(uartInvalidNotify));
		break;
	}

	if(UartHandle.flag == VALIDNUM){

		//enable ADC conversion
		vADCEnable();

		HAL_UART_Transmit_IT( &huart2, (uint8_t*)(uartCmdDisplay[UartHandle.cmdNumber]) ,
							  strlen(uartCmdDisplay[UartHandle.cmdNumber]) 			   );
	}

}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(UartHandle.countChar == RESET)
		UartHandle.cmdNumber = UartHandle.recChar - 48;

	UartHandle.countChar++;
	/*
	 * after receive '\r' character, UART receive immediately '\n' character
	 * that why countChar = 3
	 */
	if(UartHandle.recChar == '\n'){
		UartHandle.flag = RECEIVEED; //SET
	}

	/* if the program is busy, STOP receive character or command number */
	if(UartHandle.flag == FREE || UartHandle.ControlFreq == STARTFREQMODE)
		HAL_UART_Receive_IT(&huart2, (uint8_t*)&UartHandle.recChar, 1);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(	UartHandle.flag == INVALIDNUM)
	{
		if(UartHandle.mode == MENUSELECT)
		{
			HAL_UART_Transmit_IT(&huart2, (uint8_t*)uartIntroChar, strlen(uartIntroChar));
		}
		else
			HAL_UART_Transmit_IT(&huart2, (uint8_t*)uartManualSweep, strlen(uartManualSweep));

		UartHandle.flag = FREE;
		HAL_UART_Receive_IT(&huart2, (uint8_t*)&UartHandle.recChar, 1);
	}

	if(UartHandle.flag == VALIDNUM)
	{
		if(eFuncRun == STEPFUNCTION)
		{
			UartHandle.flag = FREE;
			eFuncRun = NONEFUNCTION;
			HAL_UART_Transmit_IT(&huart2, (uint8_t*)uartStepRunning, strlen(uartStepRunning));
			HAL_UART_Receive_IT(&huart2, (uint8_t*)&UartHandle.recChar, 1);
		}
		else
			UartHandle.flag = BUSY;
	}

	if( (UartHandle.process == PROCESS_DONE) && (UartHandle.flag == BUSY) )
	{
		UartHandle.process = PROCESS_INWORK;
		UartHandle.flag = FREE;
		HAL_UART_Receive_IT(&huart2, (uint8_t*)&UartHandle.recChar, 1);
	}
}


static void prvvFloatToString(float fnum, char* buffer){
    char tmp[2];
    int i = (int) (fnum * 100.0);

    itoa(i, tmp, 10);	//10 is decima index

    if(i < 100){
    	buffer[0] = '0';
        buffer[1] = '.';
        buffer[2] = tmp[0];
        buffer[3] = tmp[1];
    }
    else{
        buffer[0] = tmp[0];
        buffer[1] = '.';
        buffer[2] = tmp[1];
        buffer[3] = tmp[2];
    }
    return;
}
