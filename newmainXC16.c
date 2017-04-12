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
const float x_offset_l = 1.858775653;   // Inside angle between x = 0 and sensor mount 
const float x_offset_r = 1.282817 ;      // Inside angle between x = 0 and sensor mount 
const float y_offset_l = 0.200712864;   //Angle between y=0 and vertical
const float y_offset_r = 0.200712864;   //Angle between y=0 and vertical
const float pi = 3.14159265359;
/*--------------Prototypes--------------*/

void control_step( void );
char ident( struct Camera *Cam );
void servoFollow( void );
void switcheroo(int, int, struct Camera *Cam);
void poscalc( void );

/*--------------Global Variables--------------*/
float panAngle;
float tiltAngle;


float x_target_raw = 4000;
float y_target_raw = 4000;

char bsize = 0;

float prevangle = 0;

float x_target = 4000;
float y_target = 4000;

int capture = 0;

struct Space c_space;
struct Camera LCam;
struct Camera RCam;

int data[3000];
//int index = 0;


char control_tone = 0;
char blobs = 0;

int main(void) 
{
    
    float old_x=0;
    float old_y=0;
    
    init_setup();    
    /************************Debug************************/
        //T1CONbits.TON = 0;          //Stop timer


    /************************Debug************************/
    
    
    
    char err = 0;
    
    Motor_Enable = 1;
    while(1){
        err = 0;
        LRPin = 0;
        sensor_rst();
        __delay32(100);
        sensor_init();
        __delay32(500);
        //LEDPin = 0;
        blobs = sensor_read1(&LCam);
        err = ident(&LCam); 
        
        LRPin = 1;
        sensor_rst();
        __delay32(100);
        sensor_init();
        __delay32(500);
        //LEDPin = 0;
        blobs = sensor_read1(&RCam);
        err |= ident(&RCam);
        
        poscalc();
        anglecalc();
        
//        if((LCam.P1.state && LCam.P2.state)){
//            x_target_raw = 4000 - 159*atanf(((float) LCam.S1.x - (float) LCam.S2.x) / ((float) LCam.S1.y - (float) LCam.S2.y));
//            control_tone = 0;
//        }
        if(err)
            control_tone = 1;
        else{
            x_target_raw = 4000-159*panAngle;
            y_target_raw = 3750+159*tiltAngle;
            control_tone = 0;
        }

        control_limit(&x_target_raw, &y_target_raw);
        
        x_target = (x_target_raw + x_target )/2;
        y_target = (y_target_raw + y_target )/2;;
    }
    
    
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




char ident( struct Camera *Cam ){
    switch (blobs) {
        case 0:
            return 1;
        case 1:
            return 1; 
        case 2:
            return 1;
        case 4:
            return 1;
        case 8: 
            return 1;
    }
    int p1y = 0;
    int p2y = 2000;
    int p1 = 0;
    int p2 = 0;
    prevangle = atanf(((float) Cam->P1.x - (float) Cam->P2.x) / ((float) Cam->P1.y - (float) Cam->P2.y)); //-pi => +pi
    
    if(blobs && BLOB1){
        if ((Cam->S1.x > 0) && (Cam->S1.x < 1023)){
            if (Cam->S1.y > p1y){
                p1y = Cam->S1.y;
                p1 = 1;                
            }
            if (Cam->S1.y < p2y){
                p2y = Cam->S1.y;
                p2 = 1;                
            }
        }
    }
    if(blobs && BLOB2){
        if ((Cam->S2.x > 0) && (Cam->S2.x < 1023)){
            
            if (Cam->S2.y > p1y){
                p1y = Cam->S2.y;
                p1 = 2;                
            }
            else if (Cam->S2.y < p2y){
                p2y = Cam->S2.y;
                p2 = 2;                
            }
        }
    }
    if(blobs && BLOB3){
        if ((Cam->S3.x > 0) && (Cam->S3.x < 1023)){
            if (Cam->S3.y > p1y){
                p1y = Cam->S3.y;
                p1 = 3;                
            }
            else if (Cam->S3.y < p2y){
                p2y = Cam->S3.y;
                p2 = 3;                
            }
        }    }
    if(blobs && BLOB4){
        if ((Cam->S4.x > 0) && (Cam->S4.x < 1023)){
            if (Cam->S4.y > p1y){
                p1y = Cam->S4.y;
                p1 = 4;                
            }
            else if (Cam->S1.y < p2y){
                p2y = Cam->S1.y;
                p2 = 4;                
            }
        }
    }
    
    if (p1 && p2)
        if(p1 == p2)
            error(10);
    switcheroo(p1,p2, Cam);
    
    return 0;
}

void switcheroo(int p1, int p2, struct Camera *Cam){
    switch (p1){
            case 1:
                Cam->P1.x = Cam->S1.x;
                Cam->P1.y = Cam->S1.y;
                Cam->P1.state = 1;
                break;
            case 2:
                Cam->P1.x = Cam->S2.x;
                Cam->P1.y = Cam->S2.y;
                Cam->P1.state = 1;
                break;
            case 3:
                Cam->P1.x = Cam->S3.x;
                Cam->P1.y = Cam->S3.y;
                Cam->P1.state = 1;
                break;
            case 4:
                Cam->P1.x = Cam->S4.x;
                Cam->P1.y = Cam->S4.y;
                Cam->P1.state = 1;    
                break;
            case 0:
                Cam->P1.x = 0;
                Cam->P1.y = 0;
                Cam->P1.state = 0;
                break;
    }
    
    switch (p2){
            case 1:
                Cam->P2.x = Cam->S1.x;
                Cam->P2.y = Cam->S1.y;
                Cam->P2.state = 1;
                break;
            case 2:
                Cam->P2.x = Cam->S2.x;
                Cam->P2.y = Cam->S2.y;
                Cam->P2.state = 1;
                break;
            case 3:
                Cam->P2.x = Cam->S3.x;
                Cam->P2.y = Cam->S3.y;
                Cam->P2.state = 1;
                break;
            case 4:
                Cam->P2.x = Cam->S4.x;
                Cam->P2.y = Cam->S4.y;
                Cam->P2.state = 1;
                break;
            case 0:
                Cam->P2.x = 0;
                Cam->P2.y = 0;
                Cam->P2.state = 0;
                break;
    }
}

void poscalc( void ){
    //33h, 23v. 1024x768
    //0.5759586 rad h
    //0.401425728 rad v
    //x res =  5.624596222x10^-4
    //y res = 5.226897499x10^-4
    const float x_res = 0.0005624596222;
    const float y_res = 0.0005226897499;
    
    float A, B, C; //Sensor mount, LCam, RCam
    float a, b, c; //Helm, RCam, LCam
    float y;
    //p1
    c = x_offset_l - LCam.P1.x*x_res;
    b = x_offset_r + RCam.P1.x*x_res; 
    y = LCam.P1.y*y_res - y_offset_l; 
    
    A = sensor_width;
    
    B = (A*sinf(c))/sin(pi-(b + c));
    c_space.P1.z = B*sinf(c);
    c_space.P1.x = B*cosf(c);
    c_space.P1.y = tanf(y)*c_space.P1.z;
    
    //p2
    c = x_offset_l - LCam.P2.x*x_res;
    b = x_offset_r + RCam.P2.x*x_res; 
    y = LCam.P2.y*y_res - y_offset_l; 
    
    A = sensor_width;
    
    B = (A*sinf(c))/sin(pi-(b + c));
    c_space.P2.z = B*sinf(c);
    c_space.P2.x = B*cosf(c);
    c_space.P2.y = tanf(y)*c_space.P2.z;
    
    
        
    
    
}

int anglecalc(struct Camera LCam, struct Camera Rcam){
    struct Coord long_vector;
    
    long_vector.x = c_space.P1.x - c_space.P2.x;
    long_vector.y = c_space.P1.y - c_space.P2.y;
    long_vector.z = c_space.P1.z - c_space.P2.z;
    
    //if(long_vector.y < 0)
      //  error(4);
    panAngle = atanf(long_vector.x / long_vector.y);
    //panAngle = long_vector.x;
    tiltAngle = atan2f(sqrtf(powf(long_vector.y, (float) 2)+ powf(long_vector.x, (float) 2)), long_vector.z);
    //tiltAngle = long_vector.y;
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

