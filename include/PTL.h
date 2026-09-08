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
