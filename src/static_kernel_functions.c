#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"




/*-----------------------------------------------------------*/
/* FreeRTOS kernel functions for static allocation           */
/*-----------------------------------------------------------*/

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */


void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
    /* We are simply reserving STATIC space (thus, not onto the stack) 
       and giving the corrispondent addresses to the kernel.
       Infact, if static allocation is enabled, it cannot decide on its own. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


