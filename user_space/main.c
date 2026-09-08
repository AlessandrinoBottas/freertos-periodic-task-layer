#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include <stdio.h> //sprintf()
#include "PTL.h"
#include <stdint.h>
#include "logger.h"


#define LOOPS_PER_MS 1213UL
#define WORKLOAD_1MS (LOOPS_PER_MS - 50)  //giving some rest not fully occuping the CPU
#define PERIOD 11

#define T_PRIO PTL_MIN_PRIORITY + 1
#define P_PRIO PTL_MIN_PRIORITY + 2


void vDummyTask(void *pvParameters);
void vOverheadProbeTask(void *pvParameters);
static uint32_t ulIgnoredWorkload 		= 0 * WORKLOAD_1MS;
static uint32_t ulWorkload 				= 3 * WORKLOAD_1MS;
static uint32_t ulOverruningWorkload 	= 7 * WORKLOAD_1MS;


static TaskConfig_t xUserTasksConfig[] = {
    {
        .pcName = "T1",
        .uxStackDepth = 256,
        .uxPriority = T_PRIO,
        .pvTaskCode = vDummyTask,
        .pvParameters = (void*)&ulWorkload,
        .xPeriod = PERIOD,
        .xDeadline = PERIOD,
        .xPhase = 0,
        .ePolicy = KILL,
		.cIsPeriodic = 1
    },
	{
        .pcName = "T2",
        .uxStackDepth = 256,
        .uxPriority = T_PRIO + 1,
        .pvTaskCode = vDummyTask,
        .pvParameters = (void*)&ulOverruningWorkload,
        .xPeriod = PERIOD,
        .xDeadline = 5,
        .xPhase = 0,
        .ePolicy = KILL,
        .cIsPeriodic = 1
    },
	{
        .pcName = "T3",
        .uxStackDepth = 256,
        .uxPriority = P_PRIO, // Priorità minima (vittima)
        .pvTaskCode = vDummyTask,
        .pvParameters = (void*)&ulIgnoredWorkload,
        .xPeriod = PERIOD,     // Scatta ogni 10 ms
        .xDeadline = PERIOD,   // Deadline immediata
        .xPhase = 0,       // Tutti sincronizzati a Phase=0
        .ePolicy = KILL,    // Massimo overhead di gestione
        .cIsPeriodic = 1
    },
	{
        .pcName = "T4",
        .uxStackDepth = 256,
        .uxPriority = P_PRIO, // Priorità minima (vittima)
        .pvTaskCode = vDummyTask,
        .pvParameters = (void*)&ulIgnoredWorkload,
        .xPeriod = PERIOD,     // Scatta ogni 10 ms
        .xDeadline = PERIOD,   // Deadline immediata
        .xPhase = 0,       // Tutti sincronizzati a Phase=0
        .ePolicy = KILL,    // Massimo overhead di gestione
        .cIsPeriodic = 1
    },
	{
        .pcName = "T5",
        .uxStackDepth = 256,
        .uxPriority = P_PRIO, // Priorità minima (vittima)
        .pvTaskCode = vDummyTask,
        .pvParameters = (void*)&ulIgnoredWorkload,
        .xPeriod = PERIOD,     // Scatta ogni 10 ms
        .xDeadline = PERIOD,   // Deadline immediata
        .xPhase = 0,       // Tutti sincronizzati a Phase=0
        .ePolicy = KILL,    // Massimo overhead di gestione
        .cIsPeriodic = 1
    },
	{
        .pcName = "T6",
        .uxStackDepth = 256,
        .uxPriority = P_PRIO, // Priorità minima (vittima)
        .pvTaskCode = vDummyTask,
        .pvParameters = (void*)&ulIgnoredWorkload,
        .xPeriod = PERIOD,     // Scatta ogni 10 ms
        .xDeadline = 9,   // Deadline immediata
        .xPhase = 0,       // Tutti sincronizzati a Phase=0
        .ePolicy = KILL,    // Massimo overhead di gestione
		.cIsPeriodic = 1
    },
	{
        .pcName = "T7",
        .uxStackDepth = 256,
        .uxPriority = P_PRIO, // Priorità minima (vittima)
        .pvTaskCode = vDummyTask,
        .pvParameters = (void*)&ulIgnoredWorkload,
        .xPeriod = PERIOD,     // Scatta ogni 10 ms
        .xDeadline = 9,   // Deadline immediata
        .xPhase = 0,       // Tutti sincronizzati a Phase=0
        .ePolicy = KILL,    // Massimo overhead di gestione
        .cIsPeriodic = 1
    },
	{
        .pcName = "T8",
        .uxStackDepth = 256,
        .uxPriority = P_PRIO, // Priorità minima (vittima)
        .pvTaskCode = vDummyTask,
        .pvParameters = (void*)&ulIgnoredWorkload,
        .xPeriod = PERIOD,     // Scatta ogni 10 ms
        .xDeadline = 9,   // Deadline immediata
        .xPhase = 0,       // Tutti sincronizzati a Phase=0
        .ePolicy = KILL,    // Massimo overhead di gestione
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
		/* COMMENT OR UNCOMMENT FOR PROBING */
        //xTaskCreate(vOverheadProbeTask, "Probe", 228, NULL, PTL_MIN_PRIORITY, NULL);
        vUART_PutString( "---TASK SUCCESSFULLY CREATED ---\n");

        vTaskStartScheduler();
    }
    else
    {
        vUART_PutString( "Some error occurred. Reset the CPU for cleaning the RAM\n" );
        //if heap_1.c is used, then resources cannot be freed
        //vReleaseResources(  sizeof(xUserTasksConfig) / sizeof(TaskConfig_t) );
        return 1;
    }
}

void vDummyTask(void *pvParameters) {
	uint32_t  workload = *(uint32_t*)pvParameters;
	/*
	*  Let the argument being > TaskPeriod for triggering POLICIES
	*  Otherwhise comment the task Delay
	*/
	//vTaskDelay(pdMS_TO_TICKS(2 * PERIOD));
	for (volatile uint32_t i = 0; i < workload; i++) {
        __asm volatile ("nop");
    }
}


void vOverheadProbeTask(void *pvParameters) {
    ( void ) pvParameters;
    TickType_t xStart, xEnd, xElapsed;
    const uint32_t ulTargetMs = 10000;
    char buf[128];

    vUART_PutString( "[vOverheadProbeTas] --- STARTING OVERHEAD MEASUREMENT ---\n\n" );
    vTaskDelay(pdMS_TO_TICKS(1000));

    xStart = xTaskGetTickCount();

    volatile uint32_t i = ulTargetMs * LOOPS_PER_MS;
    while(i > 0) { __asm volatile ("nop"); i--; }

	vTaskSuspendAll();

    xEnd = xTaskGetTickCount();

    xElapsed = xEnd - xStart;

    uint32_t ulOverheadMs = xElapsed - ulTargetMs;
    uint32_t ulTotalPercentx100 = (ulOverheadMs * 10000) / ulTargetMs;

    uint32_t ulPercentInt = ulTotalPercentx100 / 100;
    uint32_t ulPercentDec = ulTotalPercentx100 % 100;

    sprintf(buf, "Target: %lums | Real: %lu ms | OH: %lu ms (%lu.%02lu%%)\n",
                ulTargetMs, xElapsed, ulOverheadMs, ulPercentInt, ulPercentDec);

    vUART_PutString( buf );

    vTaskSuspend(NULL);
}
