/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

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


