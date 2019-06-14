#include <xc.h>

void configurarPuertos(){
    ANCON0 = 0x7F;
    ANCON1 = 0x1F;
}

void configurarInterrupciones(){
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
}

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

void configurarTMR5(){
    TRISDbits.TRISD4 = 0;
    PORTDbits.RD4 = 0;
    T5CON = 0x32;
    TMR5H = 0xFF;
    TMR5L = 0x6A;
    PIE5bits.TMR5IE = 1;
    PIR5bits.TMR5IF = 0;
}
