/*
 * File:   Practice2.c
 * Author: A00821429
 *
 * Created on March 16, 2021, 7:00 PM
 */


#include <xc.h>
#include "device_config.h"
#include <math.h>

//Directives
#define _XTAL_FREQ 1000000
#define ONE_SECOND 1000


//Data types

enum exponent{bbase = 2, limit = 8};
enum por_dir{ output, input };
enum por_ACDC{digital, analog };
enum resistor_state{ set_ON, res_ON };
enum led_state{ led_OFF, led_ON};
enum butto_state{ pushed, no_pushed };
void portsInit(void);
void __interrupt ( high_priority ) high_isr( void );
void __interrupt ( low_priority ) low_isr( void );


//Function declaration

void portsInit(void){
    //Digital ports
    ANSELA = digital;
    ANSELD = digital;
    //LEDS
    TRISA=0x00;
    TRISD=0xFF;   
}

//Functions



void main(void) {
    portsInit();
    char p_led;
    char b_status;
    
    while(1){
        char select = rand() %8;
        switch(select){
            case 0:
                    p_led = 0x01; // 0000 0001
            break;
            case 1:
                    p_led = 0x02; // 0000 0010
            break; 
            case 2:
                    p_led = 0x04; // 0000 0100
            break; 
            case 3:
                    p_led = 0x08; // 0000 1000
            break; 
            case 4:
                    p_led = 0x10; // 0001 0000
            break; 
            case 5:
                    p_led = 0x20; // 0010 0000
            break; 
            case 6:
                    p_led = 0x40; // 0100 0000
            break; 
            default:
                    p_led = 0x80; // 1000 0000
            break; 
        } 
        __delay_ms(ONE_SECOND);
        LATA = p_led;
        b_status = ~PORTD;
        if (p_led == b_status){
            for (int i = 0; i<8; i++){
                LATA = pow(2, i); // 2^2 = 4 -> 0000 1000
                __delay_ms(200);
            }
            for (int i = 7; i>=0; i--){
                LATA = pow(2, i); // 2^2 = 4 -> 0000 1000
                __delay_ms(200);
            }
        }
    }
}

