//
//  controlPanels.h
//  flex
//
//  Created by Joey Tomlinson on 12/24/18.
//
//

#ifndef controlPanels_h
#define controlPanels_h

#include <stdio.h>
#include "global.h"

void controlPanels_init();

//void controlPanels_clear(); // ??

void controlPanels_setModeLights (mode_t mode);
void controlPanels_setAutoLight (axis_t axis, lightMode_t mode);

void controlPanels_setAutoRangeTopDisplay (axis_t axis, uint8_t value);
void controlPanels_setAutoRangeBottomDisplay (axis_t axis, uint8_t value);

#endif /* controlPanels_h */

