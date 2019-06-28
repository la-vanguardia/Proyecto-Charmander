#include <xc.h>

void configurarPuertos(){
    ANCON0 = 0x7F;
    ANCON1 = 0x1F;
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
}

void configurarInterrupciones(){
    PPSCON = 0;
    RPINR1 = 4;
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB0 = 1;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCON2bits.INTEDG0=0;
    INTCON2bits.INTEDG1 = 0;
    INTCONbits.INT0F = 0;
    INTCONbits.INT0E = 1;
    INTCON3bits.INT1E = 1;
    INTCON3bits.INT1F = 0;
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
    TMR5L = 0xE2;
    PIE5bits.TMR5IE = 1;
    PIR5bits.TMR5IF = 0;
}

void configurarTMR4(){
    
}

void configurarRS232US100(){
    TRISDbits.TRISD6 = 0;
    TRISDbits.TRISD7 = 1;
    RPOR23 = 6; //defino RD6 como TX2
    RPINR16  = 24; //defino RD7 como Rx2
    TXSTA2 = 0x22;
    RCSTA2 = 0x90;
    BAUDCON2bits.BRG16 = 0;
    SPBRG2 = 77;
    PIR3bits.RC2IF = 0;
    PIE3bits.RC2IE = 1;
}