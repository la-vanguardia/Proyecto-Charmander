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

#include "../PIC18F47J53/CONFIG.h"
#include "../PIC18F47J53/configuracion_auto.h"
#include "../PIC18F47J53/funciones_auto.h"



unsigned char bandera = 0, datos[10] = {0}, indicador = 0;

void logicaEstadoSiguiente();

void __interrupt() rutina(){
    if(PIR1bits.RC1IF == 1){
        unsigned char dato = RCREG1;
        if(dato == 'c'){
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
    //configurarPWM7();
    configurarRS232();
    configurarInterrupciones();
    TRISDbits.TRISD4 = 0;
    TXREG1 = 'H';
    
    while(1){
        if(bandera == 1){
            bandera = 0;
            logicaEstadoSiguiente();
        }
        
    }
    return;
}

void logicaEstadoSiguiente(){
    unsigned char texto[20] = {'\0'};
    switch (datos[0]){
        case ADELANTE:
            PORTDbits.RD4 = ~PORTDbits.RD4;
            sprintf(texto, "GIRO ++");
            enviarRS232(texto, 7);
            break;
        case ATRAS:
            sprintf(texto, "GIRO --");
            enviarRS232(texto, 7);
            break;
        case IZQUIERDA:
            
            break;
        case DERECHA:
            
            break;
        case STOP:
            sprintf(texto, "STOP ++");
            enviarRS232(texto, 7);
            break;
        case VELOCIDAD:
            sprintf(texto, "VELOCIDAD");
            enviarRS232(texto, 7);
            break;
    }
}
