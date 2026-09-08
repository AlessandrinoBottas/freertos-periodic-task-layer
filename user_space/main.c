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
static uint32_t ulPayloadP1   = 10;  // 10 ms
static uint32_t ulPayloadP2_1 = 20;  // 20 ms
static uint32_t ulPayloadP2_2 = 80;  // 80 ms

static TaskConfig_t xUserTasksConfig[] = {
    {
        .pcName = "P1",
        .uxStackDepth = 256,
        .uxPriority = T_PRIO + 2,   // Alta priorità
        .pvTaskCode = vBusyDelayTask,
        .pvParameters = (void*)&ulPayloadP1,
        .xPeriod = 50,
        .xDeadline = 50,
        .xPhase = 0,
        .ePolicy = KILL,
        .cIsPeriodic = 1
    },
    {
        .pcName = "P2.1",
        .uxStackDepth = 256,
        .uxPriority = T_PRIO + 1,   // Priorità media (stessa di P2.2)
        .pvTaskCode = vBusyDelayTask,
        .pvParameters = (void*)&ulPayloadP2_1,
        .xPeriod = 100,
        .xDeadline = 100,
        .xPhase = 0,
        .ePolicy = KILL,
        .cIsPeriodic = 1
    },
    {
        .pcName = "P2.2",
        .uxStackDepth = 256,
        .uxPriority = T_PRIO + 1,   // Priorità media (stessa di P2.1)
        .pvTaskCode = vBusyDelayTask,
        .pvParameters = (void*)&ulPayloadP2_2,
        .xPeriod = 100,
        .xDeadline = 100,
        .xPhase = 0,
        .ePolicy = KILL,            // Sicuramente andrà in Overrun (Kill)
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
