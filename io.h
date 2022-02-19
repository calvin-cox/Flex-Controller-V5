//  io.h //  flex //  Created by Joey Tomlinson on 12/23/18.
#ifndef io_h
#define io_h

#include <stdio.h>
#include "includes.h"
#include "io.h"

void io_setElevationPower(bool powerOn);
void io_processFootPedal (bool home);
void io_processLatBumpers(bool rightLatPos, bool rightLatNeg, bool leftLatPos, bool leftLatNeg);
void io_processHandFootStop (bool hand, bool foot);

#endif /* io_h */
