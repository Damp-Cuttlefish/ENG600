#ifndef __33FJ128MC802_H
#include "xc.h"
#endif
#include "i2c.h"
#include "misc.h"


void StartI2C( void )
{
	I2C1CONbits.SEN = 1;		//Generate Start Condition
	while (I2C1CONbits.SEN);	//Wait for Start Condition
}

void WriteI2C(unsigned char byte)
{
	//This function transmits the byte passed to the function
	I2C1TRN = byte;					//Load byte to I2C1 Transmit buffer
	while (I2C1STATbits.TBF);		//wait for data transmission
    if(I2C1STATbits.ACKSTAT == 1)
        error(6);
    if(I2C1STATbits.BCL == 1)
        error(2);
}

void StopI2C(void)
{
	//This function generates an I2C stop condition and returns status 
	//of the Stop.

	I2C1CONbits.PEN = 1;		//Generate Stop Condition
	//Nop();
	while (I2C1CONbits.PEN);	//Wait for Stop
	//return(I2C1STATbits.P);	//Optional - return status
}



void IdleI2C(void)
{
	while (I2C1STATbits.TRSTAT);		//Wait for bus Idle
}

void write_word(unsigned char byte1, unsigned char byte2){
    
    IdleI2C();
    StartI2C();
    WriteI2C(write_addr);
    IdleI2C();
    WriteI2C(byte1);
    IdleI2C();
    WriteI2C(byte2);
    IdleI2C();
    StopI2C();
    
}

void AckI2C(void)
{
	I2C1CONbits.ACKDT = 0;			//Set for ACk
	I2C1CONbits.ACKEN = 1;
	while(I2C1CONbits.ACKEN);		//wait for ACK to complete
}

void NAckI2C(void)
{
	I2C1CONbits.ACKDT = 1;			//Set for NotACk
	I2C1CONbits.ACKEN = 1;
	while(I2C1CONbits.ACKEN);		//wait for ACK to complete
	I2C1CONbits.ACKDT = 0;			//Set for NotACk
}

void getsI2C(unsigned char *rdptr, unsigned char Length)
{
	while (Length --)
	{
		*rdptr++ = getI2C();		//get a single byte
		
		if(I2C1STATbits.BCL)		//Test for Bus collision
			error(2);
        if(I2C1STATbits.I2COV)      //Test for missed data
            error(3);
        
        if(I2C1STATbits.ACKSTAT == 1)
            error(6);
        
        
		if(Length)
			AckI2C();				//Acknowledge until all read
        
	}
    NAckI2C();
}

unsigned int getI2C(void)
{
	I2C1CONbits.RCEN = 1;			//Enable Master receive
    //--------------Needs a watchdog timer--------------
	while(!I2C1STATbits.RBF);		//Wait for receive buffer to be full
	return(I2C1RCV);				//Return data in buffer
}
