#include "mcp41010.h"
#include "adc.h"
#include "cmd.h"


#include "timers.h"
#include "semphr.h"
#include "task.h"


/* software timer */
//#define INTR1_ID_TIMER		1
//#define INTR1_COUNT_TIMER 	20
//TimerHandle_t intr1handle;
//void intr1CallBackFunction(TimerHandle_t xTimer);
/*-----------------*/

/*semaphore for mcp41*/
static SemaphoreHandle_t xMCPresourceCmd = NULL;

extern eFuncCiruitRun eFuncRun;
extern adchandler_t adchandle;



/*************************	IMPLEMENT FUNCTION	****************************************/

//initialize some variable and Start to communicate
void vMCPStart (void){

	//init software timer as INTR1___EXTI external interrupt
//	intr1handle = xTimerCreate("intr1_example", INTR1_COUNT_TIMER, pdTRUE, (void*) INTR1_ID_TIMER,(TimerCallbackFunction_t)intr1CallBackFunction );
//	if(intr1handle == NULL){
//		while(1);
//	}

	//create semaphore resource
	xMCPresourceCmd = xSemaphoreCreateBinary();
	configASSERT(xMCPresourceCmd != NULL);
	xSemaphoreGive(xMCPresourceCmd);
	xMCPTakeCmd();

	//Event for mcp software
	vMCPEventInit();

	vADCInit();

	eFuncRun = INITFUNCTION;
	vSetAllParameter(0, POSITIVE, MIN_FREQUENCY);

	vMCPStopExternISR();

	HAL_GPIO_WritePin(GPIOB, SPI2_CS1_Pin /*|SPI2_CS2_Pin|SPI2_CS3_Pin*/ , GPIO_PIN_SET);

	vStartUpTimer();	//the first time for timer startup
}


uint8_t xMCPReleaseCmdFromISR(void){

	//from ISR
	xSemaphoreGiveFromISR(xMCPresourceCmd, NULL);
	return TRUE;

}

uint8_t xMCPReleaseCmd(void){
	xSemaphoreGive(xMCPresourceCmd);
	return TRUE;
}

uint8_t xMCPTakeCmd(void){
	if (xSemaphoreTake(xMCPresourceCmd, portMAX_DELAY) == pdTRUE)
		return TRUE;
	else
		return FALSE;
}

void vMCPStopExternISR(void){
	HAL_NVIC_DisableIRQ(INTR_MCP);
//	xTimerStop(intr1handle, portMAX_DELAY);
}

void vMCPStopExternISRFromISR(void){
	HAL_NVIC_DisableIRQ(INTR_MCP);
	//xTimerStopFromISR(intr1handle, NULL);
}

void vMCPStartExternISR(void){
	//enable EXTI interrupt
	HAL_NVIC_EnableIRQ(INTR_MCP);

//	xTimerStart(intr1handle, portMAX_DELAY);
}


void vMCP41010Poll(void){
	ewriteMCP eEvent;

	if(xMCPEventGet(&eEvent) == TRUE)
	{
		switch(eEvent)
		{
			case MCP_BALANCE:

			//SET all variable to balance state - Dvalue = 128
				vSetAllParameter(PERCENT_BALANCE, POSITIVE, MIN_FREQUENCY);
				vMCPSendToAllMCP();
				eFuncRun = NONEFUNCTION;

				vCMDCompleted();
				xMCPReleaseCmd();		//continue to receive data from command register
				break;

			case MCP_STEPFUNCTION:
				eFuncRun = STEPFUNCTION;
				vMCPStartExternISR();
				break;

			case MCP_DIRACFUNCTION100:
				eFuncRun = DIRACFUNCTION100;
				vMCPStartExternISR();
				break;

			case MCP_DIRACFUNCTION200:
				eFuncRun = DIRACFUNCTION200;
				vMCPStartExternISR();
				break;

			case MCP_FREQFUNCTION_AUTO:
				eFuncRun = FREQFUNCTIONAUTO;

				vStartTimerFreq();
				vMCPStartExternISR();
				break;

			case MCP_FREQFUNCTION_MANUAL:
				eFuncRun = 	FREQFUNCTIONMAN;

				vStartTimerFreq();
				vMCPStartExternISR();
				break;

			default:
				break;
		}
	}
}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == INTR1_Pin)
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
				vCMDCompleted();
				xMCPReleaseCmdFromISR();
//				xMCPReleaseCmd();		//when use software timer callback
			break;

			case DIRACFUNCTION100:
				vDirac100FunctionRun();
			break;

			case DIRACFUNCTION200:
				vDirac200FunctionRun();
			break;

			case FREQFUNCTIONAUTO:
				vFreqFunctionRun();
			break;

			case FREQFUNCTIONMAN:
				vFreqFunctionRun();
			break;

			default:
			break;
		}
	}
}


//void intr1CallBackFunction(TimerHandle_t xTimer){			//EXTI interrupt
//	uint32_t id;
//	id = (uint32_t)pvTimerGetTimerID(xTimer);
//
//	if(id == INTR1_ID_TIMER)
//	{
//
//		if(adchandle.status == ADC_ENABLE){		//EXTI occur
//			//start ADC conversion
//			vADCStartConversion();
//		}
//
//		switch (eFuncRun)
//		{
//			case NONEFUNCTION:
//				vMCPStopExternISR();
//			break;
//
//			case STEPFUNCTION:
//				vStepFunctionRun();
//				vCMDCompleted();
//				//xMCPReleaseCmdFromISR();
//				xMCPReleaseCmd();		//when use software timer callback
//			break;
//
//			case DIRACFUNCTION100:
//				vDirac100FunctionRun();
//			break;
//
//			case DIRACFUNCTION200:
//				vDirac200FunctionRun();
//			break;
//
//			case FREQFUNCTIONAUTO:
//				vFreqFunctionRun();
//			break;
//
//			case FREQFUNCTIONMAN:
//				vFreqFunctionRun();
//			break;
//
//			default:
//			break;
//		}
//
//	}
//
//}




