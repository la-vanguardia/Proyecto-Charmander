/*
 * File:   Us-100.c
 * Author: Lautaro Saez
 *
 * Created on 22 de junio de 2019, 10:16 PM
 */



#define _XTAL_FREQ 48000000
#include <xc.h>
#include <pic18f47j53.h>
#include "../../LIBRERIAS/CONFIG.h"
#include "../../LIBRERIAS/configuracion_auto.h"
#include "../../LIBRERIAS/funciones_auto.h"

#define TRIGGER PORTBbits.RB1


unsigned char bandera_ultra_sonico = 0, time_pulse = 0, contador = 0, bandera = 0, indicador = 0;
unsigned char datos[10] = {'\0'};
float distancia;
unsigned char texto[6] = {'\0'};

void configuracionInicial();

void __interrupt() rutina(){
    if(INTCONbits.INT0F == 1){
        INTCONbits.INT0F = 0;
        time_pulse = contador * 5;
        contador = 0;
        bandera_ultra_sonico = 1;
    }
    else if(PIR1bits.RC1IF == 1){
        unsigned char dato = RCREG1;
        if(dato == 'P'){
            bandera = 1;
            indicador = 0;
        }
        else{
            TXREG1 = dato;
            datos[indicador] = dato;
            indicador++;
        }
    }
    else if(PIR5bits.TMR5IF == 1){
        PIR5bits.TMR5IF = 0;
        TMR5H = 0xFF;
        TMR5L = 0x6A;
        contador ++;
    }
}

void main(void) {
    configuracionInicial();
    TRISBbits.TRISB1 = 0;
    TXREG1 = 'H';
    while(1){
        if(bandera == 1){
            bandera = 0;
            if(datos[0] == 'w'){
                TRIGGER = 0;
                __delay_us(2);
                TRIGGER = 1;
                __delay_us(10);
                TRIGGER = 0;
                contador = 0;
            }
            
            if(bandera_ultra_sonico == 1){
                bandera_ultra_sonico = 0;
                distancia = time_pulse / 29.1;
                sprintf(texto, "%3.2f cm",distancia);
                enviarRS232(texto);
            }
        }
    }
    return;
}


void configuracionInicial(){
    configurarPuertos();
    configurarInterrupciones();
    configurarRS232();
    configurarTMR5();
}