#include "mcp41010.h"

eFuncCiruitRun eFuncRun;
uint16_t countSinCycle = 0;

/*************************	IMPLEMENT FUNCTION	****************************************/

/*
* These function run in EXTI ISR
*/
void vStepFunctionRun (void){

	//send frame to MCP over SPI
	vMCPSendToAllMCP();

	vMCPStopExternISR();
}

void vDirac100FunctionRun ( void){

	//send frame to MCP over SPI
	vMCPSendToAllMCP();

	//start timer 100ms
	vStartTimerDirac100();

	vMCPStopExternISR();
}

void vDirac200FunctionRun (void){
	//send frame to MCP over SPI
	vMCPSendToAllMCP();

	//start timer 200ms
	vStartTimerDirac200();

	vMCPStopExternISR();
}

void vFreqFunctionRun(void){

	vSetSendBufMCP1(countSinCycle);
	vWriteOnlyMCP1();

	vSetSendBufMCP2(countSinCycle);
	vWriteOnlyMCP2();

	vSetSendBufMCP3(countSinCycle);
	vWriteOnlyMCP3();

	countSinCycle++;
}








