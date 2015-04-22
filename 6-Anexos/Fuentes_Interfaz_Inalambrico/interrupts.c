/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "main.h"

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

// Order is 00 -> 10 -> 11 -> 01
static const signed char lookup[4][4] = {
		{ 0,    -1,     +1,     0   },
		{ +1,   0,      0,      -1  },
		{ -1,   0,      0,      +1  },
		{ 0,    +1,     -1,     0   },
};

/* High-priority service */

#if defined(__XC) || defined(HI_TECH_C)
void interrupt high_isr(void)
#elif defined (__18CXX)
#pragma code high_isr=0x08
#pragma interrupt high_isr
void high_isr(void)
#else
#error "Invalid compiler selection for implemented ISR routines"
#endif

{
    static char pines_act, pines_ant;

    // Unica fuente de interrupcion de alta prioridad es cambio en PortB
    // Hacemos una única lectura del puerto B, esto es IMPORTANTE porque
    // en ese instante se restablece el flag RBIF.
    // No se debe volver a leer el puerto B porque puede perderse un nuevo
    // cambio en alguno de los pines
    pines_act = PORTB & 0b11;
    // hacemos búsqueda en tabla con estado actual y anterior
    contador += lookup[pines_act][pines_ant];
    // registramos estado de pines
    pines_ant = pines_act;

    // salimos de la rutina sin necesidad de restablecer ningún flag
    // el flag RBIF se restableció automáticamente al momento de leer PORTB
}

/* Low-priority interrupt routine */
#if defined(__XC) || defined(HI_TECH_C)
void low_priority interrupt low_isr(void)
#elif defined (__18CXX)
#pragma code low_isr=0x18
#pragma interruptlow low_isr
void low_isr(void)
#else
#error "Invalid compiler selection for implemented ISR routines"
#endif
{
    // la única fuente de interrupción de baja prioridad debería ser la IRQ del
    // módulo wireless

    // restablecemos su flag
    INTCON3bits.INT2IF=0;

      /* This code stub shows general interrupt handling.  Note that these
      conditional statements are not handled within 3 seperate if blocks.
      Do not use a seperate if block for each interrupt flag to avoid run
      time errors. */

#if 0

      /* TODO Add Low Priority interrupt routine code here. */

      /* Determine which flag generated the interrupt */
      if(<Interrupt Flag 1>)
      {
          <Interrupt Flag 1=0>; /* Clear Interrupt Flag 1 */
      }
      else if (<Interrupt Flag 2>)
      {
          <Interrupt Flag 2=0>; /* Clear Interrupt Flag 2 */
      }
      else
      {
          /* Unhandled interrupts */
      }

#endif

}
