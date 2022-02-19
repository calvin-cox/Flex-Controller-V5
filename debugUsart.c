//
//  debugUsart.c
//  flex
//
//  Created by Joey Tomlinson on 11/12/13.
//
//

#include <stdio.h>
#include "includes.h"
#include "global.h"
#include "io.h"

USART_t * usart = &USARTD1;

char buffer[256];
volatile uint8_t bufferHead = 0;
volatile uint8_t bufferTail = 0;

static int put_char(uint8_t c, FILE* stream);
static FILE mystdout = FDEV_SETUP_STREAM(put_char, NULL, _FDEV_SETUP_WRITE);

static int put_char(uint8_t c, FILE* stream)
{ 
	if (c == '\n') put_char('\r', stream); 
    
    // if buffer is not full
    if ((uint8_t)(bufferHead+1) != bufferTail) {
    
        buffer[bufferHead] = c;
        bufferHead++;
        
        /* Enable DRE interrupt. */
        uint8_t tempCTRLA = usart->CTRLA;
        tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_LO_gc;
        usart->CTRLA = tempCTRLA;
    }
    
    return 0;
}



void debugUsart_init () {

    /* USARTF0, 8 Data bits, No Parity, 1 Stop bit. */
    USART_Format_Set(usart, USART_CHSIZE_8BIT_gc,
                 USART_PMODE_DISABLED_gc, false);

    /* Enable RXC interrupt. */
    USART_RxdInterruptLevel_Set(usart, USART_RXCINTLVL_LO_gc);

    /* Set Baudrate to 115200 bps: */
    USART_Baudrate_Set(usart, 131 , -3);

    /* Enable both RX and TX. */    
    USART_Rx_Enable(usart);
    USART_Tx_Enable(usart);
    
    stdout = &mystdout;
    
}

void processDbgCmd (uint8_t cmd) {
//    // for bootloader
//    if (cmd == 0x1B && ~PORTK.IN&PIN5_bm) {
//        
//        PORTK.OUTCLR = 0b00000001;
//        _delay_ms(80);
//        PORTK.OUTSET = 0b00000001;
//        _delay_ms(80);
//        
//        // reset da chip
//        CCP = CCP_IOREG_gc;
//        RST.CTRL = RST_SWRST_bm;
//    }
//    //if (cmd == 't' || cmd == 'T')
//    //    startTestMode();
//    else
        printf ("Lifetimer Flex Distraction Table\nFirmware v1.5\n\n");
}

// on recieve
ISR(USARTD1_RXC_vect)
{
    uint8_t cmd = usart->DATA;
    processDbgCmd(cmd);
}


// send handler
ISR(USARTD1_DRE_vect)
{
    usart->DATA = buffer[bufferTail];
    bufferTail++;
    if (bufferTail == bufferHead) {
        /* Disable DRE interrupts. */
        uint8_t tempCTRLA = usart->CTRLA;
        tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
        usart->CTRLA = tempCTRLA;
    }
}
