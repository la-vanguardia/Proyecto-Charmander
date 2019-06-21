/*
 * File:   auto.c
 * Author: Lautaro Saez
 *
 * Created on 18 de junio de 2019, 01:12 PM
 */

#define _XTAL_FREQ 48000000
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic18f47j53.h>
#include <math.h>

#include "../LIBRERIAS/CONFIG.h"
#include "../LIBRERIAS/configuracion_auto.h"
#include "../LIBRERIAS/funciones_auto.h"

#define ADELANTE 'w'
#define ATRAS 's'
#define DERECHA 'd'
#define IZQUIERDA 'a'
#define STOP 'c'
#define VELOCIDAD 'F'
#define FUEGO 1
#define OBSTACULO 2
#define SERVO_CENTRO 0
#define SERVO_DERECHA 1
#define SERVO_IZQUIERDA 2

unsigned int TIME_MAX = 185;
unsigned int medicion_adc = 0;
unsigned char tiempo_anterior_1 = 15, indicador = 0, contador_timer_5 = 0, servo_dirreccion = 0;
unsigned char bandera = 0, bandera_adc = 0, bandera_servo = 0 ,obstaculo = 0;
unsigned char datos[10] = {'\0'};

void configuracionInicial();
void terminal(unsigned char *command);
unsigned char estadoDirreccion(unsigned char valor);
void dirreccion(unsigned char degree);
void PWMServo();
void adelante();
void atras();

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

void main(void) {
    unsigned char text[] = "HOLA!";
    configuracionInicial();
    enviarRS232(text);
    float caca = sin(36.05465);
    while(1){
        if(bandera == 1){
            bandera = 0;
            terminal(datos);
        }
        if(bandera_servo == 1){
            bandera_servo = 0;
            PWMServo();
        }
        if(bandera_adc == 1){
            bandera_adc = 0;
            
        }
        if(obstaculo == 1){
            obstaculo = 0;
        }
        
    }
    return;
}

void configuracionInicial(){
    configurarPuertos();
    configurarPWM7();
    configurarInterrupciones();
    configurarRS232();
    configurarTMR5();
}

void terminal(unsigned char *comand){
    unsigned int medicion = 0;
    unsigned char degree;
    unsigned char texto[20] = {'\0'};
    switch (comand[0]){
        case ADELANTE:
            adelante();
            encenderMotor();
            enviarRS232("Motores encendidos!");
            break;
        case ATRAS:
            atras();
            encenderMotor();
            enviarRS232("Vehiculo en reversa!");
            break;
        case IZQUIERDA:
            degree = estadoDirreccion(1);
            dirreccion(degree);
            break;
        case DERECHA:
            degree = estadoDirreccion(0);
            dirreccion(degree);
            break;
        case STOP:
            frenarMotor();
            break;
        case VELOCIDAD:
            medicion = (comand[1] - 0x30)*100 + (comand[2] - 0x30)*10 + comand[3] - 0x30;
            fijarVelocidad(medicion);
            sprintf(texto, "Velocidad fijada al: %03u%c", medicion, '%');
            enviarRS232(texto);
            break;
        default:
            break;
    }
}

unsigned char estadoDirreccion(unsigned char valor){
    unsigned char angulo = 0;
    switch(servo_dirreccion){
        case SERVO_CENTRO: 
            if(valor == 1){
                servo_dirreccion = SERVO_DERECHA;
                angulo = 180;
            }
            else{
                servo_dirreccion = SERVO_DERECHA;
                angulo = 180;
            }
            break;
        case SERVO_DERECHA:
            if(valor != 1){
                servo_dirreccion = SERVO_CENTRO;
                angulo = 90;
            }
            break;
        case SERVO_IZQUIERDA:
            if(valor == 1){
                servo_dirreccion = SERVO_CENTRO;
                angulo = 90;
            }
            break;
    }
    return angulo;
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

void PWMServo(){
    if(SERVO == 1){
        SERVO = 0;
        tiempo_anterior_1 = TIME_MAX;
        TIME_MAX = 200 - tiempo_anterior_1;
    }
    else{
        SERVO = 1;
        TIME_MAX = tiempo_anterior_1;   
    }
}

void adelante(){
    enviarRS232("FALTA IMPLEMENTAR!");
}

void atras(){
    enviarRS232("FALTA IMPLEMETAR!");
}

void rutinaEscape(unsigned char type){
    frenarMotor();
    unsigned int medicion_1 = 0, medicion_2 = 0;
    switch(type){
        case FUEGO:
            ADCON0bits.GO_DONE = 1;
            while(ADCON0bits.GO_DONE);
            bandera_adc = 0;
            medicion_1 = medicion_adc;
            ADCON0bits.GO_DONE = 1;
            while(ADCON0bits.GO_DONE);
            bandera_adc = 0;
            if(medicion_1 > medicion_2){
                fijarVelocidad(100);
            }
            else{
                fijarVelocidad(50);
            }
            break;
        case OBSTACULO:
            fijarVelocidad(80);
            break;
    }
    atras();
    encenderMotor();
}