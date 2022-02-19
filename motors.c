//
//  motors.c
//  flex
//
//  Created by Joey Tomlinson on 12/22/18.
//
//

#include "motors.h"
#include "includes.h"


int16_t latPwmSet = 0;
int16_t latPwmActual = 0;

int16_t rotPwmSet = 0;
int16_t rotPwmActual = 0;

int16_t flexPwmSet = 0;
int16_t flexPwmActual = 0;


// should not excede +/- 30
void motors_setLatPwm (int16_t pwm) {
    if (pwm > 0)
        latPwmSet = (pwm+5)*18;
    else if (pwm < 0)
        latPwmSet = (pwm-5)*18;
    else
        latPwmSet = 0;
}

void setLatPwmHard (int16_t pwm) {
    latPwmSet = pwm;
    latPwmActual = pwm;
}



// should not excede +/- 30
void motors_setRotPwm (int16_t pwm) {
    pwm = pwm*-1;
    if (pwm > 0)
        rotPwmSet = (pwm+5)*18;
    else if (pwm < 0)
        rotPwmSet = (pwm-5)*18;
    else
        rotPwmSet = 0;

}

void setRotPwmHard (int16_t pwm) {
    rotPwmSet = pwm;
    rotPwmActual = pwm;
}




// should not excede +/- 30
void motors_setFlexPwm (int16_t pwm) {
    if (pwm > 0)
        flexPwmSet = (pwm+5)*18;
    else if (pwm < 0)
        flexPwmSet = (pwm-5)*18;
    else
        flexPwmSet = 0;
}

void setFlexPwmHard (int16_t pwm) {
    flexPwmSet = pwm;
    flexPwmActual = pwm;
}




void actuallySetLatPwm (int16_t pwm) {
    if (pwm > 630)
        pwm = 630;
    if (pwm < -630)
        pwm = -630;
    
    if (pwm > 0) {
        TC_SetCompareA( &TCE0, pwm );
        TC_SetCompareB( &TCE0, 0 );
    } else if (pwm < 0) {
        TC_SetCompareA( &TCE0, 0 );
        TC_SetCompareB( &TCE0, -pwm );
    } else { // pwm == 0
        TC_SetCompareA( &TCE0, 0 );
        TC_SetCompareB( &TCE0, 0 );
    }
}

void actuallySetRotPwm (int16_t pwm) {
    if (pwm > 630)
        pwm = 630;
    if (pwm < -630)
        pwm = -630;
    
    if (pwm > 0) {
        TC_SetCompareA( &TCC0, pwm );
        TC_SetCompareB( &TCC0, 0 );
    } else if (pwm < 0) {
        TC_SetCompareA( &TCC0, 0 );
        TC_SetCompareB( &TCC0, -pwm );
    } else { // pwm == 0
        TC_SetCompareA( &TCC0, 0 );
        TC_SetCompareB( &TCC0, 0 );
    }
}

void actuallySetFlexPwm (int16_t pwm) {
    if (pwm > 630)
        pwm = 630;
    if (pwm < -630)
        pwm = -630;
    
    if (pwm > 0) {
        TC_SetCompareC( &TCC0, pwm );
        TC_SetCompareD( &TCC0, 0 );
    } else if (pwm < 0) {
        TC_SetCompareC( &TCC0, 0 );
        TC_SetCompareD( &TCC0, -pwm );
    } else { // pwm == 0
        TC_SetCompareC( &TCC0, 0 );
        TC_SetCompareD( &TCC0, 0 );
    }
}

ISR(TCD1_OVF_vect) {
    if (latPwmSet > latPwmActual)
        latPwmActual += 10;
    if (latPwmSet < latPwmActual)
        latPwmActual -=10;
    
    if (flexPwmSet > flexPwmActual)
        flexPwmActual += 10;
    if (flexPwmSet < flexPwmActual)
        flexPwmActual -=10;
    
    if (rotPwmSet > rotPwmActual)
        rotPwmActual +=10;
    if (rotPwmSet < rotPwmActual)
        rotPwmActual -=10;
    
    actuallySetLatPwm(latPwmActual);
    actuallySetRotPwm(rotPwmActual);
    actuallySetFlexPwm(flexPwmActual);

}

void motors_initPwm () {

    // motors 1 and 2
    // config dti
	AWEX_EnableDeadTimeInsertion( &AWEXC, AWEX_DTICCAEN_bm | AWEX_DTICCBEN_bm | AWEX_DTICCCEN_bm | AWEX_DTICCDEN_bm );
	AWEX_SetOutputOverrideValue( AWEXC, 0xff ); // override all the pins on portc
	AWEX_SetDeadTimesSymmetricalUnbuffered( AWEXC, 2 ); // dead time = 32 sys clk cycles for high and low. todo

    // 32,000,000hz sys clk / 25,000 pwm freq = 1280
	/* Set the TC period. */
	TC_SetPeriod( &TCC0, 640 );

	/* Configure the TC for dual slope mode. */
	TC0_ConfigWGM( &TCC0, TC_WGMODE_DS_T_gc );

	/* Enable Compare channel A, B, C and D */
	TC0_EnableCCChannels( &TCC0, TC0_CCAEN_bm );
    TC0_EnableCCChannels( &TCC0, TC0_CCBEN_bm );
    TC0_EnableCCChannels( &TCC0, TC0_CCCEN_bm );
    TC0_EnableCCChannels( &TCC0, TC0_CCDEN_bm );

	/* Start timer by selecting a clock source. */
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV1_gc );

    TC_SetCompareA( &TCC0, 0 );
    TC_SetCompareB( &TCC0, 0 );
    TC_SetCompareC( &TCC0, 0 );
    TC_SetCompareD( &TCC0, 0 );
    
    // motors 3 and 4
    
    // config dti
	AWEX_EnableDeadTimeInsertion( &AWEXE, AWEX_DTICCAEN_bm | AWEX_DTICCBEN_bm | AWEX_DTICCCEN_bm | AWEX_DTICCDEN_bm );
	AWEX_SetOutputOverrideValue( AWEXE, 0xff ); // override all the pins on porte 
//	AWEX_SetOutputOverrideValue( AWEXE, 0b00001111 ); // dont override mot4 pins. those are hijacked for claw.
	AWEX_SetDeadTimesSymmetricalUnbuffered( AWEXE, 2 ); // dead time = 32 sys clk cycles for high and low. todo

    // 32,000,000hz sys clk / 25,000 pwm freq = 1280
	TC_SetPeriod( &TCE0, 640 );

	TC0_ConfigWGM( &TCE0, TC_WGMODE_DS_T_gc );

	TC0_EnableCCChannels( &TCE0, TC0_CCAEN_bm );
    TC0_EnableCCChannels( &TCE0, TC0_CCBEN_bm );
    TC0_EnableCCChannels( &TCE0, TC0_CCCEN_bm );
    TC0_EnableCCChannels( &TCE0, TC0_CCDEN_bm );

	TC0_ConfigClockSource( &TCE0, TC_CLKSEL_DIV1_gc );

    TC_SetCompareA( &TCE0, 0 );
    TC_SetCompareB( &TCE0, 0 );
    TC_SetCompareC( &TCE0, 0 );
    TC_SetCompareD( &TCE0, 0 );
    
    
    // hijack usarte1 (porte pins 6 and 7) for roboclaw control
//    initClaw(clawUsart);
    
    
    // timer for soft stop and start
        /* Set the TC period. */
        TC_SetPeriod( &TCD1, 1250 );
        /* Start timer by selecting a clock source. */
        TC1_ConfigClockSource( &TCD1, TC_CLKSEL_DIV256_gc );
        TC1_SetOverflowIntLevel( &TCD1, RTC_OVFINTLVL_LO_gc );


}
void timers_init(){
	    // timer for soft stop and start
	    /* Set the TC period. */
	    TC_SetPeriod( &TCD1, 1250 );
	    /* Start timer by selecting a clock source. */
	    TC1_ConfigClockSource( &TCD1, TC_CLKSEL_DIV256_gc );
	    TC1_SetOverflowIntLevel( &TCD1, RTC_OVFINTLVL_LO_gc );
}
