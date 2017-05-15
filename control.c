#include "misc.h"
#include "xc.h"

const float K = 20; //80 gives good response, lower value used during testing to reduce risk
float x_error = 0;  // of damage if something goes wrong.
float y_error = 0;

extern float x_target;
extern float y_target;
extern int capture;
extern int reverse_dir;
//extern int index;
extern int data[300];

/*Set current motor position as centre*/
void control_calib( void ){
    while(!BTN1);
    POS1CNT = 4000;
    POS2CNT=4000;
    reverse_dir = 0;
}

/*Calculate and update motor drive*/
void control_step( void ){
    x_error = x_target - POS2CNT; //Calculate X axis error
    
    float temp = 900 - (float) K*(x_error); //Calculate new duty cycle register value
    // Limit value to valid duty cycle range, out of range values saturate.
    if (temp < 1)
        P1DC1 = 1;
    else if (temp > 1799)
        P1DC1 = 1799;
    else
        P1DC1 = temp;
    
    //Repeat for Y axis
    y_error = y_target - POS1CNT;
    temp = 900 - (float) K*(y_error);
    if (temp < 1)
        P1DC2 = 1;
    else if (temp > 1799)
        P1DC2 = 1799;
    else
        P1DC2 = temp;

//Used for control modelling    
//    if (x_target == 4250)
//        if (index < 3000){
//            data[index] = POS2CNT;
//            index++;
//        }
    
}
//Limit target values to turret maximum range.
//  if values were out of range. (Not an error in this function)
//  but may indicate error in calculating target
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