/* 
 * File:   control.h
 * Author: up746099
 *
 * Created on 05 April 2017, 14:18
 */

#ifndef CONTROL_H
#define	CONTROL_H

#ifdef	__cplusplus
extern "C" {
#endif

void control_step( void );
char control_limit(float *x, float *y);



#ifdef	__cplusplus
}
#endif

#endif	/* CONTROL_H */

