//  global.h //  Created by Joey Tomlinson on 11/10/13 modified by Calvin Cox 2021/11/24
#include <inttypes.h>
#include <stdbool.h>
#ifndef flex_global_h
#define flex_global_h
void global_printGStateHeader();
void global_printGState();
typedef enum {POWER_OFF, SINGLE, DUAL} mode_t;
typedef enum {LAT, FLEX, ROT} axis_t;
typedef enum {OFF, ON, BLINK} lightMode_t;
typedef enum {RIGHT_SIDE, LEFT_SIDE} side_t;
// position units are degrees
// speed units are a 0-32 scale
struct AxisState {
    int8_t position; // current position of the table (read from pots)
    uint8_t rightPanelSetSpeed; // speed the table is set to move by ctrl panel knobs
    uint8_t leftPanelSetSpeed;
    int8_t direction; // 
    // -1 for left, 1 for right (0 for off!!)
    // this is the state of the current lat bumper motion
    int8_t bumperMotionState; // -1 moving in negative direction, 1 in pos, 0 not moving
	bool positioningOn; // is the table currently seeking a position
    int8_t positioningInitial;
	int8_t positioningTarget; // in positioning mode, this is the target position the table is seeking
    bool BottomIncState; // when knob buttons pressed during autoinc mode, activates given axis 
	bool TopIncState;
    bool autoOn; // is the table in auto mode
	// if auto is on these specify the target range of motion. Once the table reaches this range of motion it stops incrementing
    int8_t autoTargetRangeTop;
    int8_t autoTargetRangeBottom;
	// if auto is on these are the current cycle limits of table movement
	int8_t autoCurrentRangeTop;
	int8_t autoCurrentRangeBottom;
    uint8_t autoCycleCount; // number of cycles auto mode has performed
    uint8_t autoSpeedLimit;
    side_t side;
    bool autoIncrement; // in auto mode this determines whether the table is automatically incrementing towards its target range of motion 
    axis_t axis; // which axis is this?
	int16_t cycleCount; // max number of cycles before homing axis
};
struct Constants {
    // these are constant values:
    uint8_t autoButtonLongPressTime;
    uint8_t positioningSpeed;   
    int8_t positioningTurnOffDistance;
    int8_t latBumperMotionMax;
    int16_t rotPositionCalibrationAddition;
    int16_t flexPositionCalibrationAddition;
    int16_t latPositionCalibrationAddition;
    int16_t rotPositionCalibrationDivisor;
    int16_t flexPositionCalibrationDivisor;
    int16_t latPositionCalibrationDivisor;
    int16_t autoIncrementAmount;   
    uint8_t autoTargetKnobIncrementAmount;
    int8_t autoFlexTargetRangeTopMax;
    int8_t autoFlexTargetRangeBottomMin;
    int8_t autoLatTargetRangeTopMax;
    int8_t autoLatTargetRangeBottomMin;
    int8_t autoRotTargetRangeTopMax;
    int8_t autoRotTargetRangeBottomMin;
};
struct State {
    mode_t mode;
    volatile struct AxisState flexAxis;
    volatile struct AxisState rotAxis;
    volatile struct AxisState latAxis;
};
extern volatile struct State gState;
extern volatile struct Constants gConstants;
#endif