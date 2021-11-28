/*
 * mcp_event_timer.c
 *
 *  Created on: Oct 16, 2021
 *      Author: tu.lb174310
 */

#include "main.h"
#include "event_groups.h"
#include "adc.h"
#include "cmd.h"
#include "mcp41010.h"

extern TIM_HandleTypeDef htim6;

extern uint16_t countSinCycle;
extern eFuncCiruitRun eFuncRun;
extern adchandler_t adchandle;

//event to poll--we can copy all the things that relate to event to other file

static uint16_t downcounter;

EventGroupHandle_t xMcpOsEvent;
/*****************EVENT FUNCTION**********************/

void vMCPEventInit(void){
	xMcpOsEvent = NULL;
	xMcpOsEvent = xEventGroupCreate();
	configASSERT(xMcpOsEvent != NULL);
}


//these event function to use for RTOS program
void vMCPEventPost(ewriteMCP event){
	xEventGroupSetBits(xMcpOsEvent, event);
//  return TRUE;
}

uint8_t vMCPEventPostFromISR(ewriteMCP event){
	BaseType_t xresult = pdFAIL;
	xresult = xEventGroupSetBitsFromISR(xMcpOsEvent, event, NULL);
	if(xresult == pdFAIL){
		while(1);
	}
	return TRUE;
}

uint8_t xMCPEventGet(ewriteMCP *pevent){
	ewriteMCP eMCPEvent;
//	*pevent = eMCPEvent;
	eMCPEvent = xEventGroupWaitBits(	xMcpOsEvent,
										(MCP_BALANCE 			|
										MCP_STEPFUNCTION 		|
										MCP_DIRACFUNCTION100 	|
										MCP_DIRACFUNCTION200	|
										MCP_FREQFUNCTION_AUTO	|
										MCP_FREQFUNCTION_MANUAL	),
										pdTRUE, 				//clear on exit
										pdFALSE,				//wait for any bits
										portMAX_DELAY			);
    switch (eMCPEvent)
    {
		case MCP_BALANCE:
			*pevent = MCP_BALANCE;
			break;
		case MCP_STEPFUNCTION:
			*pevent = MCP_STEPFUNCTION;
			break;
		case MCP_DIRACFUNCTION100:
			*pevent = MCP_DIRACFUNCTION100;
			break;
		case MCP_DIRACFUNCTION200:
			*pevent = MCP_DIRACFUNCTION200;
			break;
		case MCP_FREQFUNCTION_AUTO:
			*pevent = MCP_FREQFUNCTION_AUTO;
			break;
		case MCP_FREQFUNCTION_MANUAL:
			*pevent = MCP_FREQFUNCTION_MANUAL;
			break;
    }


	return TRUE;
}
/******************************************************/


/*******************TIMER FUNCION**********************/
void vStartUpTimer(void)
{
	downcounter = 100;
	HAL_TIM_Base_Start_IT(&htim6);
}

void vStopAllTimerFunc(void){
	HAL_TIM_Base_Stop_IT(&htim6);
}

/*
 * Start timer for:
 * ->frequency function
 * ->Dirac100 function
 * ->Dirac200 function
 */
void vStartTimerFreq(void){
	downcounter = DOWNCOUNT_100MS;
	HAL_TIM_Base_Start_IT(&htim6);
}

//void vStartTimerFreqManual(void){
//	downcounterManual = DOWNCOUNT_15000MS;	//15s
//	HAL_TIM_Base_Start_IT(&htim6);
//}

void vStartTimerDirac100(void){
	downcounter = COUNT_DIRAC100;
	HAL_TIM_Base_Start_IT(&htim6);
}

void vStartTimerDirac200(void){
	downcounter = COUNT_DIRAC200;
	HAL_TIM_Base_Start_IT(&htim6);
}

/*********************************************************/
/*
 * Timer for function run call back
 * ---------------------ISR interrupt after each 1ms----------------------------------
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	//each callback = 1ms
{
	static uint8_t CountTimer100ms = 0;
	float getSinf;

	if (htim->Instance == TIM13){
	   HAL_IncTick();
	   vADCSaveSampleValue();
	}

	if(htim->Instance == TIM6 )
	{

			if(!downcounter--)
			{
				switch(eFuncRun)
				{
				case INITFUNCTION:
					eFuncRun = NONEFUNCTION;
					vStopAllTimerFunc();
					vMCPEventPostFromISR(MCP_BALANCE);
					break;

				case NONEFUNCTION:
					vStopAllTimerFunc();
					vMCPStopExternISR();
					vMCPEventPostFromISR(MCP_BALANCE);
					break;

				case DIRACFUNCTION100:
					vStopAllTimerFunc();					//Stop timer for Dirac function
					vMCPEventPostFromISR(MCP_BALANCE);

					break;

				case DIRACFUNCTION200:
					vStopAllTimerFunc();				//Stop timer for Dirac function
					vMCPEventPostFromISR(MCP_BALANCE);

					break;

				case FREQFUNCTIONAUTO:
					CountTimer100ms++;
					downcounter = DOWNCOUNT_100MS;
					if(CountTimer100ms == MAX_TIME_STEP_FREQ)
					{
						CountTimer100ms = 0;
						countSinCycle = 0;							//reset count Sin Cycle
						getSinf = fSetSinFrequency(STEP_FREQUENCY + fGetSinFrequency() );
						if(getSinf > MAX_FREQUENCY)
						{
							vStopAllTimerFunc();
							vMCPStopExternISRFromISR();

							vMCPEventPostFromISR(MCP_BALANCE);
						}

					}
					break;

				case FREQFUNCTIONMAN:
					CountTimer100ms++;
					downcounter = DOWNCOUNT_100MS;
					if(CountTimer100ms == MAX_TIME_STEP_FREQ)//freq manual function run out of time
					{
						vMCPStopExternISRFromISR();
						vStopAllTimerFunc();

						vSetAllParameter(PERCENT_BALANCE, POSITIVE, fGetSinFrequency());
						vMCPSendToAllMCP();

						CountTimer100ms = 0;
						vCMDCompleted();
						xMCPReleaseCmdFromISR();
					}
					break;

				default:

					break;
				}
			}

	}

}

