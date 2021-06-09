////+++++++++++++++++++++++++++++++++++++| LIBRARIES / HEADERS |+++++++++++++++++++++++++++++++++++++
////+++++++++++++++++++++++++++++++++++++| LIBRARIES / HEADERS |+++++++++++++++++++++++++++++++++++++
#include "device_config.h"
#include <stdint.h>
#include <math.h>

//+++++++++++++++++++++++++++++++++++++| DIRECTIVES |+++++++++++++++++++++++++++++++++++++


#define _XTAL_FREQ 8000000

#define SWEEP_FREQ 50

#define LCD_DATA_R          PORTD
#define LCD_DATA_W          LATD
#define LCD_DATA_DIR        TRISD
#define LCD_RS              LATCbits.LATC2   
#define LCD_RS_DIR          TRISCbits.TRISC2
#define LCD_RW              LATCbits.LATC1
#define LCD_RW_DIR          TRISCbits.TRISC1
#define LCD_E               LATCbits.LATC0
#define LCD_E_DIR           TRISCbits.TRISC0           

//+++++++++++++++++++++++++++++++++++++| DATA TYPES |+++++++++++++++++++++++++++++++++++++
enum por_ACDC {digital, analog};          // digital = 0, analog = 1

void portsInit(void);
uint8_t char_to_seg(uint8_t);
void send_to_disp(uint32_t);
char key_scanner(void);
void LCD_init(void);
void LCD_cmd(char);
void LCD_rdy(void);
void send2LCD(char);

char myfunction(char myarr[]) {
    switch(myarr[1]) {
        case 10:
            return ((int)myarr[0] + (int)myarr[2] +'0');
            break;
        case 11:
            return ((int)myarr[0] - (int)myarr[2] +'0');
            break;
        case 12:
            return ((int)myarr[0] * (int)myarr[2] +'0');
            break;
        case 13:
            return ((int)myarr[0] / (int)myarr[2] +'0');
            break;
        default:
            break;
    }
    return 'x';
}

//+++++++++++++++++++++++++++++++++++++| MAIN |+++++++++++++++++++++++++++++++++++++
void main(void){
    portsInit();
    LCD_init();
   
}
//+++++++++++++++++++++++++++++++++++++| FUNCTIONS |+++++++++++++++++++++++++++++++++++++
void portsInit(void){
    ANSELA = digital;   
    TRISA  = 0xF0;      
    OSCCON = 0x74;      
}


char key_scanner(void){
    LATAbits.LA0 = 0;
    LATAbits.LA1 = 1;
    LATAbits.LA2 = 1;
    LATAbits.LA3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return 1;}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return 2;}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return 3;}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return 10;}
    LATAbits.LA0 = 1;
    LATAbits.LA1 = 0;
    LATAbits.LA2 = 1;
    LATAbits.LA3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return 4;}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return 5;}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return 6;}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return 11;}
    LATAbits.LA0 = 1;
    LATAbits.LA1 = 1;
    LATAbits.LA2 = 0;
    LATAbits.LA3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return 7;}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return 8;}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return 9;}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return 12;}
    LATAbits.LA0 = 1;
    LATAbits.LA1 = 1;
    LATAbits.LA2 = 1;
    LATAbits.LA3 = 0;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return 14;}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return 0;}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return 15;}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return 13;}
    else return 'x';
}


void LCD_cmd(char cx) {
    //LCD_rdy();          
    LCD_RS = 0;           
    LCD_RW = 0;           
    LCD_E  = 1;           
    Nop();
    LCD_DATA_W = cx;      
    Nop();                
    LCD_E = 0;            
}


void LCD_rdy(void) {
    char test;
    
    LCD_DATA_DIR = 0b11111111;
    test = 0x80;
    while(test){
        LCD_RS = 0;         
        LCD_RW = 1;         
        LCD_E  = 1;         
        test = LCD_DATA_R;
        Nop();
        LCD_E = 0;          
        test &= 0x80;       
    }
}

void send2LCD(char xy){
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_E  = 1;
    LCD_DATA_W = xy;
    Nop();
    Nop();
    LCD_E  = 0;
}

void LCD_init(void) {
    LATC = 0;               
    LCD_E_DIR = 0;          
    LCD_RS_DIR = 0;         
    LCD_RW_DIR = 0;         
    LCD_cmd(0x38);          
    LCD_cmd(0x0F);          
    LCD_cmd(0x01);          
}
