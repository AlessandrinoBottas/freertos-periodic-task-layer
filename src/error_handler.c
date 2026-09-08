#include "FreeRTOS.h"
#include "error_handler.h"
#include <uart.h>

/*
 * Conversion uint -> string
 */
static char *prvUIntToString( uint32_t ulValue, char *pcBuffer )
{
    char cTemp[ 10 ];
    int i = 0;

    /* Special case if value == 0 */
    if( ulValue == 0U )
    {
        *pcBuffer = '0';
        pcBuffer++;
        *pcBuffer = '\0';
        return pcBuffer;
    }

    /* Save last number as string (%10 + '0') and go for the next number (/= 10) */
    do {
        cTemp[ i ] = ( char ) ( ( ulValue % 10U ) + '0' );
        i++;
        ulValue /= 10U;
    } while( ulValue > 0U );


    /* Invert the string and add '/0' */
    while( i > 0 )
    {
        i--;
        *pcBuffer = cTemp[ i ];
        pcBuffer++;
    }
    *pcBuffer = '\0';

    return pcBuffer;
}


/* Hook function for stack overflow mod 2 (see configCHECK_FOR_STACK_OVERFLOW and documentation) */
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    ( void ) xTask;

    vUART_PutString("\n\r######################################");
    vUART_PutString("\n\r[FATAL ERROR] DETECTED STACK OVERFLOW!");
    vUART_PutString("\n\rGuilty Task: ");
    vUART_PutString( pcTaskName );
    vUART_PutString("\n\r######################################\n\r");
    
    taskDISABLE_INTERRUPTS();

    /* This infinite cycle allows debugging with GDB */
    for( ;; );
}

/* Prints the FILE and the LINE where the error occurred */
void vAssertCalled( const char *pcFile, uint32_t ulLine )
{
    char cLineBuf[ 12 ];
    
    vUART_PutString("\n\r[ASSERT FAILED] ");
    vUART_PutString(pcFile);
    vUART_PutString(" : line ");
    
    prvUIntToString(ulLine, cLineBuf);
    
    vUART_PutString( cLineBuf );
    vUART_PutString( "\n\r" );
  
    /* Disabling interruption is fundamental when the system enters an incoherent state */
    taskDISABLE_INTERRUPTS();
    for( ;; ); 
}
