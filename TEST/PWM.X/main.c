/*
 * File:   main.c
 * Author: Lautaro Saez
 *
 * Created on 8 de junio de 2019, 08:32 PM
 */

#include "CONFIG.h"

#include <xc.h>
#include <pic18f47j53.h>
#include <stdlib.h>
#include <stdio.h>

#define _XTAL_FREQ 48000000
#define FCY (_XTAL_FREQ/4)

#define LED PORTBbits.RB4

void configurarPuertos();
void configurarTMR1();
void configurarPWM7();

unsigned char contador = 0;

void __interrupt() rutina_high(){
    if(PIR1bits.TMR1IF == 1){
        PIR1bits.TMR1IF = 0;
        contador ++;
        if(contador == 30){
            contador = 0;
            LED = ~LED;
        }
    }
}

void main(void) {
    configurarPuertos();
    configurarTMR1();
    configurarPWM7();
    while(1){
    }
    return;
}

void configurarPuertos(){
    ANCON0 = 0xFF;
    ANCON1 = 0x1F;
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB7 = 0;
}

void configurarTMR1(){
    T1CON = 0x30;
    TMR1 = 0x15A0; //interrupciones cada 10ms
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 1;
    T1CONbits.TMR1ON = 1;
}

void configurarPWM7(){
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB7 = 0;
    CCP7CONbits.CCP7M = 0xC;
    CCPTMRS1bits.C7TSEL = 0b00;
    //TMR2 config
    T2CON = 0x02;
    PR2 = 124;
    //Duty cicle(DC) DC = CCPR / (4*(Pr2+1))
    CCPR7L = 0x70; //0x07
    CCP7CONbits.DC7B = 0b10;
    T2CONbits.TMR2ON = 1;
    __delay_ms(50);
    CCPR7L = 0x4B; //0x07
    CCP7CONbits.DC7B = 0b00;
    TMR2 = 0x00;
    
}