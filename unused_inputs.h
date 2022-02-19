//
//  controlPanel.h
//  flex
//
//  Created by Joey Tomlinson on 11/17/13.
//
//

#include "global.h"

#ifndef flex_controlPanel_h
#define flex_controlPanel_h

void setPowerLights(bool on);
void setAutoLights (autoMotion_t motion, bool blink);

void readButtons();
void initCtrlPanels ();

void sendRightPanel (char c);
void sendLeftPanel (char c);

#endif
