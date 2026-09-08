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

#ifndef TASK_CONTROL_BLOCK_H
#define TASK_CONTROL_BLOCK_H

#include "PTL.h"
typedef struct
{
    const TaskConfig_t * xUserTask;
    TaskHandle_t pxTaskHandle;	/* Changes on every xTaskCreate() */
    TickType_t xReleaseTick;
    TickType_t xOverrunTick;
	TickType_t xDeadlineTick;
    uint8_t ucIsOverrunning;
    uint8_t ucIsRunning;        /* N.B. job running != status of the FreeRTOS Task!!!!! */
    uint32_t uiJobMiss;
    
    /* As xUserTask is declared as "const" and "xPhase" has to be reset in KILL policy, we add a xCurrentPhase attribute */
    TickType_t xCurrentPhase;           
    
    StaticTask_t * pxTCB;
    StackType_t  * pxStackBuffer;

} TaskControlBlock_t;

#endif
