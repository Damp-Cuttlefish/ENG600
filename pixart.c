#include <xc.h>
#include "i2c.h"
#include "misc.h"

void __delay32(unsigned long cycles);

/*Function to hardware reset both cameras*/
void sensor_rst( void ){
    RSTPin = 0;
    __delay32(100);
    RSTPin = 1;
    __delay32(100);
}

/*Function that writes to cameras to confirm they are present and working
 * for debugging. Any fault should trigger an error.
 */
void sensor_ping(){
    __delay32(100000);
    IdleI2C();
    StartI2C();
    WriteI2C(write_addr);
    StopI2C();
}

/*Initialise both pixart sensors*/
void sensor_init( void ){
    //sensor_ping();
	//Config byte 1
    LRPin = 0;				//Select left camera
    write_word(0x30,0x01);	//Write config byte 1
    LRPin = 1;				//Select right camera
    write_word(0x30,0x01);  //Write config byte 1
	__delay32(5000);		//Wait 5000 cycles for camera to process
	//(Delay length obtained experimentally, sensor will NACK next byte if smaller value is used)
    
	//Config Byte 2
	LRPin = 0;
    write_word(0x30,0x08);
    LRPin = 1;
    write_word(0x30,0x08); 
	__delay32(5000);
    //Sensitivity Byte 0
	LRPin = 0;
    write_word(0x06,0x90);
    LRPin = 1;
    write_word(0x06,0x90);
	__delay32(5000);
    //Sensitivity Byte 1
	LRPin = 0;
    write_word(0x08,0xC0);
    LRPin = 1;
    write_word(0x08,0xC0); 
	__delay32(5000);
    //Sensitivity Byte 2
	LRPin = 0;
    write_word(0x1A,0x40);
    LRPin = 1;
    write_word(0x1A,0x40); 
	__delay32(5000);
    //Sensitivity Byte 3
	LRPin = 0;
    write_word(0x33,0x33);
    LRPin = 1;
    write_word(0x33,0x33); 
	__delay32(5000);
}


//Function no longer used now that sensors are reset after every frame
/*
char sensor_read( struct Camera *Cam ){
	//Much of this function is based on Stephen Hobley's PVision Arduino library, available here http://www.stephenhobley.com/arduino/PVision.zip
    unsigned char buffer[16];       //Receive buffer
    unsigned char blobcount = 0;    //Number of detected points
    unsigned char s = 0;            //Temporary variable for decoding received data
    
    unsigned char i;   
    for (i=0;i<16;i++)
        buffer[i]=0;
    //Clear buffer
    
    //Request data
    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(write_addr);
    WriteI2C(0x36);
    IdleI2C();
    StopI2C();
    IdleI2C();   
    
    //Restart bus
    __delay32(100);
    StartI2C();
    IdleI2C();
    //Read data from sensor
    WriteI2C(read_addr);
    IdleI2C();
    getsI2C(buffer, 16);
    StopI2C();
    
    blobcount = 0;
    //
    //Slot 1
    Cam->S1.x = buffer[1];
    Cam->S1.y = buffer[2];
    s = buffer[3];
    Cam->S1.x += (s & 0x30) <<4;
    Cam->S1.y += (s & 0xC0) <<2;
    Cam->S1.size = (s & 0x0F);
    blobcount |= (Cam->S1.size < 5)? BLOB1 : 0;
    
    //Slot 2
    Cam->S2.x = buffer[4];
    Cam->S2.y = buffer[5];
    s = buffer[6];
    Cam->S2.x += (s & 0x30) <<4;
    Cam->S2.y += (s & 0xC0) <<2;
    Cam->S2.size = (s & 0x0F);
    blobcount |= (Cam->S2.size < 5)? BLOB2 : 0;
    
    //Slot 3
    Cam->S3.x = buffer[7];
    Cam->S3.y = buffer[8];
    s = buffer[11];
    Cam->S3.x += (s & 0x30) <<4;
    Cam->S3.y += (s & 0xC0) <<2;
    Cam->S3.size = (s & 0x0F);
    blobcount |= (Cam->S3.size < 5)? BLOB3 : 0;
    
    //Slot 4
    Cam->S4.x = buffer[9];
    Cam->S4.y = buffer[10];
    s = buffer[11];
    Cam->S4.x += (s & 0x30) <<4;
    Cam->S4.y += (s & 0xC0) <<2;
    Cam->S4.size = (s & 0x0F);
    blobcount |= (Cam->S4.size < 5)? BLOB4 : 0;


    return blobcount;
    
}
*/

char sensor_read1( struct Camera *Cam ){
//Much of this function is based on Stephen Hobley's PVision Arduino library, available here http://www.stephenhobley.com/arduino/PVision.zip
    unsigned char buffer[16];       //Receive buffer
    unsigned char blobcount = 0;    //Number of detected points
    unsigned char s = 0;            //Temporary variable for decoding received data
   
    //Clear buffer
    unsigned char i;   
    for (i=0;i<16;i++)
        buffer[i]=0;
    
    //Request data from sensor
    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(write_addr);
    WriteI2C(0x36);
    IdleI2C();
    StopI2C();
    IdleI2C();    
    
    //Restart bus and read data
    __delay32(100);
    StartI2C();
    IdleI2C();
    WriteI2C(read_addr);
    IdleI2C();
    getsI2C(buffer, 16);
    StopI2C();
    
    blobcount = 0; //Points detected
    
    
    if (buffer[1] || buffer[2]){ //Determine if 1 leading 0x00 or 3
        //And decode received data accordingly
        //Slot 1
        Cam->S1.x = buffer[1];  //First 8 bits of x
        Cam->S1.y = buffer[2];  //First 8 bits of y
        s = buffer[3];
        Cam->S1.x += (s & 0x30) <<4; //Add final 2 bits of x
        Cam->S1.y += (s & 0xC0) <<2; // Final 2 bits of y
        Cam->S1.size = (s & 0x0F);   //Final 2 bits of s are point size
        blobcount |= (Cam->S1.size < 5)? BLOB1 : 0; //Set appropriate bit of blobcount if point detected

        //Slot 2
        Cam->S2.x = buffer[4];
        Cam->S2.y = buffer[5];
        s = buffer[6];
        Cam->S2.x += (s & 0x30) <<4;
        Cam->S2.y += (s & 0xC0) <<2;
        Cam->S2.size = (s & 0x0F);
        blobcount |= (Cam->S2.size < 5)? BLOB2 : 0;

        //Slot 3
        Cam->S3.x = buffer[7];
        Cam->S3.y = buffer[8];
        s = buffer[11];
        Cam->S3.x += (s & 0x30) <<4;
        Cam->S3.y += (s & 0xC0) <<2;
        Cam->S3.size = (s & 0x0F);
        blobcount |= (Cam->S3.size < 5)? BLOB3 : 0;

        //Slot 4
        Cam->S4.x = buffer[9];
        Cam->S4.y = buffer[10];
        s = buffer[11];
        Cam->S4.x += (s & 0x30) <<4;
        Cam->S4.y += (s & 0xC0) <<2;
        Cam->S4.size = (s & 0x0F);
        blobcount |= (Cam->S4.size < 5)? BLOB4 : 0;
    }
    else{
        //Slot 1
        Cam->S1.x = buffer[3];
        Cam->S1.y = buffer[4];
        s = buffer[5];
        Cam->S1.x += (s & 0x30) <<4;
        Cam->S1.y += (s & 0xC0) <<2;
        Cam->S1.size = (s & 0x0F);
        blobcount |= (Cam->S1.size < 15)? BLOB1 : 0;

        //Slot 2
        Cam->S2.x = buffer[6];
        Cam->S2.y = buffer[7];
        s = buffer[8];
        Cam->S2.x += (s & 0x30) <<4;
        Cam->S2.y += (s & 0xC0) <<2;
        Cam->S2.size = (s & 0x0F);
        blobcount |= (Cam->S2.size < 15)? BLOB2 : 0;

        //Slot 3
        Cam->S3.x = buffer[9];
        Cam->S3.y = buffer[10];
        s = buffer[11];
        Cam->S3.x += (s & 0x30) <<4;
        Cam->S3.y += (s & 0xC0) <<2;
        Cam->S3.size = (s & 0x0F);
        blobcount |= (Cam->S3.size < 15)? BLOB3 : 0;

        //Slot 4
        Cam->S4.x = buffer[12];
        Cam->S4.y = buffer[13];
        s = buffer[14];
        Cam->S4.x += (s & 0x30) <<4;
        Cam->S4.y += (s & 0xC0) <<2;
        Cam->S4.size = (s & 0x0F);
        blobcount |= (Cam->S4.size < 15)? BLOB4 : 0;
    }
    return blobcount;
    
}