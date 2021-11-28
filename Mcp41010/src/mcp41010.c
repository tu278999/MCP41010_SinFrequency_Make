#include "mcp41010.h"
#include "uart_mcp.h"
#include "adc.h"

extern TIM_HandleTypeDef htim6;	//vd thay cho EXTI interrupt
uint8_t downcounter20ms;
#define EXTI_TIME_20MS 20

extern UART_HandleTypeDef huart2;

extern eFuncCiruitRun eFuncRun;
extern MCP_UartHandle_t UartHandle;

//extern UART string variable to display
extern char *uartContinue;
extern char *uartInit;
extern char *uartStopFreqAuto;

/*************************	IMPLEMENT FUNCTION	****************************************/

//initialize some variable and Start to communicate
void vMCPStart (void){

	vADCInit();

	UartHandle.flag = BUSY;
	HAL_UART_Transmit_IT(&huart2, (uint8_t*)uartInit, strlen(uartInit));

	vMCPEventPost(MCP_WAIT); 		//wait to init function
	eFuncRun = INITFUNCTION;
	vSetAllParameter(0, POSITIVE, MIN_FREQUENCY);

	vMCPStopExternISR();

	HAL_GPIO_WritePin(GPIOB, SPI2_CS1_Pin/*|SPI2_CS2_Pin|SPI2_CS3_Pin*/, GPIO_PIN_SET);

	vStartUpTimer();	//the first time for timer startup
}

void vMCPStopExternISR(void){
	//HAL_NVIC_DisableIRQ(INTR_MCP);
	HAL_TIM_Base_Stop_IT(&htim6);

}
void vMCPStartExternISR(void){
	//enable EXTI interrupt
//	HAL_NVIC_EnableIRQ(INTR_MCP);

	downcounter20ms = EXTI_TIME_20MS;
	HAL_TIM_Base_Start_IT(&htim6);

}


void vMCP41010Poll(void){
	ewriteMCP eEvent;

	if(xMCPEventGet(&eEvent) == TRUE)
	{
		switch(eEvent)
		{
			case MCP_INIT:
				vMCPSendToAllMCP();
				eFuncRun = NONEFUNCTION;
				vMCPEventPost(MCP_WAIT);
				vStartUpTimer();			//the second time for timer startup
				break;

			case MCP_BALANCE:

			//SET all variable to balance state - Dvalue = 128
				vSetAllParameter(PERCENT_BALANCE, POSITIVE, MIN_FREQUENCY);
				vMCPSendToAllMCP();
				vMCPEventPost(MCP_WAIT);
				eFuncRun = NONEFUNCTION;

				//start to receive the command number
				UartHandle.process = PROCESS_DONE;
				if(UartHandle.mode == MANUALSWEEP){
					UartHandle.mode = MENUSELECT;
					UartHandle.process = PROCESS_INWORK;
					UartHandle.flag = FREE;
					HAL_UART_Receive_IT(&huart2, (uint8_t*)&UartHandle.recChar, 1);
				}
				else{
					while(HAL_UART_Transmit_IT(&huart2, (uint8_t*)uartContinue, strlen(uartContinue)) != HAL_OK);
				}

				break;

			case MCP_STEPFUNCTION:
				eFuncRun = STEPFUNCTION;
				vMCPEventPost(MCP_WAIT);
				vMCPStartExternISR();
				break;

			case MCP_DIRACFUNCTION100:
				eFuncRun = DIRACFUNCTION100;
				vMCPEventPost(MCP_WAIT);
				vMCPStartExternISR();
				break;

			case MCP_DIRACFUNCTION200:
				eFuncRun = DIRACFUNCTION200;
				vMCPEventPost(MCP_WAIT);
				vMCPStartExternISR();
				break;

			case MCP_FREQFUNCTION_AUTO:
				eFuncRun = FREQFUNCTION;
				vMCPEventPost(MCP_WAIT);

				while(HAL_UART_Transmit(&huart2,(uint8_t*)uartStopFreqAuto, strlen(uartStopFreqAuto),HAL_MAX_DELAY) != HAL_OK);
				HAL_UART_Receive_IT(&huart2, (uint8_t*)&UartHandle.recChar, 1);
				vStartTimerFreq();
				vMCPStartExternISR();
				break;

			case MCP_FREQFUNCTION_MANUAL:
				eFuncRun = FREQFUNCTION;
				vMCPEventPost(MCP_WAIT);

				vStartTimerFreqManual();
				vMCPStartExternISR();

				break;

			case MCP_WAIT:
				//wait for an other command
				break;

			default:
				break;
		}
	}
}


//
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	if (GPIO_Pin == INTR1_Pin)
//	{




//		switch (eFuncRun)
//		{
//		case NONEFUNCTION:
//			vMCPStopExternISR();
//		break;
//
//		case STEPFUNCTION:
//			vStepFunctionRun();
//		break;
//
//		case DIRACFUNCTION100:
//			vDirac100FunctionRun();
//		break;
//
//		case DIRACFUNCTION200:
//			vDirac200FunctionRun();
//		break;
//
//		case FREQFUNCTION:
//			vFreqFunctionRun();
//		break;
//
//		default:
//		break;
//		}
//	}
//}





