#include "mcp41010.h"

extern SPI_HandleTypeDef hspi2;

/************************ static variable *******************/
static volatile float sinFreqCreate;		//frequency of sin wave which want to be created
static uint8_t Dvalue;						// 0 < Dvalue < 256
static uint16_t sendbuf;

int	countSinTest = 0;

/********* MCP handle data static function prototype ********/
static uint8_t prvucSetDValue (float percent,  eAmplifyDirect ampState);
static uint8_t prviRoundingNumber (float floatnum);
static void prvvSetSendBuf(uint8_t dvalue);

/*************************	IMPLEMENT FUNCTION	***********************/

/**
  * @brief  set all static variable in this file: mcphandledata.c
  * @param  percent: percentage increase amplify Vin : 0% < percent <= 10 %
  * @param  ampState: Amplify direction of Vin. Two valid value are: NEGATIVE = 0 and POSITIVE
  * @param  sinFRQ: only used in Frequency function mode
  * @retval None
  */
void vSetAllParameter(float percent, eAmplifyDirect ampState, float sinFRQ){

	prvucSetDValue(percent, ampState);
	prvvSetSendBuf(Dvalue);
	vSetSinFrequency(sinFRQ);
}

void vSetSinFrequency(float sinFrq){
	sinFreqCreate = sinFrq;
}

float fGetSinFrequency(void){
	return sinFreqCreate;
}

void vSetSendBufMCP1(uint16_t NumCycle){

	/*
	float temp = SIN_AMPLITUDE * sin( 0.02 * (float)NumCycle * 2.0* PI * sinFreqCreate );
	TESTSINWAVE = (110.0 * temp + 10.0)*256.0 / (10.0*temp + 20.0) ;//Dvalue
	countSinTest++;
	if(countSinTest == 100){
		__disable_irq();
		while(1);
	}
	prvvSetSendBuf( prviRoundingNumber( TESTSINWAVE ));
	*/
	float temp = SIN_AMPLITUDE * sin( 0.02 * (float)NumCycle * 2.0* PI * sinFreqCreate );
	Dvalue = prviRoundingNumber( (110.0 * temp + 10.0)*256.0 / (10.0*temp + 20.0) );
	prvvSetSendBuf(Dvalue);

}

 void vSetSendBufMCP2(uint16_t NumCycle){
//	float temp = SIN_AMPLITUDE * sin( 0.02 * NumCycle * 2.0* PI * sinFreqCreate + 2.0*PI/3.0);
//	prvvSetSendBuf( prviRoundingNumber( (110.0 * temp + 10.0)*256.0 / (10.0*temp + 20.0) ) );
}

void vSetSendBufMCP3(uint16_t NumCycle){
//	float temp = SIN_AMPLITUDE * sin( 0.02 * NumCycle * 2.0* PI * sinFreqCreate + 4.0*PI/3.0);
//	prvvSetSendBuf( prviRoundingNumber( (110.0 * temp + 10.0)*256.0 / (10.0*temp + 20.0) ) );
}

void vMCPSendToAllMCP(void){
		
		HAL_GPIO_WritePin(GPIOB, SPI2_CS1_Pin/*|SPI2_CS2_Pin|SPI2_CS3_Pin*/, GPIO_PIN_RESET);
	
		//polling SPI
		HAL_SPI_Transmit(&hspi2, (uint8_t*)&sendbuf, 1, HAL_MAX_DELAY);
		
		HAL_GPIO_WritePin(GPIOB, SPI2_CS1_Pin/*|SPI2_CS2_Pin|SPI2_CS3_Pin*/, GPIO_PIN_SET);
}
  
void vWriteOnlyMCP1(void){

	HAL_GPIO_WritePin(GPIOB, SPI2_CS1_Pin, GPIO_PIN_RESET);

	//polling SPI
	HAL_SPI_Transmit(&hspi2, (uint8_t*)&sendbuf, 1, HAL_MAX_DELAY);

	HAL_GPIO_WritePin(GPIOB, SPI2_CS1_Pin, GPIO_PIN_SET);

}
void vWriteOnlyMCP2(void){

//	HAL_GPIO_WritePin(GPIOB, SPI2_CS2_Pin, GPIO_PIN_RESET);
//
//	//polling SPI
//	HAL_SPI_Transmit(&hspi2, (uint8_t*)&sendbuf, 1, HAL_MAX_DELAY);
//
//	HAL_GPIO_WritePin(GPIOB, SPI2_CS2_Pin, GPIO_PIN_SET);

}

void vWriteOnlyMCP3(void){

//	HAL_GPIO_WritePin(GPIOB, SPI2_CS3_Pin, GPIO_PIN_RESET);
//
//	//polling SPI
//	HAL_SPI_Transmit(&hspi2, (uint8_t*)&sendbuf, 1, HAL_MAX_DELAY);
//
//	HAL_GPIO_WritePin(GPIOB, SPI2_CS3_Pin, GPIO_PIN_SET);
	
}

static uint8_t prvucSetDValue (float percent, eAmplifyDirect ampState){
    float temp;
    if(ampState == NEGATIVE)
        temp = 1 - (percent / 100);
    else if(ampState == POSITIVE)
        temp = 1 + (percent / 100);

    Dvalue = (uint16_t) prviRoundingNumber( 256 * (110 * temp - 100) / (10 *temp + 10) );

    if((percent > 9.09) && (ampState == NEGATIVE) )
        Dvalue = 0;
		//do something here if want to return state
		return 0;
}

static void prvvSetSendBuf(uint8_t dvalue){


	sendbuf = (uint16_t)( ((uint16_t)dvalue & 0x00FF) | MCP_COMMAND_WRITE );
}

static uint8_t prviRoundingNumber(float floatnum){
    uint16_t temp1, tempResult;
    tempResult = (uint16_t) floatnum;
    temp1 = (uint16_t) (floatnum *10);
    if(temp1 % 10 >= 5)
    {
        tempResult += 1;
    }
	if(tempResult >= 256)
	{
		tempResult = 255;
	}
    return (uint8_t)tempResult;
}




