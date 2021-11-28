#include "cmsis_os2.h"
#include "modbus_slave_task.h"
#include "main.h"
#include "e_port.h"
#include "user_mb_app.h"
#include "mb.h"

extern USHORT   usSRegInStart;
extern USHORT   usSRegInBuf[S_REG_INPUT_NREGS];

extern USHORT   usSRegHoldStart;
extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS];

extern USHORT   usSCoilStart;
extern UCHAR    ucSCoilBuf[S_COIL_NCOILS/8];

extern USHORT   usSDiscInStart;
extern UCHAR    ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES/8];

TaskHandle_t slavepoll_handle;
TaskHandle_t led_handle;

void initMBSlaveTask(void){
	BaseType_t status;
	status = xTaskCreate(slavepoll_task, "mb slave poll", 600, NULL, osPriorityNormal, &slavepoll_handle );
	configASSERT(status == pdPASS);

	status = xTaskCreate(led_task, "blink led", 100, NULL, osPriorityNormal, &led_handle );
	configASSERT(status == pdPASS);
	// vTaskStartScheduler();
}

void slavepoll_task(void*p){

	  usSRegInBuf[4] = 0x11;
	  usSRegInBuf[1] = 0x22;
	  usSRegInBuf[2] = 0x33;
	  usSRegInBuf[3] = 0x44;
	  usSRegHoldBuf[2] = 0x1111;
	  usSRegHoldBuf[1] = 0x2222;
	  ucSDiscInBuf[0] = 0xf0;
	  ucSCoilBuf[0] = 0xff ;
	eMBInit(MB_RTU, 1, 9, 115200, MB_PAR_NONE);
	eMBEnable();
	while(1){
		eMBPoll();
	}
}

void led_task (void*p){

	while(1){
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		//HAL_GPIO_WritePin(GreenLed_GPIO_Port, GreenLed_Pin, GPIO_PIN_SET);
		vTaskDelay(500);
	}
}

