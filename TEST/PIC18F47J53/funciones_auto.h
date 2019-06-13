
//Definicion de los estados de los vehiculos
#define ADELANTE 'w'
#define ATRAS 's'
#define DERECHA 'd'
#define IZQUIERDA 'a'
#define STOP ' '
#define VELOCIDAD 'F'

unsigned int cicle_90 = 0x01C2;
unsigned int velocidad = 0;

void rutinaArranque(){
    CCP7CONbits.DC7B = cicle_90 & 0x0003;
    CCPR7L = cicle_90>>2;
    T2CONbits.TMR2ON = 1;
    __delay_ms(50);
}

void fijarVelocidad(unsigned char speed){
    float DC = 0.003 * speed + 0.6; //DC - CCPR / (4*(PR2+1))
    velocidad = 4 * DC * 125;
}



void enviarRS232(unsigned char *valores, unsigned char numero_valores){
    for(unsigned char i = 0; i<numero_valores; i++){
        TXREG1 = valores[i];
        __delay_ms(10);
    }
}


