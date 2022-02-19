//
//  init.c
//  flex
//
//  Created by Joey Tomlinson on 11/9/13.
//
//

#include "includes.h"

#include "debugUsart.h"
#include "motors.h"
#include "pots.h"
#include "io.h"
#include "controlPanels.h"

void initClock (void) {
        // enable 32mhz oscillator 
        CLKSYS_Enable( OSC_RC32MEN_bm );
        do {} while ( CLKSYS_IsReady( OSC_RC32MRDY_bm ) == 0 );

        // configure pll source = 32mhz oscillator/4 * 16 = 4*32mhz = 128mhz output
        CLKSYS_PLL_Config( OSC_PLLSRC_RC32M_gc, 16 );

        // enable pll
        CLKSYS_Enable( OSC_PLLEN_bm );
        do {} while ( CLKSYS_IsReady( OSC_PLLEN_bm ) == 0 );

        // enable prescale by 2 and 2 again to generate 2x and 4x clocks
        CCP = CCP_IOREG_gc;
        CLK.PSCTRL = CLK_PSBCDIV_2_2_gc;

        // select main clock source as pll output
        CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_PLL_gc );
}

void initPinDir () {
    PORTA.DIR = 0x0; // [0-3] sense
    PORTB.DIR = 0x0; // [0-3] pots [4-7] jtag
    PORTC.DIR = 0xff; // motor 1 and 2 out
    PORTD.DIR = 0b10000001; // [0-5] but4 [6-7] usart dbg, (pin 0 driven for lat switches)
    PORTD.PIN6CTRL = PORT_OPC_PULLUP_gc; // because ftdi chip leaves this floating when usb is disconnected
    
    PORTE.DIR = 0xff; // motor 3 and 4 out
    PORTE.DIR = 0b10001111; // hijack for roboclaw interface
    
    PORTF.DIR = 0b10001000; // [2-3] usart 1  [3-4] usart2
    
    // so that uart lines wont float when ctrl panels are not connected
    PORTF.PIN6CTRL = PORT_OPC_PULLUP_gc;
    PORTF.PIN2CTRL = PORT_OPC_PULLUP_gc;
    
    PORTH.DIR = 0x0; // [0-5] but1
    PORTJ.DIR = 0x0; // [0-5] but2
    PORTK.DIR = 0b00000001; // [0-5] but3 (led on portk 0)
    PORTQ.DIR = 0b00000011; // [0] ACoutEN

    
    PORTK.OUTSET = 0b00000001; // (led on portk 0)
    PORTQ.OUTSET = 0b00000010; // Turn on Motor drivers
    PORTD.OUTSET = 0b00000001; // pin 0 driven high for lat switches
    
    PORTD.PIN0CTRL = PORT_OPC_PULLUP_gc;
    PORTD.PIN1CTRL = PORT_OPC_PULLUP_gc;
    PORTD.PIN2CTRL = PORT_OPC_PULLUP_gc;
    PORTD.PIN3CTRL = PORT_OPC_PULLUP_gc;
    PORTD.PIN4CTRL = PORT_OPC_PULLUP_gc;
    PORTD.PIN5CTRL = PORT_OPC_PULLUP_gc;
    
    PORTH.PIN0CTRL = PORT_OPC_PULLUP_gc;
    PORTH.PIN1CTRL = PORT_OPC_PULLUP_gc;
    PORTH.PIN2CTRL = PORT_OPC_PULLUP_gc;
    PORTH.PIN3CTRL = PORT_OPC_PULLUP_gc;
    PORTH.PIN4CTRL = PORT_OPC_PULLUP_gc;
    PORTH.PIN5CTRL = PORT_OPC_PULLUP_gc;
    
    PORTJ.PIN0CTRL = PORT_OPC_PULLUP_gc;
    PORTJ.PIN1CTRL = PORT_OPC_PULLUP_gc;
    PORTJ.PIN2CTRL = PORT_OPC_PULLUP_gc;
    PORTJ.PIN3CTRL = PORT_OPC_PULLUP_gc;
    PORTJ.PIN4CTRL = PORT_OPC_PULLUP_gc;
    PORTJ.PIN5CTRL = PORT_OPC_PULLUP_gc;
    
    PORTK.PIN0CTRL = PORT_OPC_PULLUP_gc;
    PORTK.PIN1CTRL = PORT_OPC_PULLUP_gc;
    PORTK.PIN2CTRL = PORT_OPC_PULLUP_gc;
    PORTK.PIN3CTRL = PORT_OPC_PULLUP_gc;
    PORTK.PIN4CTRL = PORT_OPC_PULLUP_gc;
    PORTK.PIN5CTRL = PORT_OPC_PULLUP_gc;

}

void init () {
    initClock ();
    initPinDir ();
    debugUsart_init ();
    controlPanels_init();
    motors_initPwm();
    pots_initAdcs();
        
    /* Enable low level interrupts. */
	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_HILVLEN_bm;
	sei( );
}
