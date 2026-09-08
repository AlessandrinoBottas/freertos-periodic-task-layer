#ifndef PTL_H
#define PTL_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

#define PTL_MAX_PRIORITY (configMAX_PRIORITIES - 2)
#define PTL_MIN_PRIORITY (tskIDLE_PRIORITY + 2)

typedef enum {
    SKIP,       /* SKIP the new job; let the late one finish */
    KILL,       /* KILL the running job immediately and release the new one, mark prev job as missed*/
    CATCH_UP,   /* release now, mark previous job missed, keep nominal cadence */
    NONE
} OverrunPolicy_t;

typedef struct {
    /* FreeRTOS xTaskCreate() Params */
    TaskFunction_t pvTaskCode;
    const char * pcName;
    configSTACK_DEPTH_TYPE uxStackDepth;
    void *pvParameters;
    UBaseType_t uxPriority;
    /* Periodic Task Params */
    TickType_t xPeriod;       // Activation period (in Tick)
    TickType_t xDeadline;     // Relative deadline (in Tick)
    TickType_t xPhase;        // Initial phase (in Tick)
    OverrunPolicy_t ePolicy;  // Policy to apply
    uint8_t cIsPeriodic;
    
    StackType_t *pxStackBuffer;
    StaticTask_t *pxTCBBuffer;
} TaskConfig_t;


BaseType_t xInitScheduler(TaskConfig_t *pxTasks, size_t uxNumTasks);

#endif
