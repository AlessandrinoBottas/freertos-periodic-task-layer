#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include <stdio.h> //sprintf()
#include "PTL.h"
#include <stdint.h>
#include "logger.h"

#define T_PRIO PTL_MIN_PRIORITY + 1

void vFibonacciTask(void *pvParameters);

/* Carichi di lavoro (n-esimo numero di Fibonacci da calcolare) */
static uint32_t ulPayloadLight   = 10000;
static uint32_t ulPayloadMedium  = 30000;
static uint32_t ulPayloadHeavy   = 60000;
static uint32_t ulPayloadExtreme = 100000;

static TaskConfig_t xUserTasksConfig[] = {
    {
        .pcName = "T_Light",
        .uxStackDepth = 256,
        .uxPriority = T_PRIO + 3,
        .pvTaskCode = vFibonacciTask,
        .pvParameters = (void*)&ulPayloadLight,
        .xPeriod = 20,
        .xDeadline = 20,
        .xPhase = 0,
        .ePolicy = KILL,
        .cIsPeriodic = 1
    },
    {
        .pcName = "T_Medium",
        .uxStackDepth = 256,
        .uxPriority = T_PRIO + 2,
        .pvTaskCode = vFibonacciTask,
        .pvParameters = (void*)&ulPayloadMedium,
        .xPeriod = 50,
        .xDeadline = 50,
        .xPhase = 0,
        .ePolicy = KILL,
        .cIsPeriodic = 1
    },
    {
        .pcName = "T_Heavy",
        .uxStackDepth = 256,
        .uxPriority = T_PRIO + 1,
        .pvTaskCode = vFibonacciTask,
        .pvParameters = (void*)&ulPayloadHeavy,
        .xPeriod = 100,
        .xDeadline = 100,
        .xPhase = 0,
        .ePolicy = CATCH_UP,
        .cIsPeriodic = 1
    },
    {
        .pcName = "T_Extreme",
        .uxStackDepth = 256,
        .uxPriority = T_PRIO,
        .pvTaskCode = vFibonacciTask,
        .pvParameters = (void*)&ulPayloadExtreme,
        .xPeriod = 200,
        .xDeadline = 200,
        .xPhase = 0,
        .ePolicy = SKIP,
        .cIsPeriodic = 1
    }
};

int main(int argc, char **argv){
    (void) argc;
    (void) argv;

    vTraceInit();
    vUART_Init();

    if( xInitScheduler(xUserTasksConfig, sizeof(xUserTasksConfig) / sizeof(TaskConfig_t)) == pdTRUE )
    {
        vUART_PutString( "--- TASKS SUCCESSFULLY CREATED ---\n");
        vTaskStartScheduler();
    }
    else
    {
        vUART_PutString( "Some error occurred. Reset the CPU for cleaning the RAM\n" );
        return 1;
    }
}

/* 
 * Real workload that consumes CPU calculating the n-th Fibonacci number.
 * Using volatile to prevent compiler optimizations from skipping the loop.
 */
void vFibonacciTask(void *pvParameters) {
    uint32_t n = *(uint32_t*)pvParameters;
    
    volatile uint32_t a = 0;
    volatile uint32_t b = 1;
    volatile uint32_t c = 0;
    
    for (uint32_t i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }
}
