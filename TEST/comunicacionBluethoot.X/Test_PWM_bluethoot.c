/*
 * File:   Test_PWM_bluethoot.c
 * Author: Lautaro Saez
 *
 * Created on 11 de junio de 2019, 07:38 PM
 */


#define _XTAL_FREQ 48000000
#include <xc.h>
#include <pic18f47j53.h>
#include <stdlib.h>
#include <stdio.h>


#include "../../LIBRERIAS/CONFIG.h"
#include "../../LIBRERIAS/configuracion_auto.h"
#include "../../LIBRERIAS/funciones_auto.h"
#include "../../LIBRERIAS/Estados.h"

unsigned char bandera = 0, datos[10] = {'\0'}, indicador = 0, fuego=1;

void __interrupt() rutina(){
    if(PIR1bits.RC1IF == 1){
        unsigned char dato = RCREG1;
        if(dato == 'P'){
            bandera = 1;
            indicador = 0;
        }
        else{
            datos[indicador] = dato;
            indicador++;
        }
    }
}

void main(void) {
    configurarPuertos();
    configurarPWM7();
    configurarRS232();
    configurarInterrupciones();
    TRISDbits.TRISD4 = 0;
    TXREG1 = 'H';
    PORTD = 0x00;
    CCP7CONbits.DC7B = 0b10;
    CCPR7L = 0x07;
    T2CONbits.TMR2ON = 1;
    while(1){
        if(bandera == 1){
            bandera = 0;
            logicaEstadoSiguiente(datos);
        }   
        if(PORTDbits.RD5 == 1 && fuego==1){
            enviarRS232("FUEGO!", 6);
            fuego = 0;
        }
        if(PORTDbits.RD5 == 0 && fuego==0){
            enviarRS232("APAGADO", 7);
            fuego = 1;
        }
    }
    return;
}



