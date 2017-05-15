/*This file contains UNIX line endings only, and will not display properly in MS Notepad*/

#include "xc.h"			//Header file to automatically include correct device libraries
#include <math.h>		//Standard maths library11
#include "i2c.h"		//Library containing useful i2c functions
#include "init.h"		//Library containing initialisation/setup code
#include "misc.h"		//Library containing miscelaneous useful functions and definitions
#include "pixart.h"		//Library containing functions for camera interfacing
#include "control.h"	//Library containing motor control related functions

/*--------------Definitions--------------*/
const int sensor_width = 100;
const float x_offset_l = 1.858775653;   // Inside angle between x = 0 and sensor mount 
const float x_offset_r = 1.282817 ;      // Inside angle between x = 0 and sensor mount 
const float y_offset_l = 0.200712864;   //Angle between y=0 and vertical
const float y_offset_r = 0.200712864;   //Angle between y=0 and vertical
const float x_res = 0.0005624596222;    //Camera resolution in radians per pixel
const float y_res = 0.0005226897499;    // ----^
const float pi = 3.14159265359;
/*--------------Prototypes--------------*/

void control_step( void );
char ident( struct Camera *Cam );
void servoFollow( void );
void switcheroo(int, int, struct Camera *Cam);
void poscalc( void );
void anglecalc( void );

/*--------------Global Variables--------------*/
float pan_angle;
float tilt_angle;


float x_target_raw = 4000;
float y_target_raw = 4000;

char bsize = 0;//???

float prevAngle = 0;

float x_target = 4000;	//The target X axis position used by the motor controller
float y_target = 4000;	//The target Y axis position used by the motor controller

//Temporary variables used for control modelling
//int capture = 0;	
//int data[3000];
//int index = 0;

int reverse_dir = 0; 

//All structures are defined in "misc.h"
struct Camera LCam;		//Structure containing data relevant to the left camera
struct Camera RCam;		//Structure containing data relevant to the right camera
struct Space c_space;	//Structure containing tracked points within cockpit space

char control_tone = 0;

char blobs = 0; //Bits <0:3> determine which points the sensor has detected

int main(void) 
{
    
    init_setup();	//Function in init.c to configure hardware
    /************************Debug************************/
        //T1CONbits.TON = 0;          //Stop timer
    /************************Debug************************/
    char err = 0;
    Motor_Enable = 1; //Enable motor driver
	
    while(1){
        if (!BTN1)				//Poll calibration button on each run
            control_calib();	//And calibrate if presesd
        err = 0;
        
        sensor_rst();			//Reset both sensors
        __delay32(100);			//delet this??
        sensor_init();			//Initialise both sensors
        
		LRPin = 0;				//Select Left Camera
        //__delay32(500);
        blobs = sensor_read1(&LCam);//Read from left camera storing data in LCam
		err = ident(&LCam); 		//Identify front and back LED, returns 0 if successful
        if (err){				//If identification was not successful (Usually means only one LED was found)
            control_tone = 1;	//then enable an audible error tone to alert the user
            continue;			//and restart measurement loop.
        }       
        
		LRPin = 1; 				//Select right camera
        //__delay32(500);
        blobs = sensor_read1(&RCam);//Read from right camera storing data in RCam
        err = ident(&RCam);		//Identify front and back LED, returns 0 if successful
        if (err){				//If identification was not successful (Usually means only one LED was found)
            control_tone = 1;	//then enable an audible error tone to alert the user
            continue;			//and restart measurement loop.
        }

        poscalc();				//Triangulate the position in space of helmet LEDs, results stored in c_space
        anglecalc();			//Calculate angle between helmet LEDs, results store in pan_angle and tilt_angle.
        prevAngle = x_target_raw;
        x_target_raw = 4000+159*pan_angle; 	//Convert angle in radians to angle in encoder counts
        y_target_raw = 3750+159*tilt_angle;	//again
        
		/* Identification of LEDs cannot determine which LED is which, so if the position appears to have rapidly flipped
		 *	180 degrees it is likely that the LEDs have been misidentified.*/
		if(fabsf(x_target_raw - prevAngle)>400){ //If this is the case
            if (reverse_dir == 1)                //Note that this is the case
                reverse_dir = 0;
            else reverse_dir = 1;
            anglecalc();      					//And recalculate the angles
            x_target_raw = 4000+159*pan_angle;	
            y_target_raw = 3750+159*tilt_angle;
        }
        
        control_tone = 0;						//Now that a successful measurement has been made, the error tone is disabled
        LEDPin = ~ LEDPin;						//LED flashes to show camera refresh rate during testing

        control_limit(&x_target_raw, &y_target_raw);	//Ensure that the input position does not exceed the turret's mechanical limits
        x_target = (x_target_raw + x_target )/2;		//Update the motor controller target, averaging current and previous position to
        y_target = (y_target_raw + y_target )/2;		//reduce jitter.
    }
    
    
               
    
    return 0;
}

//Function used for control modelling
/*void step( void ){
    
    x_target = 4250;
    __delay32(5000000);
    x_target = 4250;
    __delay32(50000);
    __delay32(5000000);
}*/



/*Identify front and back points of image*/
char ident( struct Camera *Cam ){
    switch (blobs) { //First confirm that more than one point is detected
        case 0:
            return 1;   //Return error if not
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
    
    //Identify points with highest and lowest Y values, store in p1 and p2
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
    
    if (p1 && p2)       //If two points detected
        if(p1 == p2)    //And they're both highest and lowest Y
            error(1);   //Something's gone horribly wrong...
    
    switcheroo(p1,p2, Cam); //Copy point data from raw data structure to detected data structure
    
    return 0;
}

/*Copies the detected point numbers p1 and p2 to Cam.S1 and Cam.S2*/
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

/*Calculate position in space of helmet LEDs
 * Result stored in c_space*/
void poscalc( void ){
    //FOV = 33deg h, 23 deg v. 
    //Output resolution = 1024x768
    //0.5759586 rad h
    //0.401425728 rad v
    //x res =  5.624596222x10^-4 rad/pixel
    //y res = 5.226897499x10^-4 rad/pixel
    
    
    float A, B, Zp; //Sensor mount, LCam-Helmet, Perpedicular Xaxis-Helmet
    float b, c; //Helm, RCam, LCam
    float y; //y plane angle to z axis
    //Point 1
    c = x_offset_l - LCam.P1.x*x_res; //Inside angle between left camera and 
    b = x_offset_r + RCam.P1.x*x_res; 
    y = LCam.P1.y*y_res - y_offset_l; 
    
    A = sensor_width;
    float sinc = sinf(c); //sinf(c) is used more than once, so the value is stored to avoid recalculating
                                
    B = (A*sinc)/sin(pi-(b + c));
	/*c_space.P1.z = B*sinc;
	  c_space.P1.y = tanf(y)*c_space.P1.z;
	 *Zp approximately equals z, so may be used to increase speed
	 *at the expense of reduced accuracy */
	Zp = B*sinc;
	c_space.P1.z = Zp*sinf(y);
    c_space.P1.x = B*cosf(c);
    c_space.P1.y = tanf(y)*Zp;
    
    //Repeat for point 2
    c = x_offset_l - LCam.P2.x*x_res;
    b = x_offset_r + RCam.P2.x*x_res; 
    y = LCam.P2.y*y_res - y_offset_l; 
    sinc = sinf(c);
    
    A = sensor_width;
    B = (A*sinc)/sin(pi-(b + c));
    Zp = B*sinc;
	c_space.P1.z = Zp*sinf(y);
    c_space.P1.x = B*cosf(c);
    c_space.P1.y = tanf(y)*Zp;
	
}

/*Calculate angle between cockpit LEDs
 * Results stored in pan_angle and tilt_angle */
void anglecalc(){
    struct Coord long_vector;
    //Populate vector between front and back LEDs
    if (reverse_dir){ //Reverse LED positions if helmet is facing backwards
        long_vector.x = c_space.P2.x - c_space.P1.x;
        long_vector.y = c_space.P2.y - c_space.P1.y;
        long_vector.z = c_space.P2.z - c_space.P1.z;
    }
    else{
        long_vector.x = c_space.P1.x - c_space.P2.x;
        long_vector.y = c_space.P1.y - c_space.P2.y;
        long_vector.z = c_space.P1.z - c_space.P2.z;
    }
    
    pan_angle = atan2f(long_vector.x , long_vector.y);
    tilt_angle = atan2f(sqrtf(long_vector.y*long_vector.y+ long_vector.x*long_vector.x), long_vector.z);
    
}


void __attribute__((interrupt, auto_psv)) _T1Interrupt( void ){ //Function name auto fills vector table to match
    /* Envelope
     * Centre 4000
     * XMax = 4500, XMin = 3500
     * YMax = 4250, YMin = 3250
     */
    IFS0bits.T1IF = 0; //Clear interrupt flag
    if (control_tone)
        LEDPin = ~LEDPin; //Produces an audible tone if control_tone is set, used for debugging
    control_step(); //Recalculate motor drive
}

