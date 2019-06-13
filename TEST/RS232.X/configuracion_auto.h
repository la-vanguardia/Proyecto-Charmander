#include <xc.h>

unsigned int cicle_90 = 0x01C2;
unsigned int velocidad = 0;

void configurarPWM7(){
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB7 = 0;
    CCP7CONbits.CCP7M = 0xC;
    CCPTMRS1bits.C7TSEL = 0b00;
    //TMR2 config
    T2CON = 0x02;
    PR2 = 124;
    //Duty cicle(DC) DC = CCPR / (4*(Pr2+1)
    TMR2 = 0x00;
}

void configurarRS232(){
    TRISCbits.TRISC7 = 1;
    TRISCbits.TRISC6 = 0;
    TXSTA1 = 0x22;
    RCSTA1 = 0x90;
    BAUDCON1bits.BRG16 = 0;
    SPBRG1 = 77;
    PIR1bits.RC1IF = 0;
    PIE1bits.RC1IE = 1;
}

void rutinaArranque(){
    CCP7CONbits.DC7B = cicle_90 & 0x0003;
    CCPR7L = cicle_90>>2;
    T2CONbits.TMR2ON = 1;
    __delay_ms(50);
}

void fijarVelocidad(unsigned char speed){
    float DC = 0.003 * speed + 0.6; //DC - CCPR / (4*(PR2+1))
    velocidad = 4 * DC * 125 ;
}

void enviarRS232(unsigned char *valores, unsigned char numero_valores){
    for(unsigned char i = 0; i<numero_valores; i++){
        TXREG = valores[i];
        __delay_ms(5);
    }
}