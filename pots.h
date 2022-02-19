//
//  pots.h
//  flex
//
//  Created by Joey Tomlinson on 12/24/18.
//
//

#ifndef pots_h
#define pots_h

#include <stdio.h>

int16_t pots_getRotPosition ();
int16_t pots_getFlexPosition ();
int16_t pots_getLatPosition ();

void pots_initAdcs ();

#endif /* pots_h */
