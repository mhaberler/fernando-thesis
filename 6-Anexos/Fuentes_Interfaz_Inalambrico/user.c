/******************************************************************************/
/* Files to Include                                                           */
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

#include "user.h"
#include "nRF24L01.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    // Conexiones de Pines de uC
    // -------------------------

    // RC1/T1OSI/CCP2   CE al nRF24
    // RC2/CCP1         CSN al nRF24
    // RC3/SCK/SCL      SCK al nRF24
    // RC4/SDI/SDA      MISO desde nRF24
    // RC5/SDO          MOSI al nRF24
    // RB2/INT2         IRQ desde nRF24
    // Nota: La alimentación al uC DEBE ser de 3.3V para interfaz correcta
    // al módulo inalámbrico

    // RB0/INT0         Fase A desde encoder rotativo
    // RB1/INT1         Fase B desde encoder rotativo

    // Todo PortA salidas para evitar ruidos
    TRISA=0b0000000;
    // Todo PortB entradas digitales (como tenemos pull-ups no hay ruidos)
    TRISB=0b11111111;       // esta es la config. por defecto se puede obviar
    // Activamos pull-ups de entradas de PortB
    INTCON2bits.RBPU=0;
    // PortC salidas/entradas hacia y desde módulo nRF
    TRISC=0b11010001;

    // Configuramos todos los pines digitales
    ADCON1bits.PCFG=0b0110;

    // Configuración de módulo SPI se configura en funciones de spi.c

    // Configuración de interrupciones
    // -------------------------------
    // Para comunicación eficiente con el nRF se pude utilizar su línea de interrupción
    // IRQ en línea INT2 del uC
    // Para lectura de encoder se utiliza interrupción por cambio en pines de PortB
    // esto tiene la desventaja de ocasionar interrupciones también con las transiciones
    // en la linea IRQ del nRF24. Aunque esto no ocasiona problemas directamente
    // sí será un consumo innecesario de tiempo de procesador, habrá que ver cuanto es.

    RCONbits.IPEN=1;        // habilitamos interrupciones priorizadas

    INTCON2bits.RBIP=1;     // interrupción por cambio en PortB alta prio
    INTCONbits.RBIE=1;      // habilitamos interrupciones por cambio en PortB
    INTCON3bits.INT2IP=0;   // interrupción INT2 baja prio
    INTCON2bits.INTEDG2=0;  // interrupcion INT2 flanco desc. (IRQ desde nRF)
    INTCON3bits.INT2IE=1;   // interrupción IRQ del módulo wireless
    
    INTCONbits.RBIF=0;      // restablecemos flag cambio en PortB
    INTCON3bits.INT2IF=0;

    // Arranque de interrupciones
    INTCONbits.GIE=1;   // arrancamos interrupciones altas
    //INTCONbits.PEIE=1;  // arrancamos interrupciones bajas

    // Se configura e inicia en modo primary TX el módulo wireless
    nRF_Setup();
}

