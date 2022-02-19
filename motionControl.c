//  motionControl.c  Created by Joey Tomlinson on 12/24/18 modified by Calvin Cox 2021/11/24
#include "motionControl.h"
#include "global.h"
#include "includes.h"
#include "controlPanels.h"
void motionControl_position(volatile struct AxisState *axis, int16_t target) {
//    printf ("positioning on = true\n");
    axis->positioningOn = true;
//    printf ("positioning initial\n");
    axis->positioningInitial = axis->position;
//    printf ("positioning target = %d\n", target);
    axis->positioningTarget = target;
}
void motionControl_homeAxis(volatile struct AxisState *axis) {
	if (axis == FLEX){
		motionControl_position (&gState.flexAxis, 0);
		controlPanels_setAutoRangeBottomDisplay (FLEX, 0);
		controlPanels_setAutoRangeTopDisplay (FLEX, 0);
		gState.flexAxis.positioningOn = false;
		gState.flexAxis.autoOn = false;
		gState.flexAxis.direction = 0;
		gState.flexAxis.autoTargetRangeTop = 0;
		gState.flexAxis.autoTargetRangeBottom = 0;
	}
	if (axis == ROT){
		motionControl_position (&gState.rotAxis, 0);
		controlPanels_setAutoRangeBottomDisplay (ROT, 0);
		controlPanels_setAutoRangeTopDisplay (ROT, 0);
		gState.rotAxis.positioningOn = false;
		gState.rotAxis.autoOn = false;
		gState.rotAxis.bumperMotionState = false;
		gState.rotAxis.direction = 0;
		gState.rotAxis.autoTargetRangeTop = 0;
		gState.rotAxis.autoTargetRangeBottom = 0;
	}
	if (axis == LAT) {	
		motionControl_position (&gState.latAxis, 0);
		controlPanels_setAutoRangeBottomDisplay (LAT, 0);
		controlPanels_setAutoRangeTopDisplay (LAT, 0);
		gState.latAxis.positioningOn = false;
		gState.latAxis.autoOn = false;
		gState.latAxis.bumperMotionState = false;
		gState.latAxis.direction = 0;
		gState.latAxis.autoTargetRangeBottom = 0;
		gState.latAxis.autoTargetRangeTop = 0;
	}
}
void motionControl_home() {
    gState.flexAxis.positioningOn = false;
    gState.rotAxis.positioningOn = false;
    gState.latAxis.positioningOn = false;
    gState.flexAxis.autoOn = false;
    gState.rotAxis.autoOn = false;
    gState.latAxis.autoOn = false;
    gState.flexAxis.bumperMotionState = false;
    gState.rotAxis.bumperMotionState = false;
    gState.latAxis.bumperMotionState = false;
    gState.flexAxis.direction = 0;
    gState.rotAxis.direction = 0;
    gState.latAxis.direction = 0;
    gState.flexAxis.autoTargetRangeTop = 0;
    gState.flexAxis.autoTargetRangeBottom = 0;
    gState.rotAxis.autoTargetRangeTop = 0;
    gState.rotAxis.autoTargetRangeBottom = 0;
    gState.latAxis.autoTargetRangeTop = 0;
    gState.latAxis.autoTargetRangeBottom = 0;
    motionControl_position (&gState.flexAxis, 0);
    motionControl_position (&gState.rotAxis, 0);
    motionControl_position (&gState.latAxis, 0);
	//printf("Homed\n");
}