#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "FreeRTOS.h"

/* Buffer size */
#define TRACE_BUFFER_SIZE           ( 256U )

/* Event types */
typedef enum
{
    eTraceTaskStart = 0,
    eTraceTaskEnd,
    eTraceDeadlineMiss,
    eTraceOverrunKill,
    eTraceOverrunSkip,
    eTraceOverrunCatchUp
} eTraceEventType_t;

/* Event structure */
typedef struct
{
    TickType_t          xTimestamp;          /* x: TickType_t = 4 bytes */
    const char          *pcTaskName;         /* pc: pointer to char = 4 bytes */
    uint16_t            usJobMiss;           /* us: unsigned short = 2 bytes */
    uint8_t             ucDeadlineMiss;      /* uc: unsigned char = 1 byte */
    eTraceEventType_t   eType;               /* e: enum = 1 byte -> Total: 12 bytes */
} TraceEvent_t;


/*** --- API --- ***/


/*
 * Initialize logger
 */
void vTraceInit( void );

/*
 * Put data in the buffer
 */
void vRecordTraceEvent( TickType_t xTimestamp, eTraceEventType_t eType, const char *pcTaskName, uint16_t usJobMiss, uint8_t ucDeadlineMiss );

/*
 * Printf with UART
 */
void vTracePrinterTask( void *pvParameters );

#endif /* __LOGGER_H__ */


