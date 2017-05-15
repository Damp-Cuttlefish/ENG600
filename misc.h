/* 
 * File:   misc.h
 * Author: tom
 *
 * Created on 19 March 2017, 14:41
 */

#ifndef MISC_H
#define	MISC_H

#ifdef	__cplusplus
extern "C" {
#endif

#define read_addr 0xB1		//Sensor read address
#define write_addr 0xB0		//Sensor write address
#define master_addr 0xA0	//dsPIC I2C master address
    
#define LEDPin          PORTAbits.RA0	//Pin connected to external LED and speaker
#define LRPin           PORTBbits.RB13	//Pin to switch bus between left and right cameras (0=L)
#define RSTPin          PORTBbits.RB4	//Pin driving camera reset pins
#define BTN1            PORTBbits.RB7	//Pin connected to calibration button
#define Motor_Enable    PORTBbits.RB6	//Pin connected to motor driver enable, externally pulled low
    
#define BLOB1 0x01	//Bit masks for NOT STEALING STEPHEN HOBLEYS CODE
#define BLOB2 0x02
#define BLOB3 0x04
#define BLOB4 0x08

//Function Prototypes
void error( unsigned char );
void alert( unsigned char );

//XC16 Builting delay function
void __delay32(unsigned long cycles);

//Structure for storing raw point data from cameras
struct Point{
int x;
int y;
int size;
int intensity;
};

//Structure for storing resolved point data
//(Points identified as helmet LEDs)
struct resPoint{
    int x;
    int y;
    char state; //Detected, not detected, correlation failed?
};

//Structure for storing 3D point position data
struct Coord{
    float x;
    float y;
    float z;
    char state; //Detected, not detected, correlation failed?
};


/* Structure containing camera data.
 * Sx = Slot x. Raw data for the point in sensor slot x
 * Px = Point x. Points that have been identified as helmet LEDs
 */
struct Camera {
    struct Point S1;
    struct Point S2;
    struct Point S3;
    struct Point S4;
    struct resPoint P1;
    struct resPoint P2;
    struct resPoint P3;
    struct resPoint P4;
};

struct Space {
    struct Coord P1;
    struct Coord P2;
    struct Coord P3;
    struct Coord P4;
};




#ifdef	__cplusplus
}
#endif

#endif	/* MISC_H */

