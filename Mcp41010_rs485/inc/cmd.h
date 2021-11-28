/*
 * cmd.h
 *
 *  Created on: Nov 1, 2021
 *      Author: tu.lb174310
 */

#ifndef INC_CMD_H_
#define INC_CMD_H_
#include <stdint.h>
#define CMD_STEP5			10
#define CMD_STEP_5			11
#define CMD_STEP10			12
#define CMD_STEP_10 		13
#define CMD_DIRAC100		14
#define CMD_DIRAC200		15
#define CMD_FREQUENAUTO		16
#define CMD_FREQUENMAN		17
#define CMD_READ_ADC		19
#define CMD_BALANCE			20



void vCMDPoll(void);

void vCMDCompleted(void);
void vCMDError(void);
void vCMDMasterReadADC(uint16_t value);




#endif /* INC_CMD_H_ */
