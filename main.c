// main.c Calvin Cox 2022/02/19
#define F_CPU 32000000UL // define before any includes 32.000000 MHz (32000000UL)
#include "includes.h"
#include "init.h"
#include "pots.h"
#include "motors.h"
#include "io.h"
#include "controlPanels.h"
#include "motionControl.h"
#include "global.h"
#include "debugUsart.h"
int32_t maxCycles = 225; // max number of cycles before homing axis
void positioningIter (volatile struct AxisState *axis) { // this function is called every main loop
  if (axis->positioningOn) {
    if (axis->positioningInitial < axis->positioningTarget) { // moving positive to get to target
      if (axis->position < axis->positioningTarget - gConstants.positioningTurnOffDistance)
        axis->direction = 1;
      else {
        axis->direction = 0;
        axis->positioningOn = false;
      }
    }
    else if (axis->positioningInitial > axis->positioningTarget) { // moving negative to get to target
      if (axis->position > axis->positioningTarget + gConstants.positioningTurnOffDistance)
        axis->direction = -1;
      else {
        axis->direction = 0;
        axis->positioningOn = false;
      }
    } else {
      axis->direction = 0;
      axis->positioningOn = false;
    }
  }
}
void autoIter(volatile struct AxisState *axis) {  // this function is called every main loop
  int totalRange = axis->autoCurrentRangeTop - axis->autoCurrentRangeBottom;
  if (axis->autoOn) {
    if (axis->autoCycleCount <= maxCycles)  {

      if (totalRange <= 20)
        axis->autoSpeedLimit = 4 * totalRange;
      else
        axis->autoSpeedLimit = 28;
      if (axis->direction > 0) { // we are moving in a positive direction
        if (axis->position > axis->autoCurrentRangeTop) { // reached range top
          axis->direction = -1; // move in a negative direction
        }
      } else { // we are moving in a negative direction, or stopped
        if (axis->position < axis->autoCurrentRangeBottom + 1) { // reached range bottom
          axis->direction = 1; // move in a positive direction
          axis->autoCycleCount++;
          printf ("autoCycleCount LAT = %3d autoCycleCount ROT = %3d autoCycleCount FLEX = %3d\n", gState.latAxis.autoCycleCount, gState.rotAxis.autoCycleCount, gState.flexAxis.autoCycleCount);
          if (axis->autoIncrement) {  // if auto increment is true and if we haven't yet hit autoTargetRangeTop, increment
            if (axis->autoCurrentRangeTop < (axis->autoTargetRangeTop - gConstants.autoIncrementAmount) )
              axis->autoCurrentRangeTop += gConstants.autoIncrementAmount;
            else
              axis->autoCurrentRangeTop = axis->autoTargetRangeTop; // if we haven't hit autoTargetRangeBottom, increment
            if (axis->autoCurrentRangeBottom > axis->autoTargetRangeBottom + gConstants.autoIncrementAmount)
              axis->autoCurrentRangeBottom -= gConstants.autoIncrementAmount;
            else
              axis->autoCurrentRangeBottom = axis->autoTargetRangeBottom;
          }
        }
      }
      if (axis->autoCurrentRangeTop == 0 && axis->autoCurrentRangeBottom == 0) {
        axis->autoOn = false;
        axis->autoTargetRangeTop = 0;
        axis->autoTargetRangeBottom = 0;
        motionControl_position(axis, 0);
      }
    }
    else {
      axis->autoOn = false;
      axis->autoTargetRangeTop = 0;
      axis->autoTargetRangeBottom = 0;
      motionControl_position(axis, 0);

    }
  }
}
int main(int argc, const char * argv[]) {
  init();
  _delay_ms(1);
  printf ("Lifetimer Flex Table\nFirmware v5.1.0\n");
  long i = 0;
  bool powerOffSequenced = false;
  while (1) {
    if (gState.mode == POWER_OFF) {
      if (powerOffSequenced == false) {
        io_setElevationPower(false);
        motors_setRotPwm(0);
        motors_setFlexPwm(0);
        motors_setLatPwm(0);
        controlPanels_setModeLights(gState.mode);
        controlPanels_setAutoLight(FLEX, OFF);
        controlPanels_setAutoLight(LAT, OFF);
        controlPanels_setAutoLight(ROT, OFF);
        controlPanels_setAutoRangeBottomDisplay (FLEX, 0);
        controlPanels_setAutoRangeTopDisplay (FLEX, 0);
        controlPanels_setAutoRangeBottomDisplay (LAT, 0);
        controlPanels_setAutoRangeTopDisplay (LAT, 0);
        controlPanels_setAutoRangeBottomDisplay (ROT, 0);
        controlPanels_setAutoRangeTopDisplay (ROT, 0);
        powerOffSequenced = true;
      }
    } else {
      powerOffSequenced = false;
      io_setElevationPower(true);
      //DataHeader();
      bool handStop = PORTK.IN & PIN5_bm;
      bool footStop = PORTD.IN & PIN5_bm;
      int8_t holdCount = 0;
      if (handStop == 1 || footStop == 1) { // filter nuisance stopping
        holdCount++;
        do {
          handStop = PORTK.IN & PIN5_bm;
          footStop = PORTD.IN & PIN5_bm;
          if (handStop == 1 || footStop == 1) {
            holdCount++;
            if (holdCount > 1) {
              holdCount = 0;
              io_processHandFootStop(handStop, footStop);
            }
          }
          else {
            holdCount = 0;
          }
        } while (holdCount >= 1);
      } else {
        holdCount = 0; // end filtering nuisance stopping
      }
      bool rightLatPos = ~PORTD.IN & PIN3_bm;
      bool rightLatNeg = ~PORTD.IN & PIN2_bm;
      bool leftLatPos = ~PORTD.IN & PIN4_bm;
      bool leftLatNeg = ~PORTD.IN & PIN1_bm;
      io_processLatBumpers(rightLatPos, rightLatNeg, leftLatPos, leftLatNeg);
      bool homeButton = (~PORTJ.IN & PIN4_bm) | (~PORTH.IN & PIN4_bm);
      io_processFootPedal(homeButton);
      gState.flexAxis.position = pots_getFlexPosition();
      gState.rotAxis.position = pots_getRotPosition();
      gState.latAxis.position = pots_getLatPosition();
      autoIter(&gState.flexAxis);
      autoIter(&gState.rotAxis);
      autoIter(&gState.latAxis);
      positioningIter(&gState.flexAxis);
      positioningIter(&gState.rotAxis);
      positioningIter(&gState.latAxis);
      uint8_t flexSpeed = 0;
      if (gState.flexAxis.side == RIGHT_SIDE)
        flexSpeed = gState.flexAxis.rightPanelSetSpeed;
      if (gState.flexAxis.side == LEFT_SIDE)
        flexSpeed = gState.flexAxis.leftPanelSetSpeed;
      if (gState.flexAxis.autoOn) {
        if (flexSpeed > gState.flexAxis.autoSpeedLimit)
          flexSpeed = gState.flexAxis.autoSpeedLimit;
      }
      uint8_t rotSpeed = 0;
      if (gState.rotAxis.side == RIGHT_SIDE)
        rotSpeed = gState.rotAxis.rightPanelSetSpeed;
      if (gState.rotAxis.side == LEFT_SIDE)
        rotSpeed = gState.rotAxis.leftPanelSetSpeed;
      if (gState.rotAxis.autoOn) {
        if (rotSpeed > gState.rotAxis.autoSpeedLimit)
          rotSpeed = gState.rotAxis.autoSpeedLimit;
      }
      uint8_t latSpeed = 0;
      if (gState.latAxis.side == RIGHT_SIDE)
        latSpeed = gState.latAxis.rightPanelSetSpeed;
      if (gState.latAxis.side == LEFT_SIDE)
        latSpeed = gState.latAxis.leftPanelSetSpeed;
      if (gState.latAxis.autoOn) {
        if (latSpeed > gState.latAxis.autoSpeedLimit)
          latSpeed = gState.latAxis.autoSpeedLimit;
      }
      if (gState.flexAxis.positioningOn) {
        motors_setFlexPwm(gState.flexAxis.direction * gConstants.positioningSpeed);
      } else {
        motors_setFlexPwm(gState.flexAxis.direction * flexSpeed);
      }
      if (gState.rotAxis.positioningOn) {
        motors_setRotPwm(gState.rotAxis.direction * gConstants.positioningSpeed);
      } else {
        motors_setRotPwm(gState.rotAxis.direction * rotSpeed);
      }
      if (gState.latAxis.positioningOn) {
        motors_setLatPwm(gState.latAxis.direction * gConstants.positioningSpeed);
      } else {
        motors_setLatPwm(gState.latAxis.direction * latSpeed);
      }

      // end auto mode if both targets are 0
      if (gState.flexAxis.autoTargetRangeTop == 0 && gState.flexAxis.autoTargetRangeBottom == 0) {
        gState.flexAxis.autoOn = false;
        gState.flexAxis.autoCycleCount = 0;
      }
      if (gState.latAxis.autoTargetRangeTop == 0 && gState.latAxis.autoTargetRangeBottom == 0) {
        if (gState.latAxis.autoOn == true) {
          gState.flexAxis.autoOn = false;
          gState.latAxis.autoCycleCount = 0;
        }
      }
      if (gState.rotAxis.autoTargetRangeTop == 0 && gState.rotAxis.autoTargetRangeBottom == 0) {
        if (gState.rotAxis.autoOn == true) {
          gState.rotAxis.autoOn = false;
          gState.rotAxis.autoCycleCount = 0;
        }
      }
      // end increment if both have reached their max ie current == target
      if (gState.flexAxis.autoCurrentRangeTop == gState.flexAxis.autoTargetRangeTop && gState.flexAxis.autoCurrentRangeBottom == gState.flexAxis.autoTargetRangeBottom) {
        gState.flexAxis.autoIncrement = false;

      }
      if (gState.latAxis.autoCurrentRangeTop == gState.latAxis.autoTargetRangeTop && gState.latAxis.autoCurrentRangeBottom == gState.latAxis.autoTargetRangeBottom) {
        gState.latAxis.autoIncrement = false;
      }
      if (gState.rotAxis.autoCurrentRangeTop == gState.rotAxis.autoTargetRangeTop && gState.rotAxis.autoCurrentRangeBottom == gState.rotAxis.autoTargetRangeBottom) {
        gState.rotAxis.autoIncrement = false;
      }
      if (i > 200) { // refresh control panel displays less often to prevent overloading of their RX buffers
        // set control panel Lights:
        controlPanels_setModeLights(gState.mode);
        if (gState.flexAxis.autoOn) {
          controlPanels_setAutoRangeBottomDisplay (FLEX, -1 * gState.flexAxis.autoCurrentRangeBottom);
          controlPanels_setAutoRangeTopDisplay (FLEX, gState.flexAxis.autoCurrentRangeTop);
          if (gState.flexAxis.autoIncrement) controlPanels_setAutoLight(FLEX, BLINK); else controlPanels_setAutoLight(FLEX, ON);
        } else {
          controlPanels_setAutoLight(FLEX, OFF);
          controlPanels_setAutoRangeBottomDisplay (FLEX, -1 * gState.flexAxis.autoTargetRangeBottom);
          controlPanels_setAutoRangeTopDisplay (FLEX, gState.flexAxis.autoTargetRangeTop);
        }
        if (gState.latAxis.autoOn) {
          controlPanels_setAutoRangeBottomDisplay (LAT, -1 * gState.latAxis.autoCurrentRangeBottom);
          controlPanels_setAutoRangeTopDisplay (LAT, gState.latAxis.autoCurrentRangeTop);
          if (gState.latAxis.autoIncrement) controlPanels_setAutoLight(LAT, BLINK); else controlPanels_setAutoLight(LAT, ON);
        } else {
          controlPanels_setAutoLight(LAT, OFF);
          controlPanels_setAutoRangeBottomDisplay (LAT, -1 * gState.latAxis.autoTargetRangeBottom);
          controlPanels_setAutoRangeTopDisplay (LAT, gState.latAxis.autoTargetRangeTop);
        }
        if (gState.rotAxis.autoOn) {
          controlPanels_setAutoRangeBottomDisplay (ROT, -1 * gState.rotAxis.autoCurrentRangeBottom);
          controlPanels_setAutoRangeTopDisplay (ROT, gState.rotAxis.autoCurrentRangeTop);
          if (gState.rotAxis.autoIncrement) controlPanels_setAutoLight(ROT, BLINK); else controlPanels_setAutoLight(ROT, ON);
        } else {
          controlPanels_setAutoLight(ROT, OFF);
          controlPanels_setAutoRangeBottomDisplay (ROT, -1 * gState.rotAxis.autoTargetRangeBottom);
          controlPanels_setAutoRangeTopDisplay (ROT, gState.rotAxis.autoTargetRangeTop);
        }
        i = 0;
      } // end while (i > 200)
      i++;
    } // end else POWER_ON
  } // end while(1)
}
