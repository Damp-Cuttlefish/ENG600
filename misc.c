#ifndef __33FJ128MC802_H
#include "xc.h"
#endif
#include "misc.h"



void error( unsigned char err){
    T1CONbits.TON = 0;          //Stop interrupt timer
    P1DC1 = 900;    //PWM1 Generator 1 Duty Cycle 50%
    P1DC2 = 900;    //PWM1 Generator 2 Duty Cycle 50%
    LEDPin = 0;
    unsigned char i;
    unsigned char j;
    while(1){
        for(i=0; i<err; i++){
            for(j=0; j<100; j++){
                LEDPin = 0;
                __delay32(1000);
                LEDPin = 1;
                __delay32(1000);
            }
            LEDPin = 1;
            __delay32(500000);
        }
        __delay32(1000000);
    }
}

void alert( unsigned char err){
    LEDPin = 0;
    unsigned char i;
    unsigned char j;
    for(i=0; i<err; i++){
        for(j=0; j<100; j++){
            LEDPin = 0;
            __delay32(1000);
            LEDPin = 1;
            __delay32(1000);
        }
        LEDPin = 1;
        __delay32(500000);
    }
    __delay32(1000000);
    
}

/***************************************************
 * Error Codes
 * 1    Unused
 * 2    Bus collision
 * 3    Reciever overflow
 * 4    Target out of range
 * 5    Motor out of range
 * 6    NAck recieved from Pixart
 * 7
 * 8
 * 9
 * 10   
 * 11
 * 12
 * 13
 * 14
 * 15
 * 16
 * 17
 * 18
 * 19
 * 20
 */