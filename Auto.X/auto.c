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
#define MODO_PROGRAMACION 'g'
#define FUEGO 1
#define OBSTACULO 2
#define ULTRASONIDO 3
#define SERVO_CENTRO 0
#define SERVO_DERECHA 1
#define SERVO_IZQUIERDA 2
#define GIRO 0
#define LEER_DISTANCIA 0x55 //codigo para leer la distancia del us-100
#define LEER_TEMPERATURA 0x50 //codigo para leer la temperatura del us-100

#define SENSOR_OBSTACULOS PORTBbits.RB0
#define SENSOR_FUEGO PORTBbits.RB1

unsigned int TIME_MAX = 1000-65, parar= 0, tiempo_anterior_1 = 65, contador_timer_5 = 0;
unsigned int contador_distancia = 0;
unsigned char indicador = 0, bandera_fuego = 0,  servo_dirreccion = 0;
unsigned char bandera = 0, bandera_servo = 0 ,obstaculo = 0;
unsigned char datos[10] = {'\0'};
unsigned char bandera_distancia = 0, contador_datos = 0;
unsigned char estadoFuego = 0, estado = 0, cambiar_estado_rutina_escape = 0;
float distancia = 0;
long unsigned int fuego_interrupcion = 0, TIME_FUEGO = 0;

void configuracionInicial();
void terminal(unsigned char *command);
void cambiarPWM();
unsigned int estadoDirreccion(unsigned char valor);
void dirreccion(unsigned int degree);
void PWMServo();
void adelante();
void atras();
void rutinaEscape(unsigned char type);
void cambiarEstadoFuego();

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
    else if(PIR3bits.TMR4IF == 1){
        PIR3bits.TMR4IF = 0;
        contador_distancia++;
        TMR4 = 181;
        if(contador_distancia == 1000){
            contador_distancia = 0;
            if(estado == ADELANTE){
                TXREG2 = LEER_DISTANCIA;
            }
        }
    }
    else if(PIR5bits.TMR8IF == 1){
        TMR8 = 31;
        PIR5bits.TMR8IF = 0;
        fuego_interrupcion++;
        if(fuego_interrupcion == TIME_FUEGO ){
            fuego_interrupcion = 0;
            cambiar_estado_rutina_escape = 1;
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
    else if(INTCON3bits.INT1F == 1 && INTCON3bits.INT1E == 1){
        INTCON3bits.INT1F = 0;
        if (estado == ADELANTE){
            INTCON3bits.INT1E = 0;
            bandera_fuego = 1; 
        }
    }
}

void main(void) {
    configuracionInicial();
    enviarRS232("HOLA!...");
    __delay_ms(1800);
    enviarRS232("Mi nombre es Charmander");
    __delay_ms(1800);
    enviarRS232("No me destruyas! :'(");
    __delay_ms(1500);
    enviarRS232("Por Favor, calibre el sensor");
    T4CONbits.TMR4ON = 1;
    while(1){
        if(estado == ADELANTE){
            if(bandera_distancia == 1){
                bandera_distancia = 0;     
                if((distancia <= 22)){
                    rutinaEscape(ULTRASONIDO);
                    enviarRS232("ULTRASONIDO ACTIVADO!");
                }
            }
            if(bandera_fuego == 1){
                bandera_fuego = 0;
                fuego_interrupcion = 0;
                rutinaEscape(FUEGO);
                enviarRS232("ESCAPANDO DEL FUEGO...");
            }
            if(cambiar_estado_rutina_escape == 1){
                T8CONbits.TMR8ON = 0;
                fuego_interrupcion = 0;
                cambiar_estado_rutina_escape = 0;
                cambiarEstadoFuego();
            }
        }
        if(estado == ATRAS){
            if(obstaculo == 1){
                obstaculo = 0;
                rutinaEscape(OBSTACULO);
            }    
            if(cambiar_estado_rutina_escape == 1){
                T8CONbits.TMR8ON = 0;
                fuego_interrupcion = 0;
                cambiar_estado_rutina_escape = 0;
                estado = STOP;
                frenarMotor();
            }
        }
        if(bandera == 1){
            bandera = 0;
            terminal(datos);
        }
        if(bandera_servo == 1){
            bandera_servo = 0;
            PWMServo();
        }
        
    }
    return;
}

void configuracionInicial(){
    configurarPuertos();
    configurarPWM7();
    configurarInterrupciones();
    configurarRS232();
    configurarTMR4();
    configurarTMR5();
    configurarTMR8();
    configurarRS232US100();
}

void terminal(unsigned char *comand){
    unsigned int medicion = 0;
    unsigned int degree;
    unsigned char texto[20] = {'\0'};
    estado = STOP;
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
            estado = STOP;
            frenarMotor();
            INTCON3bits.INT1E = 1;
            break;
        case VELOCIDAD:
            medicion = (comand[1] - 0x30)*100 + (comand[2] - 0x30)*10 + comand[3] - 0x30;
            fijarVelocidad(medicion);
            cambiarPWM();
            break;
        case MODO_PROGRAMACION:
            enviarRS232("MODO PROGRAMADOR ACTIVADO!");
            while(1);
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
    return angulo;
}

void dirreccion(unsigned int degree){
    unsigned char tiempo_1 = 61;
    contador_timer_5 = 0;
    parar = 0;
    switch(degree){
        case CENTER_2:
            tiempo_1 = 57;
            servo_dirreccion = SERVO_CENTRO;
            break;
        case CENTER:
            tiempo_1 = 57;
            servo_dirreccion = SERVO_CENTRO;
            break;
        case RIGHT:
            tiempo_1 = 65;
            servo_dirreccion = SERVO_DERECHA;
            break;
        case LEFT:
            tiempo_1 = 50;
            servo_dirreccion = SERVO_IZQUIERDA;
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
    if(parar == 30){
        T5CONbits.TMR5ON = 0;
        SERVO = 0;
        parar = 0;
    }
}

void adelante(){
    estado = ADELANTE;
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 0;
    PORTAbits.RA0 = 1;
    PORTAbits.RA1 = 0;
}

void atras(){
    estado = ATRAS;
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 0;
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 1;
}

void rutinaEscape(unsigned char type){
    frenarMotor();
    estado = STOP;
    switch(type){
        case FUEGO:
            enviarRS232("FUEGO!");
            dirreccion(180);
            fijarVelocidad(10);
            adelante();
            encenderMotor();
            TIME_FUEGO = 4000;
            T8CONbits.TMR8ON = 1;
            estadoFuego = GIRO;
            break;
        case ULTRASONIDO:
            TIME_FUEGO = 4000;
            atras();
            fijarVelocidad(10);
            encenderMotor();
            T8CONbits.TMR8ON = 1;
            break;
        case OBSTACULO:
            frenarMotor();
            break;
    }
}

void cambiarEstadoFuego(){
    unsigned int angulo = 0;
    bandera_fuego = 0;
    switch(estadoFuego){
        case GIRO:
            TIME_FUEGO = 8000;
            angulo = estadoDirreccion(1);
            dirreccion(angulo);
            estadoFuego = ADELANTE;
            T8CONbits.TMR8ON = 1;
            break;
        case ADELANTE:
            TIME_FUEGO = 6000;
            dirreccion(0);
            estadoFuego = IZQUIERDA;
            T8CONbits.TMR8ON = 1;
            break;
        case IZQUIERDA:
            estado = STOP;
            frenarMotor();
            dirreccion(90);
            break;
    }
}