/*
 * File:   serial.c
 * Author: Lautaro Saez
 *
 * Created on 10 de junio de 2019, 08:09 PM
 */


#include <xc.h>
#include <pic18f47j53.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "CONFIG.h"

#define _XTAL_FREQ 48000000
#define FCY (_XTAL_FREQ/4)

void configuracionInicial();
void configurarPuertos();
void configurarRS232();
void configurarInterrupciones();
void enviarDatos();
void derecha();
void izquierda();
void atras();
void adelante();

unsigned char bandera = 0, dato = 0;

void __interrupt() rutina(){
    if(PIR1bits.RC1IF == 1){
        unsigned char aux = RCREG1;
        if(aux == 0x0D){
            bandera = 1;
        }
        else{
            dato = aux;
        }
    }
}

void main(void) {
    configuracionInicial();
    TXREG1 = 'H';
    float seno = sin(6);
    while(1){
        if(bandera == 1){
            bandera = 0;
            switch(dato){
                case 'w':
                    adelante();
                    break;
                case 's':
                    atras();
                    break;
                case 'd':
                    derecha();
                    break;
                case 'a':
                    izquierda();
                    break;
            }
        }
    }
    return;
}

void configuracionInicial(){
    configurarPuertos();
    configurarRS232();
    configurarInterrupciones();
}

void configurarPuertos(){
    ANCON0 = 0xFF;
    ANCON1 = 0x1F;
    
    TRISBbits.TRISB5 = 0;
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB2 = 0;
}

void configurarRS232(){
    TRISCbits.TRISC7 = 1;
    TRISCbits.TRISC6 = 0;
    TXSTA1 = 0x22;
    RCSTA1 = 0x90;
    BAUDCON1bits.BRG16 = 0;
    SPBRG1 = 77;
}

void configurarInterrupciones(){
    RCONbits.IPEN = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIR1bits.RC1IF = 0;
    PIE1bits.RC1IE = 1;
}

void adelante(){
    PORTBbits.RB5 = 1;
    PORTBbits.RB4 = 0;
}

void atras(){
    PORTBbits.RB5 = 0;
    PORTBbits.RB4 = 1;
}

void izquierda(){
    PORTBbits.RB3 = 1;
    PORTBbits.RB2 = 0;
}

void derecha(){
    PORTBbits.RB3 = 0;
    PORTBbits.RB2 = 1;
}