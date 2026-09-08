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

#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

#define UART0_ADDRESS                ( 0x40004000UL )
#define UART0_DATA                   ( *( ( volatile uint32_t * ) ( UART0_ADDRESS + 0UL ) ) )
#define UART0_STATE                  ( *( ( volatile uint32_t * ) ( UART0_ADDRESS + 4UL ) ) )
#define UART0_CTRL                   ( *( ( volatile uint32_t * ) ( UART0_ADDRESS + 8UL ) ) )
#define UART0_BAUDDIV                ( *( ( volatile uint32_t * ) ( UART0_ADDRESS + 16UL ) ) )

/* Mask 0001 to isolate the bit that check if the transmission buffer is full */
#define UART_STATE_TXBF              ( 1UL << 0 )

void vUART_Init( void );
void vUART_PutString( const char *pcString );

#endif /* __UART_H__ */
