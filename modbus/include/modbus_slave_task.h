/*
 * modbus_slave_task.h
 *
 *  Created on: Sep 29, 2021
 *      Author: tu.lb174310
 */

#ifndef MODBUS_SLAVE_TASK_H_
#define MODBUS_SLAVE_TASK_H_


void initMBSlaveTask(void);

void slavepoll_task(void*p);

void led_task (void*);

#endif /* MODBUS_SLAVE_TASK_H_ */
