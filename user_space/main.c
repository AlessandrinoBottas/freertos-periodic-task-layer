#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include <stdio.h> //sprintf()
#include "PTL.h"
#include <stdint.h>
#include "logger.h"

#define T_PRIO PTL_MIN_PRIORITY + 1

void vBusyDelayTask(void *pvParameters);

/* Carichi di lavoro (durata dell'esecuzione in millisecondi) */
static uint32_t ulPayloadLight   = 5;   // 5 ms
static uint32_t ulPayloadMedium  = 15;  // 15 ms
static uint32_t ulPayloadHeavy   = 40;  // 40 ms
static uint32_t ulPayloadExtreme = 80;  // 80 ms

static TaskConfig_t xUserTasksConfig[] = {
    {
        .pcName = "T_Light",
        .uxStackDepth = 256,
        .uxPriority = T_PRIO + 3,
        .pvTaskCode = vBusyDelayTask,
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
        .pvTaskCode = vBusyDelayTask,
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
        .pvTaskCode = vBusyDelayTask,
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
        .pvTaskCode = vBusyDelayTask,
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
 * Simulazione reale di carico sulla CPU.
 * Sostituisce i calcoli pesanti con un busy-wait basato sui tick,
 * consumando cicli macchina in modo misurabile e prevedibile.
 */
void vBusyDelayTask(void *pvParameters) {
    uint32_t duration_ms = *(uint32_t*)pvParameters;
    TickType_t xStart = xTaskGetTickCount();
    
    while ((xTaskGetTickCount() - xStart) < pdMS_TO_TICKS(duration_ms)) {
        __asm volatile ("nop");
    }
}
