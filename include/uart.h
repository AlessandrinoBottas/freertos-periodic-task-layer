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
