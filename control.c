#include "misc.h"
#include "xc.h"

const float K = 20;
float x_error = 0;
float y_error = 0;

extern float x_target;
extern float y_target;
extern int capture;
//extern int index;
extern int data[300];

void control_step( void ){
    
    
    x_error = x_target - POS2CNT;
    float temp = 900 - (float) K*(x_error);
    if (temp < 1)
        P1DC1 = 1;
    else if (temp > 1799)
        P1DC1 = 1799;
    else
        P1DC1 = temp;
    
    
    y_error = y_target - POS1CNT;
    temp = 900 - (float) K*(y_error);
    if (temp < 1)
        P1DC2 = 1;
    else if (temp > 1799)
        P1DC2 = 1799;
    else
        P1DC2 = temp;
    
//    if (x_target == 4250)
//        if (index < 3000){
//            data[index] = POS2CNT;
//            index++;
//        }
    
}

char control_limit(float *x, float *y){
    char err = 0;
    
    if (*x > 4500){
        *x = 4500;
        err++;           
    }
    else if (*x < 3500){
        *x = 3500;
        err++;
    }
    if (*y > 4250){
        *y = 4250;
        err++;
    }
    else if (*y < 3750){
        *y = 3750;
        err++;
    }
    return err;
}