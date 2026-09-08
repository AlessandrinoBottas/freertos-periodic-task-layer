#ifndef TASK_CONTROL_BLOCK_H
#define TASK_CONTROL_BLOCK_H

#include "PTL.h"
typedef struct
{
    const TaskConfig_t * xUserTask;
    TaskHandle_t pxTaskHandle;	/* Changes on every xTaskCreate() */
    TickType_t xReleaseTick;
    TickType_t xOverrunTick;
	TickType_t xDeadlineTick;
    uint8_t ucIsOverrunning;
    uint8_t ucIsRunning;        /* N.B. job running != status of the FreeRTOS Task!!!!! */
    uint32_t uiJobMiss;
    
    /* As xUserTask is declared as "const" and "xPhase" has to be reset in KILL policy, we add a xCurrentPhase attribute */
    TickType_t xCurrentPhase;           
    
    StaticTask_t * pxTCB;
    StackType_t  * pxStackBuffer;

} TaskControlBlock_t;

#endif
