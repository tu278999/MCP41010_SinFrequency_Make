#include "main.h"


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
#define MAX_TIME_STEP_FREQ		50		//max time for each step frequency run: 100 <=> 10s , 150 <=> 15s
#define COUNT_DIRAC100			100		//100ms
#define COUNT_DIRAC200			200		//200ms
#define DOWNCOUNT_100MS			100		//100ms
#define DOWNCOUNT_15000MS		15000	//15000ms = 15s



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
	MCP_BALANCE 			= 1 << 1,
	MCP_STEPFUNCTION		= 1 << 2,
	MCP_DIRACFUNCTION100 	= 1 << 3,
	MCP_DIRACFUNCTION200 	= 1 << 4,
	MCP_FREQFUNCTION_AUTO 	= 1 << 5,
	MCP_FREQFUNCTION_MANUAL = 1 << 6
}ewriteMCP;

typedef enum functionCiruitRun{
	INITFUNCTION = 0,
	STEPFUNCTION,
	DIRACFUNCTION100,
	DIRACFUNCTION200,
	FREQFUNCTIONAUTO,
	FREQFUNCTIONMAN,
	NONEFUNCTION
}eFuncCiruitRun;


void vMCPStart (void);
void vMCP41010Poll(void);

void vMCPStopExternISR(void);
void vMCPStartExternISR(void);
void vMCPStopExternISRFromISR(void);

//semaphore
uint8_t xMCPReleaseCmd(void);
uint8_t xMCPTakeCmd(void);
uint8_t xMCPReleaseCmdFromISR(void);

//function circuit which run in  EXTI ISR
void vStepFunctionRun (void);
void vDirac100FunctionRun ( void);
void vDirac200FunctionRun (void);
void vFreqFunctionRun(void);

/* function prototypes serve Frequency function*/
void vSetSendBufMCP1(uint16_t NumCycle);
//void vSetSendBufMCP2(uint16_t NumCycle);
//void vSetSendBufMCP3(uint16_t NumCycle);

/* function prototypes handle data*/
void vSetAllParameter(float percent, eAmplifyDirect ampState, float sinFRQ);
float fSetSinFrequency(float sinFrq);
float fGetSinFrequency(void);

void vWriteOnlyMCP1(void);
//void vWriteOnlyMCP2(void);
//void vWriteOnlyMCP3(void);

void vMCPSendToAllMCP(void);

//Event which need to modify when we use RTOS
void vMCPEventPost(ewriteMCP event);
uint8_t vMCPEventPostFromISR(ewriteMCP event);
uint8_t xMCPEventGet(ewriteMCP *pevent);
void vMCPEventInit(void);

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

