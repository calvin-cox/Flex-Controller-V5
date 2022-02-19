//
//  motionControl.h
//  flex
//
//  Created by Joey Tomlinson on 12/24/18.
//
//

#ifndef motionControl_h
#define motionControl_h

#include <stdio.h>
#include "global.h"

void motionControl_position(volatile struct AxisState *axis, int16_t target);
void motionControl_home();
void motionControl_homeAxis(volatile struct AxisState *axis);

#endif /* motionControl_h */
