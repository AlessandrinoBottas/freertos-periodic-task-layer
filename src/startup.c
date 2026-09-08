#include "uart.h"

/* FreeRTOS interrupt handlers. */
extern void vPortSVCHandler( void );
extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );

/* Exception handlers. */
static void HardFault_Handler( void ) __attribute__( ( naked ) );
static void Default_Handler( void ) __attribute__( ( naked ) );
void Reset_Handler( void ) __attribute__( ( naked ) );

extern int main( void );
extern uint32_t _estack;

/* Vector table. */
const uint32_t* isr_vector[] __attribute__((section(".isr_vector"), used)) =
{
    ( uint32_t * ) &_estack,
    ( uint32_t * ) &Reset_Handler,     // Reset                -15
    ( uint32_t * ) &Default_Handler,   // NMI_Handler          -14
    ( uint32_t * ) &HardFault_Handler, // HardFault_Handler    -13
    ( uint32_t * ) &Default_Handler,   // MemManage_Handler    -12
    ( uint32_t * ) &Default_Handler,   // BusFault_Handler     -11
    ( uint32_t * ) &Default_Handler,   // UsageFault_Handler   -10
    0, // reserved   -9
    0, // reserved   -8
    0, // reserved   -7
    0, // reserved   -6
    ( uint32_t * ) &vPortSVCHandler,    // SVC_Handler          -5
    ( uint32_t * ) &Default_Handler,    // DebugMon_Handler     -4
    0, // reserved   -3
    ( uint32_t * ) &xPortPendSVHandler, // PendSV handler       -2
    ( uint32_t * ) &xPortSysTickHandler,// SysTick_Handler      -1
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, // Timer 0
    0, // Timer 1
    0,
    0,
    0,
    0, // Ethernet   13
    0,
};

void Reset_Handler( void )
{
    main();
}

/* Variables used to store the value of registers at the time a hardfault
 * occurs.  These are volatile to try and prevent the compiler/linker optimizing
 * them away as the variables never actually get used. */
volatile uint32_t r0;
volatile uint32_t r1;
volatile uint32_t r2;
volatile uint32_t r3;
volatile uint32_t r12;
volatile uint32_t lr; /* Link register. */
volatile uint32_t pc; /* Program counter. */
volatile uint32_t psr;/* Program status register. */

/* Called from the hardfault handler to provide information on the processor
 * state at the time of the fault.
 */
__attribute__( ( used ) ) void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    vUART_PutString( "Calling prvGetRegistersFromStack() from fault handler" );
    //fflush( stdout );

    /* When the following line is hit, the variables contain the register values. */
    for( ;; );
}


void Default_Handler( void )
{
    __asm volatile
    (
        ".align 8                                \n"
        " ldr r3, =0xe000ed04                    \n" /* Load the address of the interrupt control register into r3. */
        " ldr r2, [r3, #0]                       \n" /* Load the value of the interrupt control register into r2. */
        " uxtb r2, r2                            \n" /* The interrupt number is in the least significant byte - clear all other bits. */
        "Infinite_Loop:                          \n" /* Sit in an infinite loop - the number of the executing interrupt is held in r2. */
        " b  Infinite_Loop                       \n"
        " .ltorg                                 \n"
    );
}

void HardFault_Handler( void )
{
    __asm volatile
    (
        ".align 8                                                   \n"
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, =prvGetRegistersFromStack                         \n"
        " bx r2                                                     \n"
        " .ltorg                                                    \n"
    );
}
