
#include "device_config.h" 
#define _XTAL_FREQ 1000000 

float tiemposc;//Declaring float variable tiemposc
int pwmL;//Declaring int variable pwmL
char PeriodPWM (float period);//Declaring PeriodPWM function for obtaining PR2

void portsInit(void){//Declaring portsInit function
    ANSELAbits.ANSA0 = 1;//Setting RA0 as analog
    TRISAbits.RA0 = 1;//Setting RA0 as input
    ANSELAbits.ANSA5 = 0;//Setting RA5 as digital
    TRISAbits.RA5 = 0;//Setting RA5 as output
    ANSELCbits.ANSC2 = 0; //Setting RC2 as digital
    ADCON0 = 0b00000001;//Selecting AN0 and turning ADC off
    ADCON1 = 0b00000000;//Selecting VDD and VSS as reference voltages
    ADCON2 = 0b00111111;//Left justified, 4 TAD, Fosc/2
}

void pwmInit(void){
    PR2 = Periodpwm( 0.004);//Calls the function pwmPeriod with the value 0.004 as input and assigns it to PR2 register
}

char Periodpwm( float period ){
    char x;//Declaring x char variable
    tiemposc = 1 / _XTAL_FREQ;//Defining tiemposc as 1 over the cristal frequency
    x = (period/( (float)4 * tiemposc * (float)16))- (float)1;//Defining x as the value obtained of the formula to obtain the period from the Datasheet
    return x;//Returns x value
}

void main(void) {
    portsInit();//Ports initialization
    TRISCbits.RC2 = 0; //Setting CCP1 as output
    pwmInit();//Calls pwmInit function
    CCPR1L = 0;//Sets the value of the 8 MSB of the duty cycle to 0 
    T2CON = 0b00000010; //Setting prescaler as 16 and timer2 off
    CCP1CON = 0b00011111;//Set CCP1 port as pwm and assigns 1 as the LSB of the pwm duty cycle
    TMR2 = 0;//Sets value of timer2 as 0
    T2CONbits.TMR2ON = 1;//Turn tmr2 on
    LATAbits.LATA5 = 0;//Turn RA5 off
    while(1){
        ADCON0bits.GO_DONE = 1;//Start ADC convertion
        while(ADCON0bits.GO_DONE == 0);//While ADC convertion is off
            pwmL = ADRESL>>6;//Move ADRESL value 6 bits to the left and assign the value to pwmlow
            CCPR1L = ADRESH;//Assign ADRESH to CCPR1L
            CCP1CONbits.DC1B = pwmL;//Assign the LSB to CCP1CON
            if (CCPR1L > 0b10111011){//If the value of CCPR1L is higher than 127 ->75% of duty cycle
                LATAbits.LATA5 = 1;//Turn on RA5
            }
            else {
                LATAbits.LATA5 = 0;//Turn off RA5 
            }
    }
}