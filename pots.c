//  pots.c
//  flex
//  Created by Joey Tomlinson on 12/24/18.
#include "pots.h"
#include "includes.h"
#include "global.h"

// todo sort out this left being rot and rot being flex business

//volatile uint8_t rotCurrentBuf[256];
//volatile uint8_t leftCurrentBuf[256];
//volatile uint16_t rotCurrentSum = 0;
//volatile uint16_t leftCurrentSum = 0;
//
//volatile uint8_t rotCurrentCnt = 0;
//volatile uint8_t leftCurrentCnt = 0;
//
//
//volatile uint8_t rotPositionBuf[256];
//volatile uint8_t leftPositionBuf[256];
//volatile uint16_t rotPositionSum = 0;
//volatile uint16_t leftPositionSum = 0;
//
//volatile uint8_t rotPositionCnt = 0;
//volatile uint8_t leftPositionCnt = 0;
//
//volatile uint8_t latPosBuf[256];
//volatile uint16_t latPosSum = 0;
//volatile uint8_t latPosCnt = 0;
//
//volatile uint8_t latSpeedBuf[256];
//volatile uint16_t latSpeedSum = 0;
//volatile uint8_t latSpeedCnt = 0;

void initAdcFreerun (ADC_t * adc) {
    ADC_CalibrationValues_Load (adc);
    ADC_ConvMode_and_Resolution_Config (adc, ADC_ConvMode_Unsigned, ADC_RESOLUTION_8BIT_gc);
    //ADC_Prescaler_Config (adc, ADC_PRESCALER_DIV32_gc); // Fadc = 125khz
    ADC_Prescaler_Config (adc, ADC_PRESCALER_DIV512_gc);
    ADC_Reference_Config (adc, ADC_REFSEL_VCC_gc); // vref = vcc/6
    ADC_SweepChannels_Config (adc, ADC_SWEEP_0123_gc);
    ADC_FreeRunning_Enable(adc);
    
    /* Setup channel 0, 1, 2 and 3 to have single ended input and 1x gain. */
    ADC_Ch_InputMode_and_Gain_Config (&(adc->CH0), ADC_CH_INPUTMODE_SINGLEENDED_gc, ADC_CH_GAIN_1X_gc);
    ADC_Ch_InputMux_Config(&(adc->CH0), ADC_CH_MUXPOS_PIN0_gc, ADC_CH_MUXNEG_PIN0_gc);
    ADC_Ch_InputMode_and_Gain_Config (&(adc->CH1), ADC_CH_INPUTMODE_SINGLEENDED_gc, ADC_CH_GAIN_1X_gc);
    ADC_Ch_InputMux_Config(&(adc->CH1), ADC_CH_MUXPOS_PIN1_gc, ADC_CH_MUXNEG_PIN1_gc);
    ADC_Ch_InputMode_and_Gain_Config (&(adc->CH2), ADC_CH_INPUTMODE_SINGLEENDED_gc, ADC_CH_GAIN_1X_gc);
    ADC_Ch_InputMux_Config(&(adc->CH2), ADC_CH_MUXPOS_PIN2_gc, ADC_CH_MUXNEG_PIN2_gc);
    ADC_Ch_InputMode_and_Gain_Config (&(adc->CH3), ADC_CH_INPUTMODE_SINGLEENDED_gc, ADC_CH_GAIN_1X_gc);
    ADC_Ch_InputMux_Config(&(adc->CH3), ADC_CH_MUXPOS_PIN3_gc, ADC_CH_MUXNEG_PIN3_gc);

    ADC_Enable (adc); // Enable ADC
    ADC_Wait_32MHz (adc); // Wait until common mode voltage is stable
}

uint8_t rotSampleCount = 0;
uint8_t leftSampleCount = 0;
uint8_t latPosSampleCount = 0;

ISR(ADCB_CH0_vect) {
    return;
//    if (rotSampleCount == 10) {
//        rotSampleCount = 0;
//        rotPositionBuf[rotPositionCnt] = ADCB.CH0.RES;
//        rotPositionSum += rotPositionBuf[rotPositionCnt];
//        rotPositionCnt++;
//        rotPositionSum -= rotPositionBuf[rotPositionCnt];
//    } else {
//        rotSampleCount++;
//    }
}
ISR(ADCB_CH1_vect) {
    return;
//    if (leftSampleCount == 10) {
//        leftSampleCount = 0;
//        leftPositionBuf[leftPositionCnt] = ADCB.CH1.RES;
//        leftPositionSum += leftPositionBuf[leftPositionCnt];
//        leftPositionCnt++;
//        leftPositionSum -= leftPositionBuf[leftPositionCnt];
//    } else {
//        leftSampleCount++;
//    }
}

// lateral position
ISR(ADCB_CH2_vect) {
    return;
//    if (latPosSampleCount == 10) {
//        latPosSampleCount = 0;
//        latPosBuf[latPosCnt] = ADCB.CH3.RES;
//        latPosSum += latPosBuf[latPosCnt];
//        latPosCnt++;
//        latPosSum -= latPosBuf[latPosCnt];
//    } else {
//        latPosSampleCount++;
//    }
}

// lateral position
ISR(ADCB_CH3_vect) {
    return;
//    if (latPosSampleCount == 10) {
//        latPosSampleCount = 0;
//        latPosBuf[latPosCnt] = ADCB.CH3.RES;
//        latPosSum += latPosBuf[latPosCnt];
//        latPosCnt++;
//        latPosSum -= latPosBuf[latPosCnt];
//    } else {
//        latPosSampleCount++;
//    }
}

void pots_initAdcs () {
    initAdcFreerun(&ADCB);
    
    ADC_Ch_Interrupts_Config(&(ADCB.CH0), ADC_CH_INTMODE_COMPLETE_gc, ADC_CH_INTLVL_LO_gc);
    ADC_Ch_Interrupts_Config(&(ADCB.CH1), ADC_CH_INTMODE_COMPLETE_gc, ADC_CH_INTLVL_LO_gc);
    ADC_Ch_Interrupts_Config(&(ADCB.CH2), ADC_CH_INTMODE_COMPLETE_gc, ADC_CH_INTLVL_LO_gc);
    ADC_Ch_Interrupts_Config(&(ADCB.CH3), ADC_CH_INTMODE_COMPLETE_gc, ADC_CH_INTLVL_LO_gc);
}

int16_t pots_getRotPosition () {
//    return ADCB.CH0.RES;
    return -1*((10*((int32_t)ADCB.CH1.RES+gConstants.rotPositionCalibrationAddition))/gConstants.rotPositionCalibrationDivisor);
}
int16_t pots_getFlexPosition () {
//    return ADCB.CH1.RES;
    return ((10*((int32_t)ADCB.CH0.RES+gConstants.flexPositionCalibrationAddition))/gConstants.flexPositionCalibrationDivisor);
}
int16_t pots_getLatPosition () {
//    return ADCB.CH2.RES;
    return ((10*((int32_t)ADCB.CH2.RES+gConstants.latPositionCalibrationAddition))/gConstants.latPositionCalibrationDivisor);
}
