/* 
 * File:   shite.h
 * Author: tom
 *
 * Created on 19 March 2017, 14:41
 */

#ifndef SHITE_H
#define	SHITE_H

#ifdef	__cplusplus
extern "C" {
#endif

#define read_addr 0xB1
#define write_addr 0xB0
#define master_addr 0xA0
    
#define LEDPin          PORTAbits.RA0
#define LRPin           PORTBbits.RB13
#define RSTPin          PORTBbits.RB4
#define BTN1            PORTBbits.RB7
#define Motor_Enable    PORTBbits.RB6
    
#define BLOB1 0x01
#define BLOB2 0x02
#define BLOB3 0x04
#define BLOB4 0x08

void error( unsigned char );
void alert( unsigned char );
    
void __delay32(unsigned long cycles);

struct Point{
int x;
int y;
int size;
int intensity;
};

struct resPoint{
    int x;
    int y;
    char state; //Detected, not detected, correlation failed?
};

struct Coord{
    float x;
    float y;
    float z;
    char state; //Detected, not detected, correlation failed?
};



/* Structure containing camera data.
 * Sx = Slot x. Raw data for the point in Pixart slot x
 * Px = Point x. Points that have been identified and cross correlated
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

#endif	/* SHITE_H */

