/*
 * File:   main.c
 * Author: A00821429
 *
 * Created on March 2, 2021, 1:27 AM
 */


#include "device_config.h"

#define _XTAL_FREQ 1000000
#define ONE_SECOND 1000

enum por_dir{ output, input };
enum por_ACDC{digital, analog };
enum resistor_state{ set_ON, res_ON };
enum led_state{ led_OFF, led_ON};
enum butto_state{ pushed, no_pushed };

void __interrupt ( high_priority ) high_isr( void );

void __interrupt ( low_priority ) low_isr( void );

void portsInit( void );

void main( void ) {
    portsInit();
    
    while( 1 ) {
        if( PORTBbits.RB4 == pushed )
            LATAbits.LATA7 = led_OFF;
        else
            LATAbits.LATA7 = led_ON;
        LATAbits.LATA4 = led_ON;
        __delay_ms( ONE_SECOND );
        __delay_ms( ONE_SECOND );
           
    }
}

void portsInit( void ) {
    ANSELA = digital;
    TRISAbits.TRISA4 = output;
    TRISAbits.TRISA7 = output;
    ANSELB = digital;
    TRISBbits.TRISB4 = input;
    
}