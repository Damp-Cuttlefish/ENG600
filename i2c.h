/* 
 * File:   i2c.h
 * Author: tom
 *
 * Created on 19 March 2017, 12:16
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif
    
void StartI2C( void );
void WriteI2C(unsigned char);
void StopI2C(void);
void IdleI2C(void);
void write_word(unsigned char, unsigned char);
void AckI2C(void);
void NAckI2C(void);
unsigned int getI2C(void);
void getsI2C(unsigned char *, unsigned char);




#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

