#include <xc.h>
#include "i2c.h"
#include "misc.h"




void __delay32(unsigned long cycles);

void sensor_rst( void ){
    RSTPin = 0;
    __delay32(100);
    RSTPin = 1;
    __delay32(100);
}
void sensor_ping(){
    __delay32(100000);
    IdleI2C();
    StartI2C();
    WriteI2C(write_addr);
    StopI2C();
}

void sensor_init( void ){
    //IRsensorAddress = 0xB0;
    //sensor_ping();
    write_word(0x30,0x01); __delay32(5000);
    write_word(0x30,0x08); __delay32(5000);
    write_word(0x06,0x90); __delay32(5000);
    write_word(0x08,0xC0); __delay32(5000);
    write_word(0x1A,0x40); __delay32(5000);
    write_word(0x33,0x33); __delay32(1000);
}



char sensor_read( struct Camera *Cam ){
    //Use -> to dereference the pointer on its left hand side and access the member on its right hand side.
    //Use . to access the member on its right hand side of the variable on its left hand side.
    //&x is ptr to x
    //Is explanation of language features in comments a sign of bad code?
    
    unsigned char buffer[16];
    unsigned char blobcount = 0;
    unsigned char s = 0;
    
    unsigned char i;   
    for (i=0;i<16;i++)
        buffer[i]=0;
    
    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(write_addr);
    WriteI2C(0x36);
    IdleI2C();
    StopI2C();
    IdleI2C();    
    
    __delay32(100);
    StartI2C();
    IdleI2C();
    WriteI2C(read_addr);
    IdleI2C();
    getsI2C(buffer, 16);
    StopI2C();
    
    blobcount = 0;
    
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

char sensor_read1( struct Camera *Cam ){
    //Use -> to dereference the pointer on its left hand side and access the member on its right hand side.
    //Use . to access the member on its right hand side of the variable on its left hand side.
    //&x is ptr to x
    //Is explanation of language features in comments a sign of bad code?
    
    unsigned char buffer[16];
    unsigned char blobcount = 0;
    unsigned char s = 0;
    
    unsigned char i;   
    for (i=0;i<16;i++)
        buffer[i]=0;
    
    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(write_addr);
    WriteI2C(0x36);
    IdleI2C();
    StopI2C();
    IdleI2C();    
    
    __delay32(100);
    StartI2C();
    IdleI2C();
    WriteI2C(read_addr);
    IdleI2C();
    getsI2C(buffer, 16);
    StopI2C();
    
    blobcount = 0;
    
    if (buffer[1] || buffer[2]){
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



void send_byte( char Data )
{
    /*
    Assert a Start condition on SDAx and SCLx.
    Send the I 2 C device address byte to the slave with a write indication.
    Wait for and verify an Acknowledge from the slave.
    Send the serial memory address high byte to the slave.
    Wait for and verify an Acknowledge from the slave.
    Send the serial memory address low byte to the slave.
    Wait for and verify an Acknowledge from the slave.
    Assert a Repeated Start condition on SDAx and SCLx.
    Send the device address byte to the slave with a read indication.
    Wait for and verify an Acknowledge from the slave.
    Enable the master reception to receive serial memory data.
    Generate an ACK or NACK condition at the end of a received byte of data.
    Generate a Stop condition on SDAx and SCLx.
     */
    
    
}
