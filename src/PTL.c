/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "PTL.h"
#include "task_control_block.h"
#include "uart.h"
#include "logger.h"
#include "queue.h"

/*** PRIORITIES ***/
#define OVERRUN_TASK_MANAGER_PRIORITY ( configMAX_PRIORITIES - 1 )
#define TRACE_PRINTER_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 )

#define MAX_SYSTEM_TASKS 16
#define QUEUE_LENGTH     MAX_SYSTEM_TASKS
#define ITEM_SIZE        sizeof(uint8_t)

/*** GLOBAL VARIABLES ***/
TaskControlBlock_t pxSystemTasks[ MAX_SYSTEM_TASKS ];
size_t uxCurrentNumTasks = 0;

static StaticTask_t xManagerTCB;
static StackType_t xManagerStack[ 256 ];
                   
static StackType_t xTraceStack[ 512 ];
static StaticTask_t xTraceTCB;

/*** FUNCTIONS FIRMS ***/
BaseType_t xInitScheduler(TaskConfig_t *pxTasks, size_t uxNumTasks);

static void prvPeriodicTaskWrapper( void* pvParameters );
static void prvOverrunPolicyEnforcer( void * pvParameters );                /* FreeRTOS Task */

/* Queue for enforcment e deferrable */
static StaticQueue_t xStaticQueue;
static uint8_t ucQueueStorageArea[ QUEUE_LENGTH * ITEM_SIZE ];
QueueHandle_t xTaskQueue;


/*********************************************************************************************/
/*                                    THE CODE                                               */
/*********************************************************************************************/


BaseType_t xInitScheduler(TaskConfig_t *pxTasks, size_t uxNumTasks)
{
    if (uxNumTasks > MAX_SYSTEM_TASKS) {
        return pdFALSE;
    }
    
    uxCurrentNumTasks = uxNumTasks;
    
    xTaskQueue = xQueueCreateStatic( QUEUE_LENGTH, ITEM_SIZE, ucQueueStorageArea, &xStaticQueue );
    
    xTaskCreateStatic(
        prvOverrunPolicyEnforcer,    
        "OverrunManager",         
        256,                      
        NULL,                     
        OVERRUN_TASK_MANAGER_PRIORITY,     
        xManagerStack,            
        &xManagerTCB              
    );

    xTaskCreateStatic( vTracePrinterTask, "TraceTask", 512, NULL, TRACE_PRINTER_TASK_PRIORITY, xTraceStack, &xTraceTCB );

    /* Populating pxSystemTasks with PERIODIC TASKS*/
    for( size_t uxI = 0; uxI < uxNumTasks; uxI++ )
    {
        TaskControlBlock_t * pxIntTask = &pxSystemTasks[ uxI ];
        TaskConfig_t * pxCfg = &pxTasks[ uxI ];
        
        pxIntTask->xUserTask = pxCfg;
        pxIntTask->pxStackBuffer = NULL;
        pxIntTask->pxTCB = NULL;

        BaseType_t xRet = xTaskCreate( 
            prvPeriodicTaskWrapper, 
            pxIntTask->xUserTask->pcName, 
            pxIntTask->xUserTask->uxStackDepth, 
            ( void * ) pxIntTask, 
            pxIntTask->xUserTask->uxPriority, 
            &pxIntTask->pxTaskHandle
        );  
        
        if (xRet != pdPASS) {
            return pdFALSE;
        }
            
        pxIntTask->ucIsOverrunning = 0; 
        pxIntTask->ucIsRunning = 0;     
        pxIntTask->uiJobMiss = 0;       
        pxIntTask->xReleaseTick = 0;    
        pxIntTask->xOverrunTick = 0;
        pxIntTask->xCurrentPhase = pxIntTask->xUserTask->xPhase;
    }
    
    return pdTRUE;
}


/* FreeRTOS Task: prvPeriodicTaskWrapper
 * One prvPeriodicTaskWrapper for each task
 */
void prvPeriodicTaskWrapper(void* pvParameters)
{
    TaskControlBlock_t *pxTCB = (TaskControlBlock_t *) pvParameters;

    TaskFunction_t pUserTaskFunctionBody = pxTCB->xUserTask->pvTaskCode;
    void* pvUserParameters = pxTCB->xUserTask->pvParameters;

    /* Waiting for initial phase */
    if( pxTCB->xCurrentPhase > 0 )
    {
        vTaskDelayUntil( &pxTCB->xReleaseTick, pdMS_TO_TICKS(pxTCB->xCurrentPhase) );
        pxTCB->xCurrentPhase = 0;
    }

    for(;;)
    {
        pxTCB->xOverrunTick  = pxTCB->xReleaseTick + pxTCB->xUserTask->xPeriod; 
		pxTCB->xDeadlineTick = pxTCB->xReleaseTick + pxTCB->xUserTask->xDeadline;
        pxTCB->ucIsRunning = 1;
	    pxTCB->ucIsOverrunning = 0;

	    /* Recording Starting time */
	    vRecordTraceEvent( xTaskGetTickCount(), eTraceTaskStart, pxTCB->xUserTask->pcName, pxTCB->uiJobMiss, 0);

        /* -- Start User body task execution --*/
        pUserTaskFunctionBody(pvUserParameters);

        pxTCB->ucIsRunning = 0;

	    /* Recording Ending time */
        TickType_t xEndTick = xTaskGetTickCount();
	    vRecordTraceEvent( xEndTick, eTraceTaskEnd, pxTCB->xUserTask->pcName, pxTCB->uiJobMiss, ( xEndTick > pxTCB->xDeadlineTick ));

        vTaskDelayUntil(&pxTCB->xReleaseTick, pdMS_TO_TICKS(pxTCB->xUserTask->xPeriod));
    }
}

void vOverrunMonitorTickHook ( TickType_t ulCurrentTick )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	ulCurrentTick++;

	/* Periodic Overrun Checks */
    TaskControlBlock_t * pxTCB;

    for( uint8_t ucIndex=0; ucIndex < uxCurrentNumTasks; ucIndex++ )
    {
        pxTCB = &pxSystemTasks[ ucIndex ];
        if( pxTCB->ucIsRunning == 0 ) 		continue;
	    if( pxTCB->ucIsOverrunning == 1 )	continue;
        if( pxTCB->pxTaskHandle == NULL ) 	continue;	//safety check
        
        if( ulCurrentTick >= pxTCB->xOverrunTick )
        {
            xQueueSendFromISR( xTaskQueue, &ucIndex, &xHigherPriorityTaskWoken );
        }
    }

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


void prvOverrunPolicyEnforcer( void * pvParameters )
{
    (void) pvParameters;
    uint8_t ucTaskID;
    TaskControlBlock_t * pxTCB;

    for(;;)
    {
		if( xQueueReceive( xTaskQueue, &ucTaskID, portMAX_DELAY ) == pdPASS )
		{
			pxTCB = &pxSystemTasks[ ucTaskID ];
			eTraceEventType_t eEvent = eTraceTaskStart;

			TickType_t xCurrentTick = xTaskGetTickCount();

            switch( pxTCB->xUserTask->ePolicy )
            {
                case KILL:
					eEvent = eTraceOverrunKill;
                    vTaskDelete( pxTCB->pxTaskHandle );
					pxTCB->pxTaskHandle = NULL; //safety
                    pxTCB->ucIsRunning = 0;
					pxTCB->xReleaseTick  = pxTCB->xOverrunTick;
					
                    xTaskCreate(
                        prvPeriodicTaskWrapper,
                        pxTCB->xUserTask->pcName,
                        pxTCB->xUserTask->uxStackDepth,
                        ( void * ) pxTCB,
                        pxTCB->xUserTask->uxPriority,
                        &pxTCB->pxTaskHandle
                    );
                    break;
                
                case CATCH_UP:
		    		eEvent = eTraceOverrunCatchUp;
					pxTCB->ucIsOverrunning = 1;
                    break;
                
                case SKIP:
                    eEvent = eTraceOverrunSkip;
                    pxTCB->uiJobMiss++;
                    pxTCB->xReleaseTick = pxTCB->xOverrunTick;
                    pxTCB->xOverrunTick = pxTCB->xReleaseTick + pxTCB->xUserTask->xPeriod;
                    break;
                
                case NONE:
		    		break;
	    	}

			vRecordTraceEvent( 	xCurrentTick,
								eEvent,
								pxTCB->xUserTask->pcName,
								pxTCB->uiJobMiss,
								(xCurrentTick >= pxTCB->xDeadlineTick)
			);
		}
	}
}
