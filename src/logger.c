#include "FreeRTOS.h"
#include "task.h" 
#include "logger.h"
#include "uart.h"
#include "queue.h"
#include "error_handler.h"


/*
 * Static (file private) function prototypes appaiono qui.
 */
static char *prvUIntToString( uint32_t ulValue, char *pcBuffer );
static char *prvStrCpy( char *pcDest, const char *pcSrc );
static const char *pcPrvEventTypeToName( uint8_t ucType );


/*-----------------------------------------------------------*/


/* ---- GLOBAL VARIABLES ---*/
static StaticQueue_t xStaticQueue;
static QueueHandle_t xTraceQueue = NULL;
static uint8_t ucQueueStorageArea[ TRACE_BUFFER_SIZE * sizeof(TraceEvent_t) ];


/*-----------------------------------------------------------*/


/* --- STRING UTILITY --- */


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


/*-----------------------------------------------------------*/


/*
 * Copy a string into another and return the last address
 */
static char *prvStrCpy( char *pcDest, const char *pcSrc )
{
    while( *pcSrc != '\0' )
    {
        *pcDest = *pcSrc;
        pcDest++;
        pcSrc++;
    }
    *pcDest = '\0';
    return pcDest;
}


/*-----------------------------------------------------------*/


/*
 * Convertion enum -> string
 */
static const char *pcPrvEventTypeToName( uint8_t ucType )
{
    switch( ucType )
    {
        case eTraceDeadlineMiss:
            return " DL_MISS   ";
        case eTraceOverrunKill:
            return " Ovr_KILL  ";
        case eTraceOverrunSkip:
            return " Ovr_SKIP  ";
        case eTraceOverrunCatchUp:
            return " Ovr_CATCHUP";
        case eTraceTaskStart:
            return " Start     ";
        case eTraceTaskEnd:
            return " End       ";
        default:
            return "???";
    }
}


/*-----------------------------------------------------------*/


/*
 * Format the event to log
 */
static void prvPrintEvent( uint32_t ulTimestamp, const char *pcTaskName, uint8_t ucType, uint16_t usJobMiss, uint8_t ucDeadlineMiss )
{
    char cBuffer[ 120 ]; /* Increased local stack buffer to accommodate the new boolean string */
    char *pcPtr;

    UBaseType_t uxMessagesInQueue = uxQueueMessagesWaiting( xTraceQueue );
    UBaseType_t uxSpacesAvailable = uxQueueSpacesAvailable( xTraceQueue );

    pcPtr = cBuffer;

    /* Timestamp */
    pcPtr = prvStrCpy( pcPtr, "[ " );
    pcPtr = prvUIntToString( ulTimestamp, pcPtr );

    /* Task ID */
    pcPtr = prvStrCpy( pcPtr, " ] | " );
    pcPtr = prvStrCpy( pcPtr, pcTaskName );

    /* Type */
    pcPtr = prvStrCpy( pcPtr, " | " );
    pcPtr = prvStrCpy( pcPtr, pcPrvEventTypeToName( ucType ) );

    /* Extra data */
    pcPtr = prvStrCpy( pcPtr, " | JobMiss:" );
    pcPtr = prvUIntToString( ( uint32_t ) usJobMiss, pcPtr );

    /* Deadline Miss logic */
    pcPtr = prvStrCpy( pcPtr, " | DlMiss:" );
    if( ucDeadlineMiss != 0U )
    {
        pcPtr = prvStrCpy( pcPtr, "TRUE " ); /* Added trailing space to match length of False */
    }
    else
    {
        pcPtr = prvStrCpy( pcPtr, "False" );
    }

    /* Queue metrics: {waiting} <free> */
    /*
    pcPtr = prvStrCpy( pcPtr, " | {Queue items: " );
    pcPtr = prvUIntToString( ( uint32_t ) uxMessagesInQueue, pcPtr );
    pcPtr = prvStrCpy( pcPtr, " Free: " );
    pcPtr = prvUIntToString( ( uint32_t ) uxSpacesAvailable, pcPtr );
    */

    /* Trailer */
    pcPtr = prvStrCpy( pcPtr, " \r\n" );
    

    vUART_PutString( cBuffer );
}


/*-----------------------------------------------------------*/


/* ---- INITIALIZATION ---*/

/*
 * Initialize the queue
 */
void vTraceInit( void )
{
    vUART_Init();
    
    xTraceQueue = xQueueCreateStatic ( TRACE_BUFFER_SIZE, sizeof( TraceEvent_t ), ucQueueStorageArea, &xStaticQueue );

    /*If xTraceQueue == NULL --> configAssert triggers an error*/
    configASSERT( xTraceQueue );
}


/* ---- PRODUCER ---- */

/* --- API --- */

/*
 * Write the event into the buffer 
 */
void vRecordTraceEvent( TickType_t xTimestamp, eTraceEventType_t eType, const char *pcTaskName, uint16_t usJobMiss, uint8_t ucDeadlineMiss )
{
    TraceEvent_t xEvent = { .xTimestamp = xTimestamp,
                            .pcTaskName = ( char * ) pcTaskName,
                            .usJobMiss = usJobMiss,
                            .ucDeadlineMiss = ucDeadlineMiss,
                            .eType = eType
                           }; 
    xQueueSend( xTraceQueue, &xEvent, 0);
}


/*-----------------------------------------------------------*/


/* ---- CONSUMER ---- */


/*
 * Log the buffer events
 */
void vTracePrinterTask( void *pvParameters )
{
    ( void ) pvParameters;
    TraceEvent_t xEvent;

    for( ;; )
    {
        if( xQueueReceive( xTraceQueue, &xEvent, portMAX_DELAY ) == pdPASS )
        {
            /* --- FORMATTING AND PRINTING --- */
            prvPrintEvent( xEvent.xTimestamp, xEvent.pcTaskName, xEvent.eType, xEvent.usJobMiss, xEvent.ucDeadlineMiss );
        }
    }
}

/*-----------------------------------------------------------*/

