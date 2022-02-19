// global.c  // flex // Created by Joey Tomlinson on 11/10/13. // Modified by Calvin Cox 2021-11-15
int cycleCount;
#include <stdio.h>
#include <string.h>
#include "global.h"
void global_printGStateHeader() {
	printf ("\nM ||F|  PO  RS  LS  DR  BM |  ON  IN  TA |  ON  TT  TB  CT  CB  CC  IS  IC |R|  PO  RS  LS  DR  BM |  ON  IN  TA |  ON  TT  TB  CT  CB  CC  IS  IC |L|   PO  RS  LS  DR  BM |  ON  IN  TA |  ON  TT  TB  CT  CB  CC  IS  IC");
}
void global_printGState() {
	printf ("\n%d ||F| %3d %3d %3d %3d %3d | %3d %3d %3d | %3d %3d %3d %3d %3d %3d %3d %3d |R| %3d %3d %3d %3d %3d | %3d %3d %3d | %3d %3d %3d %3d %3d %3d %3d %3d |L|  %3d %3d %3d %3d %3d | %3d %3d %3d | %3d %3d %3d %3d %3d %3d %3d %3d\n",
	gState.mode,
	gState.flexAxis.position,
	gState.flexAxis.rightPanelSetSpeed,
	gState.flexAxis.leftPanelSetSpeed,
	gState.flexAxis.direction,
	gState.flexAxis.bumperMotionState,
	gState.flexAxis.positioningOn,
	gState.flexAxis.positioningInitial,
	gState.flexAxis.positioningTarget,
	gState.flexAxis.autoOn,
	gState.flexAxis.autoTargetRangeTop,
	gState.flexAxis.autoTargetRangeBottom,
	gState.flexAxis.autoCurrentRangeTop,
	gState.flexAxis.autoCurrentRangeBottom,
	gState.flexAxis.autoCycleCount,
	gState.flexAxis.side,
	gState.flexAxis.autoIncrement,
	gState.rotAxis.position,
	gState.rotAxis.rightPanelSetSpeed,
	gState.rotAxis.leftPanelSetSpeed,
	gState.rotAxis.direction,
	gState.rotAxis.bumperMotionState,
	gState.rotAxis.positioningOn,
	gState.rotAxis.positioningInitial,
	gState.rotAxis.positioningTarget,
	gState.rotAxis.autoOn,
	gState.rotAxis.autoTargetRangeTop,
	gState.rotAxis.autoTargetRangeBottom,
	gState.rotAxis.autoCurrentRangeTop,
	gState.rotAxis.autoCurrentRangeBottom,
	gState.rotAxis.autoCycleCount,
	gState.rotAxis.side,
	gState.rotAxis.autoIncrement,
	gState.latAxis.position,
	gState.latAxis.rightPanelSetSpeed,
	gState.latAxis.leftPanelSetSpeed,
	gState.latAxis.direction,
	gState.latAxis.bumperMotionState,
	gState.latAxis.positioningOn,
	gState.latAxis.positioningInitial,
	gState.latAxis.positioningTarget,
	gState.latAxis.autoOn,
	gState.latAxis.autoTargetRangeTop,
	gState.latAxis.autoTargetRangeBottom,
	gState.latAxis.autoCurrentRangeTop,
	gState.latAxis.autoCurrentRangeBottom,
	gState.latAxis.autoCycleCount,
	gState.latAxis.side,
	gState.latAxis.autoIncrement
	);
}
// todo - comment these well
volatile struct State gState = { // global state
	.mode = POWER_OFF, // off, single, dual
	.flexAxis = {
		.rightPanelSetSpeed = 0,
		.leftPanelSetSpeed = 0,
		.position = 0,
		.direction = 0,
		.positioningOn = false,
		.positioningOn = false,
		.positioningInitial = 0,
		.positioningTarget = 0,
		.autoOn = false,
		.autoTargetRangeTop = 0,
		.autoTargetRangeBottom = 0,
		.autoCurrentRangeTop = 0,
		.autoCurrentRangeBottom = 0,
		.autoSpeedLimit = 28,
		.autoCycleCount = 0,
		.autoIncrement = 0,
		.side = RIGHT_SIDE,
		.axis = FLEX,
		.BottomIncState = false,
		.TopIncState = false,
		.cycleCount = 200
	},
	.rotAxis = {
		.rightPanelSetSpeed = 0,
		.leftPanelSetSpeed = 0,
		.position = 0,
		.direction = 0,
		.positioningOn = false,
		.positioningOn = false,
		.positioningInitial = 0,
		.positioningTarget = 0,
		.autoOn = false,
		.autoTargetRangeTop = 0,
		.autoTargetRangeBottom = 0,
		.autoCurrentRangeTop = 0,
		.autoCurrentRangeBottom = 0,
		.autoSpeedLimit = 28,
		.autoCycleCount = 0,
		.autoIncrement = 0,
		.side = RIGHT_SIDE,
		.axis = ROT,
		.BottomIncState = false,
		.TopIncState = false,
		.cycleCount = 200
	},
	.latAxis = {
		.rightPanelSetSpeed = 0,
		.leftPanelSetSpeed = 0,
		.position = 0,
		.direction = 0,
		.positioningOn = false,
		.positioningOn = false,
		.positioningInitial = 0,
		.positioningTarget = 0,
		.autoOn = false,
		.autoTargetRangeTop = 0,
		.autoTargetRangeBottom = 0,
		.autoCurrentRangeTop = 0,
		.autoCurrentRangeBottom = 0,
		.autoSpeedLimit = 28,
		.autoCycleCount = 0,
		.autoIncrement = 0,
		.side = RIGHT_SIDE,
		.axis = LAT,
		.BottomIncState = false,
		.TopIncState = false,
		.cycleCount = 255
	},
};
// these control the entire state of the table
volatile struct Constants gConstants = {  // these are constant values:
	.autoButtonLongPressTime = 3,
	.positioningSpeed = 10,
	.positioningTurnOffDistance = 0,
	.latBumperMotionMax = 7,
	.rotPositionCalibrationAddition = -142, //140
	.flexPositionCalibrationAddition = -147, //-147
	.latPositionCalibrationAddition = -128, //-134
	.rotPositionCalibrationDivisor = 39,
	.flexPositionCalibrationDivisor = 40,
	.latPositionCalibrationDivisor = 38, //40
	.autoIncrementAmount = 1,
	.autoTargetKnobIncrementAmount = 1, // the amount autoTargetRangeX increases per knob click
	.autoFlexTargetRangeTopMax = 20, //23
	.autoFlexTargetRangeBottomMin = -25, //30
	.autoLatTargetRangeTopMax = 10, // 10
	.autoLatTargetRangeBottomMin = -10, // 10
	.autoRotTargetRangeTopMax = 25, //20
	.autoRotTargetRangeBottomMin = -25 //20
};