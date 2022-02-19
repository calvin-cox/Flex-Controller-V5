//
//  motors.h
//  flex
//
//  Created by Joey Tomlinson on 12/22/18.
//
//

#ifndef motors_h
#define motors_h

#include <stdio.h>

// soft start included with these functions

// should not excede +/- 640
void motors_setRotPwm (int16_t pwm);

// should not excede +/- 640
void motors_setFlexPwm (int16_t pwm);

// should not excede +/- 640
void motors_setLatPwm (int16_t pwm);

void motors_initPwm ();
void timers_init();

#endif /* motors_h */
