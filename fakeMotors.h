//
//  fakeMotors.h
//  flex
//
//  Created by Joey Tomlinson on 11/26/13.
//
//

#include "includes.h"

#ifndef flex_fakeMotors_h
#define flex_fakeMotors_h

void setFakeRotSpeed (int16_t speed);
void setFakeFlexSpeed (int16_t speed);

int16_t getFakeRotSpeed ();
int16_t getFakeFlexSpeed ();

int16_t getFakeRotPosition ();
int16_t getFakeFlexPosition ();

void initFakeMotors();

#endif
