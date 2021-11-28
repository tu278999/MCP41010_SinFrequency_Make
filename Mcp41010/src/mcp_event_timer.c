/*
 * mcp_event_timer.c
 *
 *  Created on: Oct 16, 2021
 *      Author: tu.lb174310
 */

#include "mcp41010.h"
#include "uart_mcp.h"
#include "main.h"
#include "adc.h"

extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim6;	//vd thay cho EXTI interrupt
extern uint8_t downcounter20ms;

extern uint16_t countSinCycle;
extern eFuncCiruitRun eFuncRun;
extern MCP_UartHandle_t UartHandle;
extern adchandler_t adchandle;

//event to poll--we can copy all the things that relate to event to other file
static ewriteMCP eMCPEvent;
static uint16_t downcounter = 0;
static uint16_t downcounterManual = 0;



/*****************EVENT FUNCTION**********************/

//these event function to use for RTOS program
void vMCPEventPost(ewriteMCP event){
	eMCPEvent = event;
}

uint8_t xMCPEventGet(ewriteMCP *pevent){
	*pevent = eMCPEvent;
	return TRUE;
}
/******************************************************/


/*******************TIMER FUNCION**********************/
void vStartUpTimer(void){
	downcounter = 100;
	HAL_TIM_Base_Start_IT(&htim7);
}

void vStopAllTimerFunc(void){
	HAL_TIM_Base_Stop_IT(&htim7);
}

/*
 * Start timer for:
 * ->frequency function
 * ->Dirac100 function
 * ->Dirac200 function
 */
void vStartTimerFreq(void){
	downcounter = DOWNCOUNT_100MS;
	HAL_TIM_Base_Start_IT(&htim7);
}

void vStartTimerFreqManual(void){
	downcounterManual = DOWNCOUNT_FREQ_MANUAL;	//10s or 15s or else
	HAL_TIM_Base_Start_IT(&htim7);
}

void vStartTimerDirac100(void){
	downcounter = COUNT_DIRAC100;
	HAL_TIM_Base_Start_IT(&htim7);
}

void vStartTimerDirac200(void){
	downcounter = COUNT_DIRAC200;
	HAL_TIM_Base_Start_IT(&htim7);
}

/*********************************************************/
/*
 * Timer for function run call back
 * ---------------------ISR interrupt after each 1ms----------------------------------
 */
uint8_t CountTimer100ms = 0;
uint8_t CountNumStepFreq = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	//each callback = 1ms
{


	if(htim->Instance == TIM7 )
	{
		if(UartHandle.mode == MENUSELECT)
		{
			if(!downcounter--)
			{
				//downcounter = 1000;
				//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
				//return ;
				switch(eFuncRun)
				{
				case INITFUNCTION:
					vMCPEventPost(MCP_INIT);
					vStopAllTimerFunc();
					break;

				case NONEFUNCTION:
					vMCPEventPost(MCP_WAIT);
					vStopAllTimerFunc();					// start up MCP program is completed

					//Startup UART
					vMCPUartStart();
					break;

				case DIRACFUNCTION100:
					vStopAllTimerFunc();					//Stop timer for Dirac function
					vMCPEventPost(MCP_BALANCE);
					break;

				case DIRACFUNCTION200:

						CountTimer100ms = 0;
						vStopAllTimerFunc();				//Stop timer for Dirac function
						vMCPEventPost(MCP_BALANCE);

					break;

				case FREQFUNCTION:
					CountTimer100ms++;
					downcounter = DOWNCOUNT_100MS;
					if(CountTimer100ms == DOWNCOUNT_FREQ_AUTO)
					{
						CountNumStepFreq++;
						countSinCycle = 0;							//reset count Sin Cycle

						vSetSinFrequency(CountNumStepFreq * STEP_FREQUENCY + MIN_FREQUENCY);

						if(CountNumStepFreq == MAX_NUM_STEP_FREQ)
						{
							vStopAllTimerFunc();

							vMCPEventPost(MCP_BALANCE);
							CountNumStepFreq = 0;
							UartHandle.ControlFreq = STOPFREQMODE;
							vMCPStopExternISR();
						}
						CountTimer100ms = 0;
					}
					break;

				default:

					break;
				}
			}
		}
		else
		{
			if(!downcounterManual--){
				vMCPStopExternISR();
				vStopAllTimerFunc();
				countSinCycle = 0;
				vSetAllParameter(PERCENT_BALANCE, POSITIVE, fGetSinFrequency());
				vMCPSendToAllMCP();
			}
		}
	}

	else if(htim->Instance == TIM6 )		//example for EXTI
	{

		if(!downcounter20ms--)
		{
			if(adchandle.status == ADC_ENABLE){		//EXTI occur
				//start ADC conversion
				vADCStartConversion();
			}

			switch (eFuncRun)
			{
					case NONEFUNCTION:
						vMCPStopExternISR();
					break;

					case STEPFUNCTION:
						vStepFunctionRun();
					break;

					case DIRACFUNCTION100:
						vDirac100FunctionRun();
					break;

					case DIRACFUNCTION200:
						vDirac200FunctionRun();
					break;

					case FREQFUNCTION:
						vFreqFunctionRun();
					break;

					default:
					break;
			}

			downcounter20ms = 20;
		}

	}

}



