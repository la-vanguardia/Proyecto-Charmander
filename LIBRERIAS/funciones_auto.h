#include <stdio.h>
#include <stdlib.h>

#define CENTER 90
#define RIGHT 0
#define LEFT 180
#define SERVO PORTDbits.RD4
#define TOTAL_CICLE 200

unsigned int cicle_90 = 0x01C2;
unsigned int velocidad = 0x01C2;
unsigned char motor = 0;

void rutinaArranque();
void fijarVelocidad(unsigned char speed);
void encenderMotor();
void definirVelocidad();
void frenarMotor();
void enviarRS232(unsigned char *valores);
int length(unsigned char *text);

void rutinaArranque(){
    CCP7CONbits.DC7B = cicle_90 & 0x0003;
    CCPR7L = cicle_90>>2;
    T2CONbits.TMR2ON = 1;
    __delay_ms(50);
}

void fijarVelocidad(unsigned char speed){
    float DC = 0.006 * speed + 0.6; //DC - CCPR / (4*(PR2+1))
    velocidad = 4 * DC * 125;
}

void encenderMotor(){
    rutinaArranque();
    definirVelocidad();
    motor = 1;
}

void definirVelocidad(){
    CCP7CONbits.DC7B = velocidad & 0x0003;
    CCPR7L = velocidad>>2;
}

void frenarMotor(){
    CCP7CONbits.DC7B = 0b00;
    CCPR7L = 0x00;
    motor = 0;
}

void enviarRS232(unsigned char *valores){
    unsigned int longitud_texto = length(valores);
    for(unsigned char i = 0; i<longitud_texto; i++){
        TXREG1 = valores[i];
        __delay_ms(3);
    }
}

int length(unsigned char *text){
    unsigned char dato = text[0], i = 1;
    int longitud = 0;
    while(dato != '\0'){
        dato = text[i];
        i++;
        longitud++;
    }
    return longitud;
}


