/*
 * project_task.c
 *
 *  Created on: Nov 1, 2021
 *      Author: tu.lb174310
 */


#include "main.h"

#include "FreeRTOS.h"
#include "task.h"

#include "cmsis_os2.h"

#include "modbus_slave_task.h"

#include "cmd.h"
#include "mcp41010.h"
#include "project_task.h"

TaskHandle_t mcp41_handler = NULL;
TaskHandle_t cmd_handler = NULL;


static void cmd_handle(void *p);
static void mcp41_task(void *p);




void vTaskInitAll(void){

	BaseType_t status;
	status = xTaskCreate(mcp41_task, "mcp41 task", 600, NULL, osPriorityAboveNormal, &mcp41_handler );
	configASSERT(status == pdPASS);

	status = xTaskCreate(cmd_handle, "cmd task", 600, NULL, osPriorityNormal, &cmd_handler );
	configASSERT(status == pdPASS);

	//init modbus RS485
	initMBSlaveTask();

}

static void mcp41_task(void *p){

	vMCPStart();

	while(1)
	{

		vMCP41010Poll();

	}

}

static void cmd_handle(void *p){


	while(1)
	{
		if( xMCPTakeCmd() == TRUE )
		{
			//wait notify from modbus
			ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

			vCMDPoll();
		}

	}

}
