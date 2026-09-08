#include "uart.h"
#include "FreeRTOS.h"
#include <stdint.h>


/*-----------------------------------------------------------*/


/* --- UART FUNCTIONS --- */


void vUART_Init( void )
{
    UART0_BAUDDIV = 16UL; /* Baud rate */
    UART0_CTRL = 1UL; /* Transmission */
}


/*-----------------------------------------------------------*/


void vUART_PutString( const char *pcString )
{
    if( pcString != NULL )
    {
        while( *pcString != '\0' )
        {
            /* And betrwwn the STATE register and the mask. If the bit = 1 the buffer is full, wait */
            while( ( UART0_STATE & UART_STATE_TXBF ) != 0U ); 
            
            UART0_DATA = ( uint32_t ) ( *pcString );
            pcString++;
        }
    }   
}


/*-----------------------------------------------------------*/


