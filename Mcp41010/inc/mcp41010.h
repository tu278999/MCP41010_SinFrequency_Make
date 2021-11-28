#include "main.h"
#include <math.h>

// define for MCP41010
#define MCP_COMMAND_WRITE		0x1100

#define INTR_MCP				EXTI9_5_IRQn

#define PERCENT_BALANCE		   	0		//no percent increase
#define CYCLE_50HZ          	0.02f 	//second
#define PI                  	3.1415926535f
#define SIN_AMPLITUDE       	0.1f
#define DVALUE_ZEROPOINT		128		//Init value of MCP /or/ normal value of MCP
#define DVALUE_10PER			256


#define MIN_FREQUENCY			0.5f
#define MAX_FREQUENCY			3.0f
#define STEP_FREQUENCY			0.2f
#define MAX_NUM_STEP_FREQ		13


#define COUNT_DIRAC100			200		//Old value 100ms
#define COUNT_DIRAC200			400		//Old value 200ms

#define DOWNCOUNT_100MS			100		//100ms


#define DOWNCOUNT_FREQ_MANUAL	30000	//DOWNCOUNT_FREQ_MANUAL = 10000 = 10000ms = 10s
										//DOWNCOUNT_FREQ_MANUAL = 15000 = 15000ms = 15s
										//DOWNCOUNT_FREQ_MANUAL = 30000 = 30000ms = 30s

/*
 * DOWNCOUNT_FREQ_AUTO * DOWNCOUNT_100MS = time to run each frequency step at autoIncreseMode
 * vd: 150 * 100ms  = 15000ms = 15s for running each 'frequency step'
 */
#define DOWNCOUNT_FREQ_AUTO		300		//max time for each step frequency run: 100 <=> 10s
										//max time for each step frequency run: 150 <=> 15s
										//max time for each step frequency run: 300 <=> 30s




/* config value for MCP circuit */
#define RESISTER_ADD        	100  	//kilo (Ohm)

#ifndef TRUE
#define TRUE            		1
#endif

#ifndef FALSE
#define FALSE          			0
#endif

typedef enum{
    NEGATIVE = 0,
    POSITIVE
}eAmplifyDirect;

typedef enum writeMCPEvent{
	MCP_INIT = 0,
	MCP_BALANCE,
	MCP_STEPFUNCTION,
	MCP_DIRACFUNCTION100,
	MCP_DIRACFUNCTION200,
	MCP_FREQFUNCTION_AUTO,
	MCP_FREQFUNCTION_MANUAL,
	MCP_WAIT
}ewriteMCP;

typedef enum functionCiruitRun{
	INITFUNCTION = 0,
	STEPFUNCTION,
	DIRACFUNCTION100,
	DIRACFUNCTION200,
	FREQFUNCTION,
	NONEFUNCTION
}eFuncCiruitRun;


void vMCPStart (void);
void vMCP41010Poll(void);

void vMCPStopExternISR(void);
void vMCPStartExternISR(void);

//function circuit which run in  EXTI ISR
void vStepFunctionRun (void);
void vDirac100FunctionRun ( void);
void vDirac200FunctionRun (void);
void vFreqFunctionRun(void);

/* function prototypes serve Frequency function*/
void vSetSendBufMCP1(uint16_t NumCycle);
void vSetSendBufMCP2(uint16_t NumCycle);
void vSetSendBufMCP3(uint16_t NumCycle);

/* function prototypes handle data*/
void vSetAllParameter(float percent, eAmplifyDirect ampState, float sinFRQ);
void vSetSinFrequency(float sinFrq);
float fGetSinFrequency(void);

void vWriteOnlyMCP1(void);
void vWriteOnlyMCP2(void);
void vWriteOnlyMCP3(void);

void vMCPSendToAllMCP(void);

//Event which need to modify when we use RTOS
void vMCPEventPost(ewriteMCP event);
uint8_t xMCPEventGet(ewriteMCP *pevent);


//Timer prototype function
void vStartUpTimer(void);
void vStopAllTimerFunc(void);
void vStartTimerFreq(void);
void vStartTimerDirac100(void);
void vStartTimerDirac200(void);
void vStartTimerFreq(void);
void vStartTimerFreqManual(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);


//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

