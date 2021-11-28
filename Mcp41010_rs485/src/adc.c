/*
 * adc.c
 *
 *  Created on: Oct 31, 2021
 *      Author: tu.lb174310
 */
#include "../../Mcp41010_rs485/inc/adc.h"

#include <math.h>
#include <stdlib.h>


#include "main.h"
#include "cmd.h"


extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc2;

adchandler_t adchandle;



void vADCInit(void){
	adchandle.status = ADC_DISABLE;
	adchandle.timerFag = DISABLE;		//consider to remove
	adchandle.countNSample = 0;
}

void vADCEnable (void){

	adchandle.status = ADC_ENABLE;
	adchandle.countNSample = 0;

}

void vADCDisable(void){
	vADCInit();
}

void vADCStartConversion(void){

	//start conversion
	adchandle.status = ADC_CONVERTING;
	HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&adchandle.sampeValue, 1);


	//trigger timer 1ms to sample data
}

//void vADCStartTimerSample(void){
//
//	//start TIM6 with 50us each interrupt
//
//	//for test, I use systick ISR as timer sample
//	adchandle.timerFag = ENABLE;
//}

void vADCStopConversion(void){

	//stop conversion
	HAL_ADC_Stop_DMA(&hadc2);
	adchandle.status = ADC_DISABLE;
	//stop timer
	adchandle.timerFag = DISABLE;

}

void vADCSetAvegareValue(void){
	int i = 0;
	uint32_t temp1 = 0;
	for(i = 0; i < N_SAMPLE; i++)
	{
		temp1 += pow(adchandle.samplelist[i], 2);
	}
	adchandle.average = (uint16_t)sqrt((double)temp1/(double)N_SAMPLE);
	//itoa(adchandle.average, adchandle.stringValue, 10 );
	vCMDMasterReadADC(adchandle.average);
}



void vADCSaveSampleValue(void){		//example in Systick_hander __  stm32f4xx_it.c
	//
	 if(adchandle.status == ADC_CONVERTING){
		  //save sample value to Array
		  adchandle.samplelist[adchandle.countNSample] = adchandle.sampeValue;
		  adchandle.countNSample++;

		  //
		  if(adchandle.countNSample == N_SAMPLE){

			  vADCStopConversion();
			  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		  }

	  }

}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{

	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

}

