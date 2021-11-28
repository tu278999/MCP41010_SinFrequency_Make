/*
 * adc.h
 *
 *  Created on: Oct 31, 2021
 *      Author: tu.lb174310
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_
#include "main.h"

#define N_SAMPLE	20



typedef enum ADCStatusHandle{
	ADC_ENABLE,
	ADC_CONVERTING,
	ADC_DISABLE
}adcstatus_t;

typedef struct ADCHandler{
	adcstatus_t status;
	uint16_t samplelist[N_SAMPLE];
	uint32_t sampeValue;
	uint16_t average;
	uint16_t countNSample;
	char stringValue[5];

	/* when use TIM6 for sample Data
	 * we use timerFlag to count 20 times
	 * 50us each interrupt => 1ms to save sample value
	 */
	uint8_t timerFag;

}adchandler_t;

void vADCInit(void);
void vADCEnable (void);
void vADCDisable(void);
void vADCStartConversion(void);
void vADCStartTimerSample(void);
void vADCStopConversion(void);
void vADCSaveSampleValue(void);
void vADCSetAvegareValue(void);
void vADCGetStringValue(char** output);


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

#endif /* INC_ADC_H_ */
