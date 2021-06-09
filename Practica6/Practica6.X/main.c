////+++++++++++++++++++++++++++++++++++++| LIBRARIES / HEADERS |+++++++++++++++++++++++++++++++++++++
#include "device_config.h"
#include <xc.h>
#include <math.h>
#include <stdint.h>
//+++++++++++++++++++++++++++++++++++++| DIRECTIVES |+++++++++++++++++++++++++++++++++++++
#define _XTAL_FREQ 1000000
#define SWEEP_FREQ 20
#define ONE_SECOND 1000
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
enum por_dir{ output, input };              
enum por_ACDC { digital, analog };          
enum resistor_state { set_ON, res_ON };     
enum led_state { led_OFF, led_ON };         
enum butto_state { pushed, no_pushed };     

//+++++++++++++++++++++++++++++++++++++| ISRs |+++++++++++++++++++++++++++++++++++++
// ISR for high priority
void __interrupt ( high_priority ) high_isr( void );
// ISR for low priority
void __interrupt ( low_priority ) low_isr( void ); 

//+++++++++++++++++++++++++++++++++++++| FUNCTION DECLARATIONS |+++++++++++++++++++++++++++++++++++++
void LCD_rdy(void);
void LCD_init(void);
void LCD_cmd(char);
void send2LCD(char);
void portsInit(void);
char key_scanner(void);
char int_to_char_d1(int);
char int_to_char_d2(int);

char Teclas[4][4] = {{'1','2','3','+'},
                     {'4','5','6','-'},
                     {'7','8','9','*'},
                     {'C','0','E','/'}};

//+++++++++++++++++++++++++++++++++++++| MAIN |+++++++++++++++++++++++++++++++++++++
void main( void ){
    LCD_init();
    OSCCON = 0b01110110;
    LCD_DATA_DIR = 0x00;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_E  = 0;

    portsInit();
    int dig0 = 0;
    int dig1 = 0;
    int ans = 0;
    int flag0 = 0;
    int flag1 = 0;
    int flag2 = 0;
    int flag3 = 0;
    int flag4 = 0;
    while(1){
        char key = key_scanner();
        
        if(PORTAbits.RA4 == 0 | PORTAbits.RA5 == 0 | PORTAbits.RA6 == 0 | PORTAbits.RA7 == 0){
            if(key != 'E' & key != 'C'){
                LCD_E = 1;
                send2LCD(key);
                LCD_E = 0;
            }
            while(PORTAbits.RA4 == 0 | PORTAbits.RA5 == 0 | PORTAbits.RA6 == 0 | PORTAbits.RA7 == 0){ }
        }
        
        if(key != 'x' & key != '+' & key != '-' & key != '*' & key != '/' & flag0 == 0){
            dig0 = (int)key - 48;
            flag0 = 1;
        } else if(key != 'x' & key != '+' & key != '-' & key != '*' & key != '/' & flag1 == 0){
            dig1 = (int)key - 48;
            flag1 = 1;
        }
      
        switch(key){
            case '+':
                flag2 = 1;
            break;
            case '-':
                flag2 = 2;
            break;
            case '*':
                flag2 = 3;
            break;
            case '/':
                flag2 = 4;
            break;
            default:
            break;
        }
        
        if(key == 'C'){
            LCD_cmd(0xC0);
            switch(flag2){
                case 1:
                    ans = dig0 + dig1;
                break;
                case 2:
                    ans = dig0 - dig1;
                break;
                case 3:
                    ans = dig0 * dig1;
                break;
                case 4:
                    ans = dig0 / dig1;
                break;
                default:
                break;
            }
            flag3 = 1;
        }
        
        if(flag3 == 1 & flag4 < 2){
            if(flag4 == 1){
                char disp1 = int_to_char_d1(ans);
                LCD_E = 1;
                send2LCD(disp1);
                LCD_E = 0;
                LCD_cmd(0x14);          
                char disp2 = int_to_char_d2(ans);
                LCD_E = 1;
                send2LCD(disp2);
                LCD_E = 0;
            }
            flag4++;
        }
      
        if(key == 'E'){
            LCD_cmd(0x01);          
            ans = 0;
            flag0 = 0;
            flag1 = 0;
            flag2 = 0;
            flag3 = 0;
            flag4 = 0;
        }
    }
}

//+++++++++++++++++++++++++++++++++++++| FUNCTIONS |+++++++++++++++++++++++++++++++++++++

void LCD_rdy(void){
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
    LCD_DATA_DIR = 0b00000000;
}


void LCD_init(void){
    LATC = 0;               
    LCD_E_DIR = 0;          
    LCD_RS_DIR = 0;         
    LCD_RW_DIR = 0;         
    LCD_cmd(0x38);          
    LCD_cmd(0x0F);          
    LCD_cmd(0x01);          
}

// Send command to the LCD
void LCD_cmd(char cx) {
    LCD_rdy();              
    LCD_RS = 0;             
    LCD_RW = 0;             
    LCD_E  = 1;             
    Nop();
    LCD_DATA_W = cx;        
    Nop();                  
           
           
    LCD_E = 0;              
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


void portsInit(void){
    ANSELA = digital;  
    TRISA = 0xF0;      
}

char key_scanner(void){
    LATAbits.LA0 = 0;
    LATAbits.LA1 = 1;
    LATAbits.LA2 = 1;
    LATAbits.LA3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[0][0];}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[0][1];}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[0][2];}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[0][3];}
    LATAbits.LA0 = 1;
    LATAbits.LA1 = 0;
    LATAbits.LA2 = 1;
    LATAbits.LA3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[1][0];}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[1][1];}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[1][2];}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[1][3];}
    LATAbits.LA0 = 1;
    LATAbits.LA1 = 1;
    LATAbits.LA2 = 0;
    LATAbits.LA3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[2][0];}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[2][1];}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[2][2];}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[2][3];}
    LATAbits.LA0 = 1;
    LATAbits.LA1 = 1;
    LATAbits.LA2 = 1;
    LATAbits.LA3 = 0;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[3][0];}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[3][1];}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[3][2];}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return Teclas[3][3];}   
    else return 'x';
}


char int_to_char_d1(int N){
    int A = N/10;
    char p1;
    switch(A){
        case 0:
            p1 = '0';
        break;
        case 1:
            p1 = '1';
        break;
        case 2:
            p1 = '2';
        break;
        case 3:
            p1 = '3';
        break;
        case 4:
            p1 = '4';
        break;
        case 5:
            p1 = '5';
        break;
        case 6:
            p1 = '6';
        break;
        case 7:
            p1 = '7';
        break;
        case 8:
            p1 = '8';
        break;
        case 9:
            p1 = '9';
        break;
        default:
        break;
    }
    return p1;
}


char int_to_char_d2(int N){
    int B = N%10;
    char p2;
    switch(B){
        case 0:
            return p2 = '0';
        break;
        case 1:
            return p2 = '1';
        break;
        case 2:
            return p2 = '2';
        break;
        case 3:
            return p2 = '3';
        break;
        case 4:
            return p2 = '4';
        break;
        case 5:
            return p2 = '5';
        break;
        case 6:
            return p2 = '6';
        break;
        case 7:
            return p2 = '7';
        break;
        case 8:
            return p2 = '8';
        break;
        case 9:
            return p2 = '9';
        break;
        default:
        break;
    }
}