/* 
 * File:   pixart.h
 * Author: tom
 *
 * Created on 19 March 2017, 12:33
 */

#ifndef PIXART_H
#define	PIXART_H

#ifdef	__cplusplus
extern "C" {
#endif


struct Camera;
void sensor_init( void );
char sensor_read( struct Camera * );
char sensor_read1( struct Camera * );
void send_byte( char );
void sensor_ping(void);
void sensor_rst( void );



#ifdef	__cplusplus
}
#endif

#endif	/* PIXART_H */

