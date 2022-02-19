// controlPanels.c Created by Joey Tomlinson on 12/24/18 Modified by Calvin Cox 2021/11/24
#include "controlPanels.h"
#include "includes.h"
#include "global.h"
#include "motionControl.h"
#define BUTTON_FLEX_AUTO_RELEASE 'a'
#define BUTTON_ROT_AUTO_RELEASE 'b'
#define BUTTON_LAT_AUTO_RELEASE 'c'
#define BUTTON_LAT_AUTO 'C'
#define BUTTON_LAT_BOTTOM_KNOB 'K'
#define BUTTON_LAT_TOP_KNOB 'L'
#define BUTTON_FLEX_AUTO 'A'
#define BUTTON_FLEX_BOTTOM_KNOB 'G'
#define BUTTON_FLEX_TOP_KNOB 'H'
#define BUTTON_ROT_AUTO 'B'
#define BUTTON_ROT_BOTTOM_KNOB 'I'
#define BUTTON_ROT_TOP_KNOB 'J'
#define BUTTON_POWER 'D'
#define BUTTON_SINGLE 'E'
#define BUTTON_DUAL 'F'
#define LIGHT_POWER_ON 'D' //D
#define LIGHT_SINGLE_ON 'E'
#define LIGHT_DUAL_ON 'F'
#define LIGHT_POWER_OFF 'd'
#define LIGHT_SINGLE_OFF 'e'
#define LIGHT_DUAL_OFF 'f'
#define LIGHT_FLEX_ON 'A'
#define LIGHT_LAT_ON 'C'
#define LIGHT_ROT_ON 'B'
#define LIGHT_FLEX_OFF 'a'
#define LIGHT_LAT_OFF 'c'
#define LIGHT_ROT_OFF 'b'
#define LIGHT_FLEX_BLINK '0'
#define LIGHT_LAT_BLINK '2'
#define LIGHT_ROT_BLINK '1'
#define DISPLAY_FLEX_TOP 'H'
#define DISPLAY_FLEX_BOTTOM 'G'
#define DISPLAY_ROT_TOP 'J'
#define DISPLAY_ROT_BOTTOM 'I'
#define DISPLAY_LAT_TOP 'L'
#define DISPLAY_LAT_BOTTOM 'K'
#define KNOB_LAT_TOP_INCREMENT 'v'
#define KNOB_LAT_TOP_DECREMENT 'V'
#define KNOB_LAT_BOTTOM_INCREMENT 'u'
#define KNOB_LAT_BOTTOM_DECREMENT 'U'
#define KNOB_FLEX_TOP_INCREMENT 'r'
#define KNOB_FLEX_TOP_DECREMENT 'R'
#define KNOB_FLEX_BOTTOM_INCREMENT 'q'
#define KNOB_FLEX_BOTTOM_DECREMENT 'Q'
#define KNOB_ROT_TOP_INCREMENT 't'
#define KNOB_ROT_TOP_DECREMENT 'T'
#define KNOB_ROT_BOTTOM_INCREMENT 's'
#define KNOB_ROT_BOTTOM_DECREMENT 'S'
volatile uint8_t rightFlexSpeed;
volatile uint8_t rightRotSpeed;
volatile uint8_t leftFlexSpeed;
volatile uint8_t leftRotSpeed;
volatile side_t autoInitSide;
USART_t * usartRight = &USARTF0;
USART_t * usartLeft = &USARTF1;
void initCtrlPanel (USART_t* usart) {
  /* USARTF0, 8 Data bits, No Parity, 1 Stop bit. */
  USART_Format_Set(usart, USART_CHSIZE_8BIT_gc,
                   USART_PMODE_DISABLED_gc, false);
  //parity
  //USART_Format_Set(usart, USART_CHSIZE_8BIT_gc, USART_PMODE_ODD_gc, false);
  /* Enable RXC interrupt. */
  USART_RxdInterruptLevel_Set(usart, USART_RXCINTLVL_LO_gc);
  /* Set Baudrate to 9600 bps: */
  USART_Baudrate_Set(usart, 207 , 0);
  /* Enable both RX and TX. */
  USART_Rx_Enable(usart);
  USART_Tx_Enable(usart);
}
void controlPanels_init() {
  initCtrlPanel(usartRight);
  initCtrlPanel(usartLeft);
}
// flex long press stuff
void flexLongPress() {
  // printf("flexLongPress\n");
  if (gState.flexAxis.autoOn) {// cancel auto flex
    gState.flexAxis.autoOn = false;
    gState.flexAxis.direction = 0;
    gState.flexAxis.autoTargetRangeTop = 0;
    gState.flexAxis.autoTargetRangeBottom = 0;
  } else {
    gState.flexAxis.autoOn = true;
    gState.flexAxis.direction = 1;
    gState.flexAxis.side = autoInitSide;
    gState.flexAxis.autoCurrentRangeTop = gState.flexAxis.autoTargetRangeTop;
    gState.flexAxis.autoCurrentRangeBottom = gState.flexAxis.autoTargetRangeBottom;
    gState.flexAxis.autoCycleCount = 0;
    gState.flexAxis.autoIncrement = false;
    gState.flexAxis.autoTargetRangeBottom = gConstants.autoFlexTargetRangeBottomMin;
    gState.flexAxis.autoTargetRangeTop = gConstants.autoFlexTargetRangeTopMax;
  }
}
void flexShortPress () {
  //    printf("flexShortPress\n");
  if (gState.flexAxis.autoOn) {
    gState.flexAxis.autoIncrement = !gState.flexAxis.autoIncrement;
  } else {
    return;
  }
}
volatile uint8_t flexButtonTimerCounter = 0;
volatile uint8_t flexButtonLongPressFlag = false;
void startFlexButtonTimer () {// timer for debounce (100mS)
  //printf("stopFlexButtonTimer\n");
  flexButtonTimerCounter = 0;
  TC0_ConfigClockSource( &TCF0, TC_CLKSEL_OFF_gc );
  TC_SetCount(&TCF0, 0);
  TC_SetPeriod( &TCF0, 12500 );/* Set the TC period. */
  TC0_ConfigClockSource( &TCF0, TC_CLKSEL_DIV1024_gc );/* Start timer by selecting a clock source. */
  TC0_SetOverflowIntLevel( &TCF0, TC_OVFINTLVL_HI_gc );
}
void stopFlexButtonTimer() {
  //printf("stopFlexButtonTimer\n");
  TC0_ConfigClockSource( &TCF0, TC_CLKSEL_OFF_gc );
}
ISR(TCF0_OVF_vect) {
  flexButtonTimerCounter++;
  //printf("flex timer overflow cnt=%d\n", flexButtonTimerCounter);
  if (flexButtonTimerCounter == gConstants.autoButtonLongPressTime) {
    // timer has reached autoButtonLongPressTime before button was released
    // signal to handleFlexButtonRelease that we've handled this as a long press
    flexButtonLongPressFlag = true;
    flexLongPress();
    stopFlexButtonTimer();
  }
}
void handleFlexButtonPress() {
  //printf("handleFlexButtonPress\n");
  startFlexButtonTimer();
}
void handleFlexButtonRelease() {
  //printf("handleFlexButtonRelease\n");
  if (flexButtonLongPressFlag) {
    flexButtonLongPressFlag = false;
    return;
  } else {
    stopFlexButtonTimer();
    flexShortPress();
  }
}
// rot long press stuff
void rotLongPress() {
  //printf("rotLongPress\n");
  if (gState.rotAxis.autoOn) {
    // cancel auto rot
    gState.rotAxis.autoOn = false;
    gState.rotAxis.direction = 0;
    gState.rotAxis.autoTargetRangeTop = 0;
    gState.rotAxis.autoTargetRangeBottom = 0;
  } else {
    // start with current == target
    gState.rotAxis.autoOn = true;
    gState.rotAxis.direction = 1;
    gState.rotAxis.side = autoInitSide;
    gState.rotAxis.autoCurrentRangeTop = gState.rotAxis.autoTargetRangeTop;
    gState.rotAxis.autoCurrentRangeBottom = gState.rotAxis.autoTargetRangeBottom;
    gState.rotAxis.autoCycleCount = 0;
    gState.rotAxis.autoIncrement = false;
    gState.rotAxis.autoTargetRangeBottom = gConstants.autoRotTargetRangeBottomMin;
    gState.rotAxis.autoTargetRangeTop = gConstants.autoRotTargetRangeTopMax;
  }
}
void rotShortPress () {
  //printf("rotShortPress\n");
  if (gState.rotAxis.autoOn) {
    gState.rotAxis.autoIncrement = !gState.rotAxis.autoIncrement;
  } else {
    return;
  }
}
volatile uint8_t rotButtonTimerCounter = 0;
volatile uint8_t rotButtonLongPressFlag = false;
// timer for debounce (100mS)
void startRotButtonTimer () {
  //printf("stopRotButtonTimer\n");
  rotButtonTimerCounter = 0;
  TC1_ConfigClockSource( &TCC1, TC_CLKSEL_OFF_gc );
  TC_SetCount(&TCC1, 0);
  /* Set the TC period. */
  TC_SetPeriod( &TCC1, 12500 );
  /* Start timer by selecting a clock source. */
  TC1_ConfigClockSource( &TCC1, TC_CLKSEL_DIV1024_gc );
  TC1_SetOverflowIntLevel( &TCC1, TC_OVFINTLVL_HI_gc );
}
void stopRotButtonTimer() {
  //printf("stopRotButtonTimer\n");
  TC1_ConfigClockSource( &TCC1, TC_CLKSEL_OFF_gc );
}
ISR(TCC1_OVF_vect) {
  rotButtonTimerCounter++;
  //printf("rot timer overflow cnt=%d\n", rotButtonTimerCounter);
  if (rotButtonTimerCounter == gConstants.autoButtonLongPressTime) {
    // timer has reached autoButtonLongPressTime before button was released
    // signal to handleRotButtonRelease that we've handled this as a long press
    rotButtonLongPressFlag = true;
    rotLongPress();
    stopRotButtonTimer();
  }
}
void handleRotButtonPress() {
  //printf("handleRotButtonPress\n");
  startRotButtonTimer();
}
void handleRotButtonRelease() {
  //printf("handleRotButtonRelease\n");
  if (rotButtonLongPressFlag) {
    rotButtonLongPressFlag = false;
    return;
  } else {
    stopRotButtonTimer();
    rotShortPress();
  }
}
// lat long press stuff
void latLongPress() {
  //printf("latLongPress\n");
  if (gState.latAxis.autoOn) {
    // cancel auto lat
    gState.latAxis.autoOn = false;
    gState.latAxis.direction = 0;
    gState.latAxis.autoTargetRangeTop = 0;
    gState.latAxis.autoTargetRangeBottom = 0;
  } else {
    // start with current == target
    gState.latAxis.autoOn = true;
    gState.latAxis.direction = 1;
    gState.latAxis.side = autoInitSide;
    gState.latAxis.autoCurrentRangeTop = gState.latAxis.autoTargetRangeTop;
    gState.latAxis.autoCurrentRangeBottom = gState.latAxis.autoTargetRangeBottom;
    gState.latAxis.autoCycleCount = 0;
    gState.latAxis.autoIncrement = false;
    gState.latAxis.autoTargetRangeBottom = gConstants.autoLatTargetRangeBottomMin;
    gState.latAxis.autoTargetRangeTop = gConstants.autoLatTargetRangeTopMax;
  }
}
void latShortPress () {
  //printf("latShortPress\n");
  if (gState.latAxis.autoOn) {
    gState.latAxis.autoIncrement = !gState.latAxis.autoIncrement;
  } else {
    return;
  }
}
volatile uint8_t latButtonTimerCounter = 0;
volatile uint8_t latButtonLongPressFlag = false;
// timer for debounce (100mS)
void startLatButtonTimer () {
  //printf("stopLatButtonTimer\n");
  latButtonTimerCounter = 0;
  TC0_ConfigClockSource( &TCD0, TC_CLKSEL_OFF_gc );
  TC_SetCount(&TCD0, 0);
  /* Set the TC period. */
  TC_SetPeriod( &TCD0, 12500 );
  /* Start timer by selecting a clock source. */
  TC0_ConfigClockSource( &TCD0, TC_CLKSEL_DIV1024_gc );
  TC0_SetOverflowIntLevel( &TCD0, TC_OVFINTLVL_HI_gc );
}
void stopLatButtonTimer() {
  //printf("stopLatButtonTimer\n");
  TC0_ConfigClockSource( &TCD0, TC_CLKSEL_OFF_gc );
}
ISR(TCD0_OVF_vect) {
  latButtonTimerCounter++;
  //printf("lat timer overflow cnt=%d\n", latButtonTimerCounter);
  if (latButtonTimerCounter == gConstants.autoButtonLongPressTime) {
    // timer has reached autoButtonLongPressTime before button was released
    // signal to handleLatButtonRelease that we've handled this as a long press
    latButtonLongPressFlag = true;
    latLongPress();
    stopLatButtonTimer();
  }
}
void handleLatButtonPress() {
  //printf("handleLatButtonPress\n");
  startLatButtonTimer();
}
void handleLatButtonRelease() {
  //printf("handleLatButtonRelease\n");
  if (latButtonLongPressFlag) {
    latButtonLongPressFlag = false;
    return;
  } else {
    stopLatButtonTimer();
    latShortPress();
  }
}
//void controlPanels_clear() {
//    return;
//}
volatile char bufferRight[256];
volatile char bufferLeft[256];
volatile uint8_t rightHead = 0;
volatile uint8_t rightTail = 0;
volatile uint8_t leftHead = 0;
volatile uint8_t leftTail = 0;
void processCmd (uint8_t cmd, side_t side);
void sendRightPanel (char c) {
  // if buffer is not full
  if ((uint8_t)(rightHead + 1) != rightTail) {
    bufferRight[rightHead] = c;
    rightHead++;
    /* Enable DRE interrupt. */
    uint8_t tempCTRLA = usartRight->CTRLA;
    tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_LO_gc;
    usartRight->CTRLA = tempCTRLA;
  }
}
void sendLeftPanel (char c) {
  // if buffer is not full
  if ((uint8_t)(leftHead + 1) != leftTail) {
    bufferLeft[leftHead] = c;
    leftHead++;
    /* Enable DRE interrupt. */
    uint8_t tempCTRLA = usartLeft->CTRLA;
    tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_LO_gc;
    usartLeft->CTRLA = tempCTRLA;
  }
}
// on recieve
ISR(USARTF0_RXC_vect)
{
  uint8_t data = usartRight->DATA;
  processCmd (data, LEFT_SIDE);
}
ISR(USARTF1_RXC_vect)
{
  uint8_t data = usartLeft->DATA;
  processCmd (data, RIGHT_SIDE);
}
// send handler
ISR(USARTF0_DRE_vect)
{
  usartRight->DATA = bufferRight[rightTail];
  //printf("%c", bufferRight[rightTail]);
  rightTail++;
  if (rightTail == rightHead) {
    /* Disable DRE interrupts. */
    uint8_t tempCTRLA = usartRight->CTRLA;
    tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
    usartRight->CTRLA = tempCTRLA;
  }
}
ISR(USARTF1_DRE_vect)
{
  usartLeft->DATA = bufferLeft[leftTail];
  //printf("%c", bufferLeft[leftTail]);
  leftTail++;
  if (leftTail == leftHead) {
    /* Disable DRE interrupts. */
    uint8_t tempCTRLA = usartLeft->CTRLA;
    tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
    usartLeft->CTRLA = tempCTRLA;
  }
}
void sendPanels (char c) {
  sendRightPanel(c);
  sendLeftPanel(c);
}
void controlPanels_setModeLights (mode_t mode) {
  if (mode == POWER_OFF) {
    sendPanels((LIGHT_POWER_ON));
    sendPanels((LIGHT_SINGLE_OFF));
    sendPanels((LIGHT_DUAL_OFF));
  } else if (mode == SINGLE) {
    sendPanels((LIGHT_POWER_OFF));
    sendPanels((LIGHT_SINGLE_ON));
    sendPanels((LIGHT_DUAL_OFF));
  } else if (mode == DUAL) {
    sendPanels((LIGHT_POWER_OFF));
    sendPanels((LIGHT_SINGLE_OFF));
    sendPanels((LIGHT_DUAL_ON));
  }
}
void controlPanels_setAutoLight (axis_t axis, lightMode_t mode) {
  if (axis == FLEX) {
    if (mode == OFF)
      sendPanels(LIGHT_FLEX_OFF);
    if (mode == ON)
      sendPanels(LIGHT_FLEX_ON);
    if (mode == BLINK)
      sendPanels(LIGHT_FLEX_BLINK);
  }
  if (axis == ROT) {
    if (mode == OFF)
      sendPanels(LIGHT_ROT_OFF);
    if (mode == ON)
      sendPanels(LIGHT_ROT_ON);
    if (mode == BLINK)
      sendPanels(LIGHT_ROT_BLINK);
  }
  if (axis == LAT) {
    if (mode == OFF)
      sendPanels(LIGHT_LAT_OFF);
    if (mode == ON)
      sendPanels(LIGHT_LAT_ON);
    if (mode == BLINK)
      sendPanels(LIGHT_LAT_BLINK);
  }
}
void controlPanels_setAutoRangeTopDisplay (axis_t axis, uint8_t value) {
  if (axis == FLEX) {
    sendPanels(DISPLAY_FLEX_TOP);
    sendPanels(value);
  }
  if (axis == ROT) {
    sendPanels(DISPLAY_ROT_TOP);
    sendPanels(value);
  }
  if (axis == LAT) {
    sendPanels(DISPLAY_LAT_TOP);
    sendPanels(value);
  }
}
void controlPanels_setAutoRangeBottomDisplay (axis_t axis, uint8_t value) {
  if (axis == FLEX) {
    sendPanels(DISPLAY_FLEX_BOTTOM);
    sendPanels(value);
  }
  if (axis == ROT) {
    sendPanels(DISPLAY_ROT_BOTTOM);
    sendPanels(value);
  }
  if (axis == LAT) {
    sendPanels(DISPLAY_LAT_BOTTOM);
    sendPanels(value);
  }
}
static int i = 0 ;
void processCmd (uint8_t cmd, side_t side) {
  // printf ("%d ", cmd);
  //if (i==3) {
  //  printf ("\n");
  // i=0;
  // }
  //i++;
  // process speed commands
  if (cmd >= 0x80 && cmd < 0xA0) {
    if (side == RIGHT_SIDE)
      gState.flexAxis.rightPanelSetSpeed = cmd - 0x80;
    if (side == LEFT_SIDE) {
      gState.flexAxis.leftPanelSetSpeed = cmd - 0x80;
    }
  }
  if (cmd >= 0xA0 && cmd < 0xC0) {
    if (side == RIGHT_SIDE)
      gState.rotAxis.rightPanelSetSpeed = cmd - 0xA0;
    if (side == LEFT_SIDE) {
      gState.rotAxis.leftPanelSetSpeed = cmd - 0xA0;
    }
  }
  if (cmd >= 0xC0) {
    if (side == RIGHT_SIDE)
      gState.latAxis.rightPanelSetSpeed = cmd - 0xC0;
    if (side == LEFT_SIDE) {
      gState.latAxis.leftPanelSetSpeed = cmd - 0xC0;
    }
  }
  if (cmd < 0x80) {
    // todo - maybe remove debounce here?  put it in control panel itself?
    //        if (discardCmds)
    //            return;
    //        startDebounceTimer();
    switch (cmd) {
      case BUTTON_POWER:
        if (gState.mode != POWER_OFF) {
          gState.mode = POWER_OFF;
        }
        break;
      case BUTTON_SINGLE:
        // reset state
        if (gState.mode == POWER_OFF) {
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
          gState.flexAxis.BottomIncState = false;
          gState.flexAxis.TopIncState = false;
          gState.rotAxis.BottomIncState = false;
          gState.rotAxis.TopIncState = false;
          gState.latAxis.BottomIncState = false;
          gState.latAxis.TopIncState = false;
        }
        gState.mode = SINGLE;
        break;
      case BUTTON_DUAL:
        if (gState.mode == POWER_OFF) {
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
          gState.flexAxis.BottomIncState = false;
          gState.flexAxis.TopIncState = false;
          gState.rotAxis.BottomIncState = false;
          gState.rotAxis.TopIncState = false;
          gState.latAxis.BottomIncState = false;
          gState.latAxis.TopIncState = false;
        }
        gState.mode = DUAL;
        break;
      default:
        break;
    }
    // first press sets autoIncrement = true - blinking led
    // second press toggles autoIncrement - solid led
    if (gState.mode != POWER_OFF) {
      switch (cmd) {
        case (BUTTON_FLEX_AUTO): // when the button is pressed
          handleFlexButtonPress();
          autoInitSide = side;
          break;
        case (BUTTON_FLEX_AUTO_RELEASE): // when the button is pressed
          handleFlexButtonRelease();
          break;
        case (BUTTON_ROT_AUTO): // when the button is pressed
          handleRotButtonPress();
          autoInitSide = side;
          break;
        case (BUTTON_ROT_AUTO_RELEASE): // when the button is pressed
          handleRotButtonRelease();
          break;
        case (BUTTON_LAT_AUTO): // when the button is pressed
          handleLatButtonPress();
          autoInitSide = side;
          break;
        case (BUTTON_LAT_AUTO_RELEASE): // when the button is pressed
          handleLatButtonRelease();
          break;
        // for the knob buttons, in manual mode, they set position
        // in auto mode, they set current range to max
        case BUTTON_LAT_TOP_KNOB:
          if (!gState.latAxis.autoOn)
            motionControl_position(&gState.latAxis, gState.latAxis.autoTargetRangeTop);
          else {
            if (gState.latAxis.autoIncrement)
              gState.latAxis.TopIncState = !gState.latAxis.TopIncState;
          }
          break;
        case BUTTON_LAT_BOTTOM_KNOB:
          if (!gState.latAxis.autoOn)
            motionControl_position(&gState.latAxis, gState.latAxis.autoTargetRangeBottom);
          else {
            if (gState.latAxis.autoIncrement)
              gState.latAxis.BottomIncState = !gState.latAxis.BottomIncState;
          }
          break;
        case BUTTON_FLEX_TOP_KNOB:
          if (!gState.flexAxis.autoOn)
            motionControl_position(&gState.flexAxis, gState.flexAxis.autoTargetRangeTop);
          else {
            if (gState.flexAxis.autoIncrement)
              gState.flexAxis.TopIncState = !gState.flexAxis.TopIncState;
          }
          break;
        case BUTTON_FLEX_BOTTOM_KNOB:
          if (!gState.flexAxis.autoOn)
            motionControl_position(&gState.flexAxis, gState.flexAxis.autoTargetRangeBottom);
          else {
            if (gState.flexAxis.autoIncrement)
              gState.flexAxis.BottomIncState = !gState.flexAxis.BottomIncState;
          }
          break;
        case BUTTON_ROT_TOP_KNOB:
          if (!gState.rotAxis.autoOn)
            motionControl_position(&gState.rotAxis, gState.rotAxis.autoTargetRangeTop);
          else {
            if (gState.rotAxis.autoIncrement)
              gState.rotAxis.TopIncState = !gState.rotAxis.TopIncState;
          }
          break;
        case BUTTON_ROT_BOTTOM_KNOB:
          if (!gState.rotAxis.autoOn)
            motionControl_position(&gState.rotAxis, gState.rotAxis.autoTargetRangeBottom);
          else {
            if (gState.rotAxis.autoIncrement)
              gState.rotAxis.BottomIncState = !gState.rotAxis.BottomIncState;
          }
          break;
        // in auto, these knobs adjust current
        // when not in auto mode, they adjust target
        case KNOB_LAT_TOP_INCREMENT:
          if (gState.latAxis.autoOn) {
            if (gState.latAxis.autoCurrentRangeTop + gConstants.autoTargetKnobIncrementAmount < gConstants.autoLatTargetRangeTopMax)
              gState.latAxis.autoCurrentRangeTop += gConstants.autoTargetKnobIncrementAmount;
            else
              gState.latAxis.autoCurrentRangeTop = gConstants.autoLatTargetRangeTopMax;
          } else {
            if (gState.latAxis.autoTargetRangeTop + gConstants.autoTargetKnobIncrementAmount < gConstants.autoLatTargetRangeTopMax)
              gState.latAxis.autoTargetRangeTop += gConstants.autoTargetKnobIncrementAmount;
            else
              gState.latAxis.autoTargetRangeTop = gConstants.autoLatTargetRangeTopMax;
          }
          break;
        case KNOB_LAT_TOP_DECREMENT:
          if (gState.latAxis.autoOn) {
            if (gState.latAxis.autoCurrentRangeTop - gConstants.autoTargetKnobIncrementAmount > 0) {
              gState.latAxis.autoCurrentRangeTop -= gConstants.autoTargetKnobIncrementAmount;
            } else
              gState.latAxis.autoCurrentRangeTop = 0;
          } else {
            if (gState.latAxis.autoTargetRangeTop - gConstants.autoTargetKnobIncrementAmount > 0) {
              gState.latAxis.autoTargetRangeTop -= gConstants.autoTargetKnobIncrementAmount;
            } else
              gState.latAxis.autoTargetRangeTop = 0;
          }
          break;
        case KNOB_LAT_BOTTOM_INCREMENT:
          if (gState.latAxis.autoOn) {
            if (gState.latAxis.autoCurrentRangeBottom - gConstants.autoTargetKnobIncrementAmount > gConstants.autoLatTargetRangeBottomMin) {
              gState.latAxis.autoCurrentRangeBottom -= gConstants.autoTargetKnobIncrementAmount;
            } else
              gState.latAxis.autoCurrentRangeBottom = gConstants.autoLatTargetRangeBottomMin;
          } else {
            if (gState.latAxis.autoTargetRangeBottom - gConstants.autoTargetKnobIncrementAmount > gConstants.autoLatTargetRangeBottomMin) {
              gState.latAxis.autoTargetRangeBottom -= gConstants.autoTargetKnobIncrementAmount;
            } else
              gState.latAxis.autoTargetRangeBottom = gConstants.autoLatTargetRangeBottomMin;
          }
          break;
        case KNOB_LAT_BOTTOM_DECREMENT:
          if (gState.latAxis.autoOn) {
            if (gState.latAxis.autoCurrentRangeBottom + gConstants.autoTargetKnobIncrementAmount < 0)
              gState.latAxis.autoCurrentRangeBottom += gConstants.autoTargetKnobIncrementAmount;
            else
              gState.latAxis.autoCurrentRangeBottom = 0;
          } else {
            if (gState.latAxis.autoTargetRangeBottom + gConstants.autoTargetKnobIncrementAmount < 0)
              gState.latAxis.autoTargetRangeBottom += gConstants.autoTargetKnobIncrementAmount;
            else
              gState.latAxis.autoTargetRangeBottom = 0;
          }
          break;
        case KNOB_FLEX_TOP_INCREMENT:
          if (gState.flexAxis.autoOn) {
            if (gState.flexAxis.autoCurrentRangeTop + gConstants.autoTargetKnobIncrementAmount < gConstants.autoFlexTargetRangeTopMax)
              gState.flexAxis.autoCurrentRangeTop += gConstants.autoTargetKnobIncrementAmount;
            else
              gState.flexAxis.autoCurrentRangeTop = gConstants.autoFlexTargetRangeTopMax;
          } else {
            if (gState.flexAxis.autoTargetRangeTop + gConstants.autoTargetKnobIncrementAmount < gConstants.autoFlexTargetRangeTopMax)
              gState.flexAxis.autoTargetRangeTop += gConstants.autoTargetKnobIncrementAmount;
            else
              gState.flexAxis.autoTargetRangeTop = gConstants.autoFlexTargetRangeTopMax;
          }
          break;
        case KNOB_FLEX_TOP_DECREMENT:
          if (gState.flexAxis.autoOn) {
            if (gState.flexAxis.autoCurrentRangeTop - gConstants.autoTargetKnobIncrementAmount > 0) {
              gState.flexAxis.autoCurrentRangeTop -= gConstants.autoTargetKnobIncrementAmount;
            } else
              gState.flexAxis.autoCurrentRangeTop = 0;
          } else {
            if (gState.flexAxis.autoTargetRangeTop - gConstants.autoTargetKnobIncrementAmount > 0) {
              gState.flexAxis.autoTargetRangeTop -= gConstants.autoTargetKnobIncrementAmount;
            } else
              gState.flexAxis.autoTargetRangeTop = 0;
          }
          break;
        case KNOB_FLEX_BOTTOM_INCREMENT:
          if (gState.flexAxis.autoOn) {
            if (gState.flexAxis.autoCurrentRangeBottom - gConstants.autoTargetKnobIncrementAmount > gConstants.autoFlexTargetRangeBottomMin) {
              gState.flexAxis.autoCurrentRangeBottom -= gConstants.autoTargetKnobIncrementAmount;
            } else
              gState.flexAxis.autoCurrentRangeBottom = gConstants.autoFlexTargetRangeBottomMin;
          } else {
            if (gState.flexAxis.autoTargetRangeBottom - gConstants.autoTargetKnobIncrementAmount > gConstants.autoFlexTargetRangeBottomMin) {
              gState.flexAxis.autoTargetRangeBottom -= gConstants.autoTargetKnobIncrementAmount;
            } else
              gState.flexAxis.autoTargetRangeBottom = gConstants.autoFlexTargetRangeBottomMin;
          }
          break;
        case KNOB_FLEX_BOTTOM_DECREMENT:
          if (gState.flexAxis.autoOn) {
            if (gState.flexAxis.autoCurrentRangeBottom + gConstants.autoTargetKnobIncrementAmount < 0)
              gState.flexAxis.autoCurrentRangeBottom += gConstants.autoTargetKnobIncrementAmount;
            else
              gState.flexAxis.autoCurrentRangeBottom = 0;
          } else {
            if (gState.flexAxis.autoTargetRangeBottom + gConstants.autoTargetKnobIncrementAmount < 0)
              gState.flexAxis.autoTargetRangeBottom += gConstants.autoTargetKnobIncrementAmount;
            else
              gState.flexAxis.autoTargetRangeBottom = 0;
          }
          break;
        case KNOB_ROT_TOP_INCREMENT:
          if (gState.rotAxis.autoOn) {
            if (gState.rotAxis.autoCurrentRangeTop + gConstants.autoTargetKnobIncrementAmount < gConstants.autoRotTargetRangeTopMax)
              gState.rotAxis.autoCurrentRangeTop += gConstants.autoTargetKnobIncrementAmount;
            else
              gState.rotAxis.autoCurrentRangeTop = gConstants.autoRotTargetRangeTopMax;
          } else {
            if (gState.rotAxis.autoTargetRangeTop + gConstants.autoTargetKnobIncrementAmount < gConstants.autoRotTargetRangeTopMax)
              gState.rotAxis.autoTargetRangeTop += gConstants.autoTargetKnobIncrementAmount;
            else
              gState.rotAxis.autoTargetRangeTop = gConstants.autoRotTargetRangeTopMax;
          }
          break;
        case KNOB_ROT_TOP_DECREMENT:
          if (gState.rotAxis.autoOn) {
            if (gState.rotAxis.autoCurrentRangeTop - gConstants.autoTargetKnobIncrementAmount > 0) {
              gState.rotAxis.autoCurrentRangeTop -= gConstants.autoTargetKnobIncrementAmount;
            } else
              gState.rotAxis.autoCurrentRangeTop = 0;
          } else {
            if (gState.rotAxis.autoTargetRangeTop - gConstants.autoTargetKnobIncrementAmount > 0) {
              gState.rotAxis.autoTargetRangeTop -= gConstants.autoTargetKnobIncrementAmount;
            } else
              gState.rotAxis.autoTargetRangeTop = 0;
          }
          break;
        case KNOB_ROT_BOTTOM_INCREMENT:
          if (gState.rotAxis.autoOn) {
            if (gState.rotAxis.autoCurrentRangeBottom - gConstants.autoTargetKnobIncrementAmount > gConstants.autoRotTargetRangeBottomMin) {
              gState.rotAxis.autoCurrentRangeBottom -= gConstants.autoTargetKnobIncrementAmount;
            } else
              gState.rotAxis.autoCurrentRangeBottom = gConstants.autoRotTargetRangeBottomMin;
          } else {
            if (gState.rotAxis.autoTargetRangeBottom - gConstants.autoTargetKnobIncrementAmount > gConstants.autoRotTargetRangeBottomMin) {
              gState.rotAxis.autoTargetRangeBottom -= gConstants.autoTargetKnobIncrementAmount;
            } else
              gState.rotAxis.autoTargetRangeBottom = gConstants.autoRotTargetRangeBottomMin;
          }
          break;
        case KNOB_ROT_BOTTOM_DECREMENT:
          if (gState.rotAxis.autoOn) {
            if (gState.rotAxis.autoCurrentRangeBottom + gConstants.autoTargetKnobIncrementAmount < 0)
              gState.rotAxis.autoCurrentRangeBottom += gConstants.autoTargetKnobIncrementAmount;
            else
              gState.rotAxis.autoCurrentRangeBottom = 0;
          } else {
            if (gState.rotAxis.autoTargetRangeBottom + gConstants.autoTargetKnobIncrementAmount < 0)
              gState.rotAxis.autoTargetRangeBottom += gConstants.autoTargetKnobIncrementAmount;
            else
              gState.rotAxis.autoTargetRangeBottom = 0;
          }
          break;
        default:
          break;
      }
    }
  }
}