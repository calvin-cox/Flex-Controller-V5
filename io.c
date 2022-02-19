//  io.c  flex //  Created by Joey Tomlinson on 12/23/18.
#include "io.h"
#include "global.h"
#include "includes.h"
#include "motors.h"
#include "motionControl.h"
#include "TC_driver.h"
// set elevation power
void io_setElevationPower(bool powerOn) {
	if (powerOn)
	PORTQ.OUTSET = 0x1;
	else
	PORTQ.OUTCLR = 0x1;
}
void io_processFootPedal (bool home) {
	bool leftFlexNeg = ~PORTJ.IN & PIN3_bm;
	bool leftFlexPos = ~PORTJ.IN & PIN2_bm;
	bool leftRotNeg = ~PORTJ.IN & PIN1_bm;
	bool leftRotPos = ~PORTJ.IN & PIN0_bm;
	bool rightFlexNeg = ~PORTH.IN & PIN0_bm;
	bool rightFlexPos = ~PORTH.IN & PIN1_bm;
	bool rightRotNeg = ~PORTH.IN & PIN3_bm;
	bool rightRotPos = ~PORTH.IN & PIN2_bm;
	//        printf ("lfn lfp lrn lrp   rfn rfp rrn rrp\n");
	//        printf ("%d   %d   %d   %d     %d   %d   %d   %d   \n\n", leftFlexNeg, leftFlexPos, leftRotNeg, leftRotPos, rightFlexNeg, rightFlexPos, rightRotNeg, rightRotPos);
	if (home) {
		//printf("\nfoot pedal home\n");
		motionControl_home();
	}
	if (!gState.flexAxis.autoOn && !gState.flexAxis.positioningOn) {
		gState.flexAxis.direction = 0;
		if (leftFlexNeg || leftFlexPos)
		gState.flexAxis.side = LEFT_SIDE;
		if (rightFlexNeg || rightFlexPos)
		gState.flexAxis.side = RIGHT_SIDE;
		if (gState.mode == SINGLE) {
			if (leftFlexNeg)
			gState.flexAxis.direction = -1;
			if (leftFlexPos)
			gState.flexAxis.direction = 1;
			if (rightFlexNeg)
			gState.flexAxis.direction = -1;
			if (rightFlexPos)
			gState.flexAxis.direction = 1;
			} else if (gState.mode == DUAL) {
			if (leftFlexNeg)
			gState.flexAxis.direction = -1;
			if (leftFlexPos)
			gState.flexAxis.direction = 1;
			if (rightFlexNeg)
			gState.flexAxis.direction = -1;
			if (rightFlexPos)
			gState.flexAxis.direction = 1;
			if (leftRotNeg)
			gState.flexAxis.direction = -1;
			if (leftRotPos)
			gState.flexAxis.direction = 1;
			if (rightRotNeg)
			gState.flexAxis.direction = 1;
			if (rightRotPos)
			gState.flexAxis.direction = -1;
		}
	}
	if (!gState.rotAxis.autoOn && !gState.rotAxis.positioningOn) {
		gState.rotAxis.direction = 0;
		if (leftRotNeg || leftRotPos)
		gState.rotAxis.side = LEFT_SIDE;
		if (rightRotNeg || rightRotPos)
		gState.rotAxis.side = RIGHT_SIDE;
		if (gState.mode == SINGLE) {
			if (leftRotNeg)
			gState.rotAxis.direction = -1;
			if (leftRotPos)
			gState.rotAxis.direction = 1;
			if (rightRotNeg)
			gState.rotAxis.direction = -1;
			if (rightRotPos)
			gState.rotAxis.direction = 1;
			} else if (gState.mode == DUAL) {
			if (leftFlexNeg)
			gState.rotAxis.direction = -1;
			if (leftFlexPos)
			gState.rotAxis.direction = 1;
			if (rightFlexNeg)
			gState.rotAxis.direction = 1;
			if (rightFlexPos)
			gState.rotAxis.direction = -1;
			if (leftRotNeg)
			gState.rotAxis.direction = 1;
			if (leftRotPos)
			gState.rotAxis.direction = -1;
			if (rightRotNeg)
			gState.rotAxis.direction = 1;
			if (rightRotPos)
			gState.rotAxis.direction = -1;
		}
	}
}
// up away, down toward, up toward, down away    up away, down toward, up toward, down away
// tracks wether we are in the middle of a button hit
bool hit;
void io_processLatBumpers (bool rightLatPos, bool rightLatNeg, bool leftLatPos, bool leftLatNeg) {
	if (!gState.latAxis.autoOn && !gState.latAxis.positioningOn) {
		if (gState.latAxis.position > gConstants.latBumperMotionMax) {
			if (gState.latAxis.direction > 0) {
				gState.latAxis.bumperMotionState = 0;
				gState.latAxis.direction = 0;
			}
		}
		if (gState.latAxis.position < -1 * gConstants.latBumperMotionMax) {
			if (gState.latAxis.direction < 0) {
				gState.latAxis.bumperMotionState = 0;
				gState.latAxis.direction = 0;
			}
		}
		// if all buttons are released, set hit=false
		if (!rightLatPos && !leftLatPos && !rightLatNeg && !leftLatNeg) {
			_delay_ms(20); // debounce
			hit = false;
		}
		// if we aren't in the middle of a button press, accept new presses
		if (!hit) {
			// if this is the start of a new hit
			if (rightLatPos || leftLatPos || rightLatNeg || leftLatNeg) {
				hit = true;
				_delay_ms(20); // debounce
				if (gState.latAxis.bumperMotionState == 0) { // if lat motion is stopped, start it, if this is the start of a new hit
					if (rightLatPos) {
						//printf("\nright lat pos hit\n");
						gState.latAxis.bumperMotionState = 1;
						gState.latAxis.side = LEFT_SIDE;
						gState.latAxis.direction = 1;
					}
					if (leftLatPos) {
						//printf("\nleft lat pos hit\n");
						gState.latAxis.bumperMotionState = 1;
						gState.latAxis.side = RIGHT_SIDE;
						gState.latAxis.direction = 1;
					}
					if (rightLatNeg) {
						//printf("\nright lat neg hit\n");
						gState.latAxis.bumperMotionState = 1;
						gState.latAxis.side = LEFT_SIDE;
						gState.latAxis.direction = -1;
					}
					if (leftLatNeg) {
						//printf("\nleft lat neg hit\n");
						gState.latAxis.bumperMotionState = 1;
						gState.latAxis.side = RIGHT_SIDE;
						gState.latAxis.direction = -1;
					}
					} else { // if lat axis is not stopped, stop it
					gState.latAxis.bumperMotionState = 0;
					gState.latAxis.direction = 0;
				}
			}
		}
	}
}
void stopMotors () {
	motors_setFlexPwm(0);
	motors_setRotPwm(0);
	motors_setLatPwm(0);
}
void hand_stop (int window) {
	int pulse0 = 0;
	int pulse1 = 0;
	int pulse2 = 0;
	int period = 12500;
	io_setElevationPower(0); // disable elevation
	stopMotors(); // set motor PWMs to 0
	TC_SetCount(&TCD0, 0); // start timer and set to 0
	TC_SetPeriod(&TCD0, period );// Set the TC period.
	TC0_ConfigClockSource( &TCD0, TC_CLKSEL_DIV1024_gc );// Start timer by selecting a clock source.
	// 320000000/ 65535=2.1sec full period
	do {
		pulse0 = TCD0.CNT;
	} while (PORTK.IN & PIN5_bm && pulse0 < window);
	printf("%d first hand sw released\n", pulse0);
	_delay_ms(50); //debounce
	TC_SetCount(&TCD0, 0);
	do {
		pulse1 = TCD0.CNT;
	} while (~PORTK.IN & PIN5_bm && pulse1 < window);
	printf("%d second hand sw pressed\n", pulse1);
	_delay_ms(50); //debounce
	TC_SetCount(&TCD0, 0);
	do {
		pulse2 = TCD0.CNT;
	} while (PORTK.IN & PIN5_bm && pulse2 < window);
	printf("%d second button released\n", pulse2);
	_delay_ms(50); //debounce
	if (pulse0 > 1000 && pulse0 < window) {
		if (pulse1 > 1000 && pulse1 < window) {
			if (pulse2 > 1000 && pulse2 < window) {
				motionControl_home(); // home table
				printf("Homed via Hand SW\n");
				io_setElevationPower(1); // enable elevation
			}
		}
	}
}
void foot_stop (int window) {
	int pulse0 = 0;
	int pulse1 = 0;
	int pulse2 = 0;
	int period = 12500;
	io_setElevationPower(0); // disable elevation
	stopMotors(); // set motor PWMs to 0
	TC_SetCount(&TCD0, 0); // start timer and set to 0
	TC_SetPeriod( &TCD0, period );// Set the TC period.
	TC0_ConfigClockSource( &TCD0, TC_CLKSEL_DIV1024_gc );// Start timer by selecting a clock source.
	// 320000000/ 65535=2.1sec full period
	do {
		pulse0 = TCD0.CNT;
	} while (PORTD.IN & PIN5_bm && pulse0 < window);
	printf("%d first foot sw released\n", pulse0);
	_delay_ms(50);
	TC_SetCount(&TCD0, 0);
	do {
		pulse1 = TCD0.CNT;
	} while (~PORTD.IN & PIN5_bm && pulse0 < window);
	printf("%d second foot sw pressed\n", pulse1);
	_delay_ms(50); //debounce
	TC_SetCount(&TCD0, 0);
	do {
		pulse2 = TCD0.CNT;
		}while (PORTD.IN & PIN5_bm);
	printf("%d second foot sw released\n", pulse2);
	_delay_ms(50); //debounce
	if (pulse0 > (1000) && pulse0 < window) {
		if (pulse1 > (1000) && pulse1 < window) {
			if (pulse2 > (1000) && pulse2 < window) {
				motionControl_home(); // home table
				printf("Homed via Foot SW\n");
				io_setElevationPower(1); // enable elevation
			}
		}
	}
}
void io_processHandFootStop (bool hand, bool foot) {
	if (foot) {
		foot_stop(9000);
	}
	if (hand) {
		hand_stop(8000);
	}
}