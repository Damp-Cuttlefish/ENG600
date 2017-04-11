/*
 * File:   newmainXC16.c
 * Author: tom
 *
 * Created on January 20, 2017, 1:50 PM
 */

#include "xc.h"
#include <math.h>
#include "i2c.h"
#include "init.h"
#include "misc.h"
#include "pixart.h"
#include "control.h"

/*--------------Definitions--------------*/
const int sensor_width = 100;

/*--------------Prototypes--------------*/

void control_step( void );
void ident(void);
void servoFollow( void );
/*--------------Global Variables--------------*/

float x_target_raw = 0;
float y_target_raw = 0;

char bsize = 0;


float x_target = 4000;
float y_target = 4000;

int capture = 0;

struct Space c_space;
struct Camera LCam;
struct Camera RCam;

int data[3000];
int index = 0;


char control_tone = 0;
char blobs = 0;

int main(void) 
{
    
    
    
    init_setup();    
    /************************Debug************************/
        //T1CONbits.TON = 0;          //Stop timer


    /************************Debug************************/
    LRPin = 0;
    __delay32(100000);    
    sensor_rst();
    __delay32(10000);
    sensor_init();
    __delay32(10000);
    
    
    Motor_Enable = 1;
    while(1){
        sensor_rst();
        __delay32(100);

        sensor_init();
        __delay32(50000);
        
        LEDPin = 0;
        blobs = sensor_read1(&LCam);
        LEDPin = 1;     
        
        ident();
          
        
        if((LCam.P1.state && LCam.P2.state)){
            x_target_raw = 4000 + 79.6*atanf(((float) LCam.P1.x - (float) LCam.P2.x) / ((float) LCam.P1.y - (float) LCam.P2.y));
            control_tone = 0;
        }
        else{
            x_target_raw = 4000;
            control_tone = 1;
        }
//        if(LCam.P1.state){
//            x_target_raw = 3750 + 0.5*LCam.P1.x;
//            y_target_raw = 3875 + 0.25*LCam.P1.y;
//        }
        control_limit(&x_target_raw, &y_target_raw);
        x_target = x_target_raw;
        y_target = y_target_raw;
    }
    
    
    //sensor_init();
    x_target = 4000;
    y_target = 4000;
    float acount = 0;
    char temp = 0;
    __delay32(5000000);
               //Enable motors
    
//    while(1){
//        while(BTN1);
//        index = 0;
//        __delay32(500000);
//        step();
//    }
//    
//    while(1)
//    {
//        acount += 0.1;
//        if (acount > 6.28)
//            acount = 0;
//        
//        x_target = 4000 + 100 * sinf(acount);
//        y_target = 4000 + 100 * cosf(acount);
//        if (control_limit(&x_target, &y_target))
//            alert(4);
//        __delay32(50000);
//
//    }
    
    return 0;
}

void step( void ){
    
    x_target = 4250;
    __delay32(5000000);
    x_target = 4250;
    __delay32(50000);
    __delay32(5000000);
}




void ident(void){
    int p1y = 0;
    int p2y = 0;
    int p1 = 0;
    int p2 = 0;
    
    
    if(blobs && BLOB2){
        if ((LCam.S1.x > 0) && (LCam.S1.x < 1023)){
            if (LCam.S1.y > p1y){
                p1y = LCam.S1.y;
                p1 = 1;                
            }
        }
    }
    if(blobs && BLOB2){
        if ((LCam.S2.x > 0) && (LCam.S2.x < 1023)){
            
            if (LCam.S2.y > p1y){
                p2y = p1y;
                p2 = p1;
                p1y = LCam.S2.y;
                p1 = 2;
            }
            else if(LCam.S2.y > p2y){
                p2y = LCam.S2.y;
                p2 = 2;
            }
        }
    }
    if(blobs && BLOB3){
        if ((LCam.S3.x > 0) && (LCam.S3.x < 1023)){
            if (LCam.S3.y > p1y){
                p2y = p1y;
                p2 = p1;
                p1y = LCam.S3.y;
                p1 = 3;
            }
            else if(LCam.S3.y > p2y){
                p2y = LCam.S3.y;
                p2 = 3;
            }
        }
    }
    if(blobs && BLOB4){
        if ((LCam.S4.x > 0) && (LCam.S4.x < 1023)){
            if (LCam.S4.y > p1y){
                p2y = p1y;
                p2 = p1;
                p1y = LCam.S4.y;
                p1 = 4;
            }
            else if(LCam.S4.y > p2y){
                p2y = LCam.S4.y;
                p2 = 4;
            }
        }
    }
    switch (p1){
            case 1:
                LCam.P1.x = LCam.S1.x;
                LCam.P1.y = LCam.S1.y;
                LCam.P1.state = 1;
                break;
            case 2:
                LCam.P1.x = LCam.S2.x;
                LCam.P1.y = LCam.S2.y;
                LCam.P1.state = 1;
                break;
            case 3:
                LCam.P1.x = LCam.S3.x;
                LCam.P1.y = LCam.S3.y;
                LCam.P1.state = 1;
                break;
            case 4:
                LCam.P1.x = LCam.S4.x;
                LCam.P1.y = LCam.S4.y;
                LCam.P1.state = 1;    
                break;
            case 0:
                LCam.P1.x = 0;
                LCam.P1.y = 0;
                LCam.P1.state = 0;
                break;
    }
    
    switch (p2){
            case 1:
                LCam.P2.x = LCam.S1.x;
                LCam.P2.y = LCam.S1.y;
                LCam.P2.state = 1;
                break;
            case 2:
                LCam.P2.x = LCam.S2.x;
                LCam.P2.y = LCam.S2.y;
                LCam.P2.state = 1;
                break;
            case 3:
                LCam.P2.x = LCam.S3.x;
                LCam.P2.y = LCam.S3.y;
                LCam.P2.state = 1;
                break;
            case 4:
                LCam.P2.x = LCam.S4.x;
                LCam.P2.y = LCam.S4.y;
                LCam.P2.state = 1;
                break;
            case 0:
                LCam.P2.x = 0;
                LCam.P2.y = 0;
                LCam.P2.state = 0;
                break;
    }
    
    
}

int poscalc(struct Camera LCam, struct Camera Rcam){
    return 0;
}

int anglecalc(struct Camera LCam, struct Camera Rcam){
    float panAngle;
    float tiltAngle;
    
    struct Coord long_vector;
    
    long_vector.x = c_space.P1.x - c_space.P3.x;
    long_vector.y = c_space.P1.y - c_space.P3.y;
    long_vector.z = c_space.P1.z - c_space.P3.z;
    
    panAngle = atan2f(long_vector.y, long_vector.x);
    tiltAngle = atan2f(sqrtf(powf(long_vector.y, (float) 2)+powf(long_vector.x, (float) 2)), long_vector.z);
    
    
    
    /*lat_vector.x = c_space.P.x - c_space.P3.x;
    lat_vector.y = c_space.P1.y - c_space.P3.y;
    lat_vector.z = c_space.P1.z - c_space.P3.z;*/
    return 0;
}


void __attribute__((interrupt, auto_psv)) _T1Interrupt( void ){ //Function name auto fills vector table to match
    /* Envelope
     * Centre 4000
     * XMax = 4500, XMin = 3500
     * XMax = 4250, XMin = 3250
     */
    IFS0bits.T1IF = 0; //Clear interrupt flag
    if (control_tone)
        LEDPin = ~LEDPin;
    control_step();
}

