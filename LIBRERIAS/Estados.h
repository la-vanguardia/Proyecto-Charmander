#include <stdio.h>
#include <stdlib.h>

//Definicion de los estados de los vehiculos
#define ADELANTE 'w'
#define ATRAS 's'
#define DERECHA 'd'
#define IZQUIERDA 'a'
#define STOP 'c'
#define VELOCIDAD 'F'

void logicaEstadoSiguiente(unsigned char *comand){
    unsigned int medicion = 0;
    unsigned char texto[20] = {'\0'};
    switch (comand[0]){
        case ADELANTE:
            fijarVelocidad(0);
            encenderMotor();
            break;
        case ATRAS:
            fijarVelocidad(50);
            encenderMotor();
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
            medicion = (comand[1] - 0x30)*100 + (comand[2] - 0x30)*10 + comand[3] - 0x30;
            fijarVelocidad(medicion);
            encenderMotor();
            break;
        default:
            break;
    }
}