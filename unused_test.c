//
//  test.c
//  flex
//
//  Created by Joey Tomlinson on 12/18/13.
//
//

/*
#include <stdio.h>
#include <stdbool.h>
#include "test.h"

char readChar = 0;
char readCharCtrl1 = 0;
char readCharCtrl2 = 0;
bool powerBut = false;
bool safetyOffBut = false;
bool singleBut = false;
bool dualBut = false;
bool autoRightBut = false;
bool autoLeftBut = false;
bool autoRotBut = false;
bool autoExtBut = false;
bool autoFlexBut = false;
bool autoExtFlexBut = false;


void setReadChar (char c) {

}

void setReadCharCtrl1 (char c) {

}

setReadCharCtrl2 (char c) {

}
void ctrlPanelTest() {
    printf ("Starting Control Panel Test\n");
    printf ("All control panel lights should be off or dimmly lit\n");
    printf ("Please press each button on the RIGHT control panel. Each button should light up when pressed. A message will appear here for each button pressed.\n");
    
    while ( !(powerBut && safetyOffBut && singleBut && dualBut && autoRightBut && autoLeftBut && autoRotBut && autoExtBut && autoFlexBut && autoExtFlexBut) ) {
        char c = readCharCtrl1;
        if (c == 'A' || c == 'a') {
            powerBut = true;
            printf ("Power Button Pressed\n");
            sendRightPanel('A');
        }
        if (c == 'B') {
            powerBut = true;
            printf ("Power Button Pressed\n");
            sendRightPanel('B');
        }
    }
}

bool rightBut = false;
bool leftBut = false;
bool flexBut = false;
bool extBut = false;
bool homeBut = false;

void footPedalTest() {
    printf ("Starting Control Panel Test\n");
    if (// pedals are pressed) {
        printf ("Please ensure that no foot pedal buttons are pressed\n");
        printf ("Test will continue once no foot pedal buttons are pressed\n");
    }
    while (// pedals are pressed) {};
    
    printf ("Please press all buttons on the RIGHT foot pedal, including the home button.\n");
    printf ("A message will appear here for each button pressed.\n");

    while ( !(rightBut && leftBut && flexBut && extBut && homeBut) ) {
        // blarb
    }

}
void footStopTest() {
    printf ("Starting Foot Stop Test\n");
    if (// foot stop is active) {
        printf ("Please ensure that the foot stop button is not pressed\n");
        printf ("Test will continue once the foot stop button is released\n");
    }
    while (// foot stop is active) {};
    
    printf ("Please press the foot stop button\n");
    
    while (// foot stop is not active) {};
    
    printf ("Foot Stop Press Detected\n");
    printf ("\n*** Foot Stop Test passed!\n\n");
}
void handStopTest() {
    printf ("Starting Hand Stop Test\n");
    if (// foot stop is active) {
        printf ("Please ensure that the hand stop button is not pressed\n");
        printf ("Test will continue once the hand stop button is released\n");
    }
    while (// foot stop is active) {};
    
    printf ("Please press the hand stop button\n");
    
    while (// foot stop is not active) {};
    
    printf ("Hand Stop Press Detected\n");
    printf ("\n*** Hand Stop Test passed!\n\n");
}
void elevationTest() {

}
void motorTest() {

}

void allTests() {
    ctrlPanelTest();
    footPedalTest();
    footStopTest();
    handStopTest();
    elevationTest();
    motorTest();
}

void exitTestMode() {

}

void startTestMode () {

    // stop table motion

    printf("Availible Tests:\n");
    printf("1: Control Panel Test\n");
    printf("2: Foot Pedal Test\n");
    printf("3: Foot Stop Test\n");
    printf("4: Hand Stop Test\n");
    printf("5: Elevation Test\n");
    printf("6: Flexion/Extension/Rotation Test\n\n");
    printf("Press the test number to run a particular test.  Press \"A\" to run all tests.  Press \"E\" to exit test mode and restart the table.\n\n");
    
    while (1) {
        if (readChar == '1')
            ctrlPanelTest();
        if (readChar == '2')
            footPedalTest();
        if (readChar == '3')
            footStopTest();
        if (readChar == '4')
            handStopTest();
        if (readChar == '5')
            elevationTest();
        if (readChar == '6')
            motorTest();
        if (readChar == 'A')
            allTests();
        if (readChar == 'E')
            exitTestMode();
    }
}
*/