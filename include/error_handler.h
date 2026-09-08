#ifndef ERROR_HANDLER
#define ERROR_HANDLER

#include "FreeRTOS.h"
#include "task.h"

extern void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName );
extern void vAssertCalled( const char *pcFile, uint32_t ulLine );

#endif 
