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
#define LEER_DISTANCIA 0x55 //codigo para leer la distancia del us-100
#define LEER_TEMPERATURA 0x50 //codigo para leer la temperatura del us-100

#define SENSOR_FUEGO PORTBbits.RB0

unsigned int TIME_MAX = 1000-65, parar= 0, tiempo_anterior_1 = 65, contador_timer_5 = 0;
unsigned char indicador = 0,  servo_dirreccion = 0;
unsigned char bandera = 0, bandera_servo = 0 ,obstaculo = 0;
unsigned char datos[10] = {'\0'};
unsigned char bandera_distancia = 0, contador_datos = 0;
unsigned char estadoFuego = 0;
float distancia = 0;

void configuracionInicial();
void terminal(unsigned char *command);
void cambiarPWM();
unsigned int estadoDirreccion(unsigned char valor);
void dirreccion(unsigned int degree);
void PWMServo();
void adelante();
void atras();
void rutinaEscape(unsigned char type);

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
    else if(PIR5bits.TMR5IF == 1){
        PIR5bits.TMR5IF = 0;
        contador_timer_5++;
        TMR5H = 0xFF;
        TMR5L = 0xE2;
        if(contador_timer_5 == TIME_MAX){
            contador_timer_5 = 0;
            bandera_servo = 1;
        }
    }
    else if(INTCONbits.INT0F == 1){
        INTCONbits.INT0F = 0;
        obstaculo = 1;
    }
    else if(PIR3bits.RC2IF == 1){
        if(contador_datos == 1){
            bandera_distancia = 1;
            distancia += (RCREG2)/10;
            contador_datos = 0;
        }
        else{
            distancia = 256*(RCREG2)/10;
            contador_datos ++;
        }
    }
}

void main(void) {
    unsigned char text[] = "HOLA!";
    unsigned char texto[8] = {'\0'};
    configuracionInicial();
    enviarRS232(text);
    while(1){
        if(bandera == 1){
            bandera = 0;
            terminal(datos);
        }
        if(bandera_servo == 1){
            bandera_servo = 0;
            PWMServo();
        }
        if(obstaculo == 1){
            obstaculo = 0;
            rutinaEscape(OBSTACULO);
        }
        if(bandera_distancia == 1){
            bandera_distancia = 0;
            sprintf(texto,"%03.1f cm",distancia);
            enviarRS232(texto);
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
    configurarRS232US100();
}

void terminal(unsigned char *comand){
    unsigned int medicion = 0;
    unsigned int degree;
    unsigned char texto[20] = {'\0'};
    TXREG2 = LEER_DISTANCIA;
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
            degree = estadoDirreccion(2);
            dirreccion(degree);
            break;
        case DERECHA:
            degree = estadoDirreccion(1);
            dirreccion(degree);
            break;
        case STOP:
            frenarMotor();
            break;
        case VELOCIDAD:
            medicion = (comand[1] - 0x30)*100 + (comand[2] - 0x30)*10 + comand[3] - 0x30;
            fijarVelocidad(medicion);
            cambiarPWM();
            sprintf(texto, "Velocidad fijada al: %03u%c", medicion, '%');
            enviarRS232(texto);
            break;
        default:
            break;
    }
}

void cambiarPWM(){
    if(motor == 1){
        definirVelocidad();
    }
}

unsigned int estadoDirreccion(unsigned char valor){
    unsigned int angulo = 90;
    switch(servo_dirreccion){
        case SERVO_CENTRO: 
            if(valor == 1){
                servo_dirreccion = SERVO_DERECHA;
                angulo = 0;
            }
            else{
                servo_dirreccion = SERVO_IZQUIERDA;
                angulo = 180;
            }
            break;
        case SERVO_DERECHA:
            if(valor != 1){
                servo_dirreccion = SERVO_CENTRO;
                angulo = 90;
            }
            else{
                angulo = 0;
            }
            break;
        case SERVO_IZQUIERDA:
            if(valor == 1){
                servo_dirreccion = SERVO_CENTRO;
                angulo = 85;
            }
            else{
                angulo = 180;
            }
            break;
    }
//    if(valor == 1){
//        servo_dirreccion = SERVO_DERECHA;
//        angulo = 0;
//    }
//    else{
//        servo_dirreccion = SERVO_IZQUIERDA;
//        angulo = 180;
//    }
    return angulo;
}

void dirreccion(unsigned int degree){
    unsigned char tiempo_1 = 61;
    contador_timer_5 = 0;
    parar = 0;
    switch(degree){
        case CENTER_2:
            tiempo_1 = 60;
            break;
        case CENTER:
            tiempo_1 = 57;
            break;
        case RIGHT:
            tiempo_1 = 67;
            break;
        case LEFT:
            tiempo_1 = 50;
            break;
    }
    if(SERVO != 1){
        TIME_MAX = tiempo_1;
        SERVO = 1;
    }
    else{
        TIME_MAX = 1000 - tiempo_1;
        SERVO = 0;
    }
    contador_timer_5 = 0;
    bandera_servo = 0;
    T5CONbits.TMR5ON = 1;
}

void PWMServo(){
    contador_timer_5 = 0;
    if(SERVO == 1){
        parar++;
        SERVO = 0;
        tiempo_anterior_1 = TIME_MAX;
        TIME_MAX = 1000 - tiempo_anterior_1;
    }
    else{
        SERVO = 1;
        TIME_MAX = tiempo_anterior_1;   
    }
    if(parar == 300){
        T5CONbits.TMR5ON = 0;
        SERVO = 0;
        parar = 0;
    }
}

void adelante(){
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 0;
    PORTAbits.RA0 = 1;
    PORTAbits.RA1 = 0;
}

void atras(){
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 0;
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 1;
}

void rutinaEscape(unsigned char type){
    frenarMotor();
    switch(type){
        case FUEGO:
            
           
            break;
        case OBSTACULO:
            fijarVelocidad(0);
            atras();
            encenderMotor();
            __delay_ms(1000);
            adelante();
            enviarRS232("Girando");
            fijarVelocidad(50);
            definirVelocidad();
            break;
    }
}

void cambiarEstadoEscapeFuego(){
    switch(estadoFuego){
        case 0: //estado inicial indica deteccion de fuego
            dirreccion(60);
            if(SENSOR_FUEGO == 1){
                dirreccion(120);
                estadoFuego = 1;
            }
            else{
                estadoFuego = 2;
            }
            break;
        case 1: //giro hacia la izquierda
            if(SENSOR_FUEGO == 1){
                estadoFuego = 3;
            }
            else{
                estadoFuego = 4;
            }
            break;
        case 2: //giro hacia la derecha
            dirreccion(45);
            //encender motor tiempo x y doblar
            break;
        case 3: //gire izquierda y no habia fuego
            enviarRS232("No puedo esquivar el fuego! llame a los bomberos");
            //hacer marcha atras y irse a la puta
            break;
        case 4: //gire derecha y no habia fuego
            dirreccion(135);
            //enceder motores y doblar
            break;   
    }
}