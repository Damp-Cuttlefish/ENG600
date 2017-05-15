#ifndef __33FJ128MC802_H
#include "xc.h"
#endif
#include "i2c.h"
#include "misc.h"

/*Start I2C bus communication*/
void StartI2C( void )
{
	I2C1CONbits.SEN = 1;		//Generate Start Condition
	while (I2C1CONbits.SEN);	//Wait for Start Condition
}

/*Write one byte over I2C*/
void WriteI2C(unsigned char byte)
{
	I2C1TRN = byte;					//Load byte to I2C1 Transmit buffer
	while (I2C1STATbits.TBF);		//wait for data transmission
    if(I2C1STATbits.ACKSTAT == 1)
        error(6);
    if(I2C1STATbits.BCL == 1)
        error(2);
}

/*Stop I2C bus*/
void StopI2C(void)
{
	I2C1CONbits.PEN = 1;		//Generate Stop Condition
	while (I2C1CONbits.PEN);	//Wait for Stop
	//return(I2C1STATbits.P);	//Optional - return status
}

/*Wait for I2C bus to be idle*/
void IdleI2C(void)
{
	while (I2C1STATbits.TRSTAT);
}

/*Write two sequential bytes*/
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

/*Send I2C ACK*/
void AckI2C(void)
{
	I2C1CONbits.ACKDT = 0;			//Select ACK not NACK
	I2C1CONbits.ACKEN = 1;          //Start transmission
	while(I2C1CONbits.ACKEN);		//wait for completion
}

/*Send I2C NACK*/
void NAckI2C(void)
{
	I2C1CONbits.ACKDT = 1;			//Select NACK
	I2C1CONbits.ACKEN = 1;          //Send
	while(I2C1CONbits.ACKEN);		//Wait for completion
	I2C1CONbits.ACKDT = 0;			//Reset to ACK
}

/*Read $Length bytes from the bus into a buffer*/
void getsI2C(unsigned char *rdptr, unsigned char Length)
{
	while (Length --)
	{
		*rdptr++ = getI2C();		//get a single byte, store in buffer and increment buffer pointer
		
		if(I2C1STATbits.BCL)		//Test for Bus collision
			error(2);
        if(I2C1STATbits.I2COV)      //Test for missed data
            error(3);
        if(I2C1STATbits.ACKSTAT == 1) //Test for NACK received
            error(6);
        
		if(Length)
			AckI2C();				//ACK until end of buffer
        
	}
    NAckI2C();  //NACK final byte
}

/*Read one byte from the I2C bus*/
unsigned int getI2C(void)
{
	I2C1CONbits.RCEN = 1;			//Enable Master receive
    //Needs a watchdog timer
	//No it doesn't!
	while(!I2C1STATbits.RBF);		//Wait for receive buffer to be full
	return(I2C1RCV);				//Return data in buffer
}
