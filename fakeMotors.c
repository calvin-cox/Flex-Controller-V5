//
//  fakeMotors.c
//  flex
//
//  Created by Joey Tomlinson on 11/26/13.
//
//

#include "includes.h"

int16_t fakeRotSpeed = 0;
int16_t fakeFlexSpeed = 0;

volatile int16_t fakeRotPosition = 0;
volatile int16_t fakeFlexPosition = 0;

int16_t getFakeRotSpeed () {
    return fakeRotSpeed;
}

int16_t getFakeFlexSpeed () {
    return fakeFlexSpeed;
}

void setFakeRotSpeed (int16_t speed) {
    fakeRotSpeed = speed;
}
void setFakeFlexSpeed (int16_t speed) {
    fakeFlexSpeed = speed;
}

int16_t getFakeRotPosition () {
    return fakeRotPosition/50;
}
int16_t getFakeFlexPosition () {
    return fakeFlexPosition/50;
}

void initFakeMotors() {
    /* Set the TC period. */
    TC_SetPeriod( &TCF1, 2000 );
    /* Start timer by selecting a clock source. */
    TC1_ConfigClockSource( &TCF1, TC_CLKSEL_DIV1024_gc );
    TC1_SetOverflowIntLevel( &TCF1, RTC_OVFINTLVL_LO_gc );
}

ISR(TCF1_OVF_vect) {
    int16_t prevRot = fakeRotPosition;
    int16_t prevFlex = fakeFlexPosition;
    
    if ((fakeRotPosition+fakeRotSpeed) <= 200*50 && (fakeRotPosition+fakeRotSpeed) >= -200*50)
        fakeRotPosition += fakeRotSpeed;
    if ((fakeFlexPosition+fakeFlexSpeed) <=  200*50  && (fakeFlexPosition+fakeFlexSpeed) >= -200*50)
        fakeFlexPosition += fakeFlexSpeed;
    
    //printf("%d + %d = %d \t %d + %d = %d \n", prevRot, fakeRotSpeed, fakeRotPosition, prevFlex, fakeFlexSpeed, fakeFlexPosition);
    
}