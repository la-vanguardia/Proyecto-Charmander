/*
 * File:   tutu.c
 * Authors: Lautaro Saez, Isaias Gatica, Castro Emiliano
 *
 * Created on 14 de junio de 2019, 03:56 PM
 */

#define _XTAL_FREQ 48000000
#include <xc.h>
#include <pic18f47j53.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../LIBRERIAS/CONFIG.h"
#include "../../LIBRERIAS/configuracion_auto.h"
#include "../../LIBRERIAS/funciones_auto.h"

#define ADELANTE 'w'
#define ATRAS 's'
#define DERECHA 'd'
#define IZQUIERDA 'a'
#define STOP 'c'
#define VELOCIDAD 'F'

unsigned char bandera = 0, indicador = 0, datos[10]={0};
unsigned int medicion_adc = 0, bandera_adc = 0;
unsigned int contador_timer_5 = 0, TIME_MAX = 185;
unsigned char obstaculo = 0, bandera_servo = 0;


void terminal(unsigned char *command);
void configuracionInicial();
void configuracionADC();
void dirreccion(unsigned char degree);

void __interrupt() rutina(){
    if(PIR1bits.RC1IF == 1){
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
    else if(PIR1bits.ADIF == 1){
        PIR1bits.ADIF = 0;
        medicion_adc = ADRES;
        bandera_adc = 1;
    }
    else if(PIR5bits.TMR5IF == 1){
        PIR5bits.TMR5IF = 0;
        contador_timer_5++;
        TMR5H = 0xFF;
        TMR5L = 0x6A;
        if(contador_timer_5 == TIME_MAX){
            contador_timer_5 = 0;
            bandera_servo = 1;
        }
    }
    else if(INTCONbits.INT0F == 1){
        INTCONbits.INT0F = 0;
        obstaculo = 1;
    }
}

void main(void){
    unsigned char text_adc[20] = {'\0'}, tiempo_anterior_1 = 15;
    configuracionInicial(); 
    T5CONbits.TMR5ON = 1;
    while(1){
        if(bandera == 1){
            bandera = 0;
            terminal(datos);
        }
        if(bandera_adc == 1){
            bandera_adc = 0;
            sprintf(text_adc, "%04u%c", medicion_adc,0x0D);
            enviarRS232(text_adc, 4);
            __delay_ms(500);
            ADCON0bits.GO_DONE = 1;
        }
        if(bandera_servo == 1){
            bandera_servo = 0;
            
        }
        if(obstaculo == 1){
            obstaculo = 0;
            TXREG1 = 'K';
            frenarMotor();
        }
    }
    return;
}

void configuracionInicial(){
    configurarPuertos();
    configurarPWM7();
    configurarRS232();
    configurarInterrupciones();
    configuracionADC();
    configurarTMR5();
}

void configuracionADC(){
    TRISEbits.TRISE2 = 1;
    ANCON0bits.PCFG7 = 0;
    ADCON0 = 0x1D;
    ADCON1 = 0xB6;
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 0;
}

void terminal(unsigned char *comand){
    unsigned int medicion = 0;
    unsigned char texto[20] = {'\0'};
    switch (comand[0]){
        case ADELANTE:
            
            break;
        case ATRAS:
            
            break;
        case IZQUIERDA:
            dirreccion(180);
            break;
        case DERECHA:
            dirreccion(0);
            break;
        case STOP:
            frenarMotor();
            break;
        case VELOCIDAD:
            medicion = (comand[1] - 0x30)*100 + (comand[2] - 0x30)*10 + comand[3] - 0x30;
            fijarVelocidad(medicion);
            break;
        default:
            break;
    }
}

void dirreccion(unsigned char degree){
    unsigned char tiempo_1 = 15;
    switch(degree){
        case CENTER:
            tiempo_1 = 15;
            break;
        case RIGHT:
            tiempo_1 = 23;
            break;
        case LEFT:
            tiempo_1 = 5;
            break;
    }
    if(SERVO != 1){
        TIME_MAX = tiempo_1;
        SERVO = 1;
    }
    else{
        TIME_MAX = 200 - tiempo_1;
        SERVO = 0;
    }
    contador_timer_5 = 0;
    T5CONbits.TMR5ON = 1;
}