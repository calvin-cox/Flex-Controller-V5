//
//  ctrlPanels.c
//  flex
//
//  Created by Joey Tomlinson on 11/10/13.
//
//

// FOOT PEDALS ARE SUPPOSED TO (WHEN LOOKING AT TABLE, head to foot) - EXTENSION(UP) FLELXION(DOWN) TOWARDS AWAY

// AWAY - UP
// TOWARDS - DOWN
// UP - TOWARDS
// DOWN - AWAY
//
//head to foot
//EXTENSION/TOWARDS
//FLEXION/AWAY
//EXTENSION/AWAY
//FLEXION/TOWARDS

#include "includes.h"
#include "global.h"

typedef enum {RIGHT_SIDE, LEFT_SIDE} side_t;

USART_t * usartRight = &USARTF0;
USART_t * usartLeft = &USARTF1;

volatile char bufferRight[256];
volatile char bufferLeft[256];
volatile uint8_t rightHead = 0;
volatile uint8_t rightTail = 0;
volatile uint8_t leftHead = 0;
volatile uint8_t leftTail = 0;

void processCmd (uint8_t cmd, side_t side);

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

void initCtrlPanels () {
    initCtrlPanel(usartRight);
    initCtrlPanel(usartLeft);
}

void sendRightPanel (char c) {
    
    // if buffer is not full
    if ((uint8_t)(rightHead+1) != rightTail) {
    
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
    if ((uint8_t)(leftHead+1) != leftTail) {
    
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
    processCmd (data, RIGHT_SIDE);
}

ISR(USARTF1_RXC_vect)
{
    uint8_t data = usartLeft->DATA;
    processCmd (data, LEFT_SIDE);
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

void addAutoLight (autoMotion_t motion, bool blink) {
    if (motion == AUTO_RIGHT && blink)
        sendPanels('4');
    else if (motion == AUTO_RIGHT)
        sendPanels('E');
    
    if (motion == AUTO_LEFT && blink)
        sendPanels('5');
    else if (motion == AUTO_LEFT)
        sendPanels('F');

    if (motion == AUTO_RIGHT_LEFT && blink)
        sendPanels('6');
    else if (motion == AUTO_RIGHT_LEFT)
        sendPanels('G');

    if (motion == AUTO_UP && blink)
        sendPanels('7');
    else if (motion == AUTO_UP)
        sendPanels('H');

    if (motion == AUTO_DOWN && blink)
        sendPanels('8');
    else if (motion == AUTO_DOWN)
        sendPanels('I');

    if (motion == AUTO_UP_DOWN && blink)
        sendPanels('9');
    else if (motion == AUTO_UP_DOWN)
        sendPanels('J');
}

void setAutoLights (autoMotion_t motion, bool blink) {
    
    if (motion == AUTO_RIGHT && blink)
        sendPanels('4');
    else if (motion == AUTO_RIGHT)
        sendPanels('E');
    else
        sendPanels ('e');
    
    if (motion == AUTO_LEFT && blink)
        sendPanels('5');
    else if (motion == AUTO_LEFT)
        sendPanels('F');
    else
        sendPanels('f');

    if (motion == AUTO_RIGHT_LEFT && blink)
        sendPanels('6');
    else if (motion == AUTO_RIGHT_LEFT)
        sendPanels('G');
    else
        sendPanels('g');

    if (motion == AUTO_UP && blink)
        sendPanels('7');
    else if (motion == AUTO_UP)
        sendPanels('H');
    else
        sendPanels('h');

    if (motion == AUTO_DOWN && blink)
        sendPanels('8');
    else if (motion == AUTO_DOWN)
        sendPanels('I');
    else
        sendPanels('i');

    if (motion == AUTO_UP_DOWN && blink)
        sendPanels('9');
    else if (motion == AUTO_UP_DOWN)
        sendPanels('J');
    else
        sendPanels('j');
}

void setModeLights (mode_t mode) {
    if (mode == OFF)
        sendPanels('C');
    else
        sendPanels ('c');
        
    if (mode == SINGLE)
        sendPanels('B');
    else
        sendPanels('b');

    if (mode == DUAL)
        sendPanels('A');
    else
        sendPanels('a');
}

void setPowerLights (bool power) {
    setAutoLights(NONE, false);
    
    sendPanels ('a');
    sendPanels('b');
    
    if (power)
        sendPanels('C');
    else
        sendPanels('c');
    
    if (power)
        sendPanels('D');
    else
        sendPanels('d');
}



volatile uint8_t rightFlexSpeed;
volatile uint8_t rightRotSpeed;

volatile uint8_t leftFlexSpeed;
volatile uint8_t leftRotSpeed;

volatile side_t autoInitSide;


// if we are in auto mode and it was initiated from side
// set flex and rot speed based on that side's set speed
void handleSpeedChangeDuringAuto (side_t side) {
    
    if (gState.autoMotion != AUTO_NONE && autoInitSide == side ) {
        if (side == LEFT_SIDE) {
            gState.flexSpeed = leftFlexSpeed;
            gState.rotSpeed = leftRotSpeed;
        } else if (side == RIGHT_SIDE) {
            gState.flexSpeed = rightFlexSpeed;
            gState.rotSpeed = rightRotSpeed;
        }
    }
}

void startAuto (autoMotion_t motion, side_t side) {
    autoInitSide = side;
    gState.autoMotion = motion;
    gState.autoCount = 0;
    gState.autoPosition = gAutoPositionIncrement;
    gState.autoDirection = UP;
    gState.autoPaused = false;
    setAutoLights (motion, false);
}

// add auto motion to a secondary axis
void startSecondaryAutoMotion (autoMotion_t motion, side_t side) {
    // dont set side based on secondary auto.  auto speed will be set from primary side
    //autoInitSide = side;
    gState.secondaryAutoMotion = motion;
    gState.secondaryAutoCount = 0;
    gState.secondaryAutoPosition = gAutoPositionIncrement;
    gState.secondaryAutoDirection = UP;
    gState.secondaryAutoPaused = false;
    addAutoLight (motion, false);
}

void togglePauseSecondaryAuto (autoMotion_t motion) {
    if (gState.secondaryAutoPaused)
        gState.secondaryAutoPaused = false;
    else
        gState.secondaryAutoPaused = true;
    addAutoLight (motion, gState.secondaryAutoPaused);
}

void togglePauseAuto (autoMotion_t motion) {
    if (gState.autoPaused)
        gState.autoPaused = false;
    else
        gState.autoPaused = true;
    addAutoLight (motion, gState.autoPaused);
}

// is motion allowed as a secondary to currentMotion?
bool isAllowedSecondaryMotion(motion, currentMotion) {
    if (motion == AUTO_DOWN || motion == AUTO_UP || motion == AUTO_UP_DOWN) {
        return (currentMotion == AUTO_RIGHT || currentMotion == AUTO_RIGHT_LEFT || currentMotion == AUTO_LEFT);
    }
    else if (motion == AUTO_RIGHT || motion == AUTO_LEFT || motion == AUTO_RIGHT_LEFT) {
        return (currentMotion == AUTO_UP || currentMotion == AUTO_UP_DOWN || currentMotion == AUTO_DOWN);
    }
    else {
        return false;
    }

}

void handleAutoPress (autoMotion_t motion, side_t side) {
    if (gState.autoMotion == motion) {
        togglePauseAuto (motion);
    }
    else if (gState.secondaryAutoMotion == motion){
        togglePauseSecondaryAuto(motion);
    }
    else if (gState.autoMotion == AUTO_NONE) {
        startAuto(motion,side);
    }
    else if (gState.secondaryAutoMotion == AUTO_NONE && isAllowedSecondaryMotion(motion, gState.autoMotion)) {
        startSecondaryAutoMotion(motion,side);
    }

}


volatile bool discardCmds = false;

// timer for debounce (100mS)
volatile int16_t counterForDebounceTimer = 0;
void startDebounceTimer () {
    
    discardCmds = true;
    
    TC0_ConfigClockSource( &TCF0, TC_CLKSEL_OFF_gc );
    TC_SetCount(&TCF0, 0);
    
    /* Set the TC period. */
    TC_SetPeriod( &TCF0, 12500 );
    /* Start timer by selecting a clock source. */
    
    TC0_ConfigClockSource( &TCF0, TC_CLKSEL_DIV256_gc );
    TC0_SetOverflowIntLevel( &TCF0, TC_OVFINTLVL_HI_gc );
}
ISR(TCF0_OVF_vect) {
    TC0_ConfigClockSource( &TCF0, TC_CLKSEL_OFF_gc );
    discardCmds = false;
}

void processCmd (uint8_t cmd, side_t side) {

    // process speed commands
    if (cmd >= 0x80 && cmd < 0xC0) {
        if (side == RIGHT_SIDE)
            rightFlexSpeed = cmd-0x80+flexSpeedCalibration;
        if (side == LEFT_SIDE) {
            leftFlexSpeed = cmd-0x80+flexSpeedCalibration;
        }
        handleSpeedChangeDuringAuto(side); // if auto mode is active, set speed apropriately
        /*
        if (side == LEFT_SIDE) {
            //if (cmd != 179 || leftFlexSpeed != 60)
                printf("%d %d ", cmd,leftFlexSpeed);
                printf ("%d\n", gState.flexSpeed);
        }
        */
    }
    
    if (cmd >= 0xC0) {
        if (side == RIGHT_SIDE)
            rightRotSpeed = cmd-0xC0+rotSpeedCalibration;
        if (side == LEFT_SIDE)
            leftRotSpeed = cmd-0xC0+rotSpeedCalibration;
        handleSpeedChangeDuringAuto(side);
    }
    
    if (cmd < 0x80) {
    
        if (discardCmds)
            return;
    
        startDebounceTimer();
        
        switch (cmd) {
            case 'D': // power pressed
                if (gState.power) {
                    gState.power = false;
                    setPowerLights(false);
                } else {
                    // reset state
                    gState.power = true;
                    gState.footStop = false;
                    gState.handStop = false;
                    gState.home = false;
                    gState.manualMotion = NONE;
                    gState.autoMotion = AUTO_NONE;
                    gState.secondaryAutoMotion = AUTO_NONE;
                    gState.mode = OFF;
                    
                    //setModeLights(OFF);
                    //setAutoLights(NONE, false);
                    
                    gState.flexSpeed = 0;
                    gState.rotSpeed = 0;
                    
                    setPowerLights(true);
                }
            break;
            
            case 'C': // mode off pressed
                if (gState.power) {
                    gState.mode = OFF;
                    setModeLights(OFF);
                    gState.manualMotion = NONE;
                    gState.autoMotion = AUTO_NONE;
                    gState.secondaryAutoMotion = AUTO_NONE;
                    setAutoLights(NONE, false);
                }
            break;
            
            case 'B': // mode single pressed
                if (gState.power) {
                    if (gState.mode != SINGLE) {
                        gState.autoMotion = AUTO_NONE;
                        gState.secondaryAutoMotion = AUTO_NONE;
                        setAutoLights(NONE, false);
                    }
                    gState.mode = SINGLE;
                    setModeLights(SINGLE);
                }
            break;
            
            case 'A': // mode dual pressed
                if (gState.power) {
                    if (gState.mode != DUAL) {
                        gState.autoMotion = AUTO_NONE;
                        gState.secondaryAutoMotion = AUTO_NONE;
                        setAutoLights(NONE, false);
                    }
                    gState.mode = DUAL;
                    setModeLights(DUAL);
                }
            break;
            
            default:
            break;
            
        }
        
        if (gState.power && gState.mode != OFF) {

            switch (cmd) {
                case 'E': // auto up pressed
                    handleAutoPress(AUTO_RIGHT, side);
                break;
                
                case 'F': // auto down pressed
                    handleAutoPress(AUTO_LEFT, side);
                break;
                
                case 'G': // auto updown pressed
                    handleAutoPress(AUTO_RIGHT_LEFT, side);
                break;
                
                case 'H': // auto right pressed
                    handleAutoPress(AUTO_UP, side);
                break;  
                
                case 'I': // auto left pressed
                    handleAutoPress(AUTO_DOWN, side);
                break;
                
                case 'J': // auto rightleft pressed
                    handleAutoPress(AUTO_UP_DOWN, side);
                break;

                default:
                
                break;
            }
        }
    }
}





// foot pedals n other buttons

// right buttons - PORTJ
// left buttons - PORTH

// 0 up
// 1 down
// 2 home
// 3 right
// 4 left

// hand stop - portk5
// foot stop - portd5

#include "global.h"

// todo set speed per side
void readButtons () {

    if ((uint8_t)~(PORTJ.IN|0b11110000)) {
        if (!isAutoFlex())
            gState.flexSpeed = leftFlexSpeed;
        if (!isAutoRot())
            gState.rotSpeed = leftRotSpeed;
    }
    if ((uint8_t)~(PORTH.IN|0b11110000)) {
        if (!isAutoFlex())
            gState.flexSpeed = rightFlexSpeed;
        if (!isAutoRot())
            gState.rotSpeed = rightRotSpeed;
    }
    
    gState.latRF = ~PORTD.IN&PIN3_bm;
    gState.latRR = ~PORTD.IN&PIN4_bm;
    gState.latLF = ~PORTD.IN&PIN1_bm;
    gState.latLR = ~PORTD.IN&PIN2_bm;

    gState.footStop = PORTD.IN&PIN5_bm;
    gState.handStop = ~PORTK.IN&PIN5_bm;
    gState.home = (~PORTJ.IN&PIN4_bm)|(~PORTH.IN&PIN4_bm);
    
    if (gState.mode == SINGLE || gState.autoMotion != AUTO_NONE) {
        if ((~PORTJ.IN&PIN1_bm) && (~PORTJ.IN&PIN3_bm))
            gState.manualMotion = DOWN_LEFT;
        else if ((~PORTJ.IN&PIN1_bm) && (~PORTJ.IN&PIN2_bm))
            gState.manualMotion = UP_LEFT;
        else if ((~PORTJ.IN&PIN0_bm) && (~PORTJ.IN&PIN3_bm))
            gState.manualMotion = DOWN_RIGHT;
        else if ((~PORTJ.IN&PIN0_bm) && (~PORTJ.IN&PIN2_bm))
            gState.manualMotion = UP_RIGHT;
        
        // pedal 1
        else if (~PORTJ.IN&PIN1_bm)
            gState.manualMotion = LEFT;
        else if (~PORTJ.IN&PIN0_bm)
            gState.manualMotion = RIGHT;
        else if (~PORTJ.IN&PIN3_bm)
            gState.manualMotion = DOWN;
        else if (~PORTJ.IN&PIN2_bm)
            gState.manualMotion = UP;
        
        else if ((~PORTH.IN&PIN2_bm) && (~PORTH.IN&PIN1_bm))
            gState.manualMotion = UP_RIGHT;
        else if ((~PORTH.IN&PIN2_bm) && (~PORTH.IN&PIN0_bm))
            gState.manualMotion = DOWN_RIGHT;
        else if ((~PORTH.IN&PIN3_bm) && (~PORTH.IN&PIN1_bm))
            gState.manualMotion = UP_LEFT;
        else if ((~PORTH.IN&PIN3_bm) && (~PORTH.IN&PIN0_bm))
            gState.manualMotion = DOWN_LEFT;
        
        // pedal 2
        else if (~PORTH.IN&PIN2_bm)
            gState.manualMotion = RIGHT;
        else if (~PORTH.IN&PIN3_bm)
            gState.manualMotion = LEFT;
        else if (~PORTH.IN&PIN0_bm)
            gState.manualMotion = DOWN;
        else if (~PORTH.IN&PIN1_bm)
            gState.manualMotion = UP;
        else
            gState.manualMotion = NONE;
    }
    
    if (gState.mode == DUAL && gState.autoMotion == AUTO_NONE) {
        // pedal 1
        if (~PORTJ.IN&PIN1_bm)
            gState.manualMotion = DOWN_RIGHT;
        else if (~PORTJ.IN&PIN0_bm)
            gState.manualMotion = UP_LEFT;
        else if (~PORTJ.IN&PIN3_bm)
            gState.manualMotion = DOWN_LEFT;
        else if (~PORTJ.IN&PIN2_bm)
            gState.manualMotion = UP_RIGHT;

        // pedal 2
        else if (~PORTH.IN&PIN2_bm)
            gState.manualMotion = DOWN_LEFT;
        else if (~PORTH.IN&PIN3_bm)
            gState.manualMotion = UP_RIGHT;
        else if (~PORTH.IN&PIN0_bm)
            gState.manualMotion = DOWN_RIGHT;
        else if (~PORTH.IN&PIN1_bm)
            gState.manualMotion = UP_LEFT;
        else
            gState.manualMotion = NONE;
    }
    
}
