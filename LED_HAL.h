#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

//------------------------------------------
// LED API

// Initializes all the LEDs as output
void InitLEDs();

// 3 functions for LED on booster
void Toggle_Booster_LED();

void TurnON_Booster_LED();

void TurnOFF_Booster_LED();

// 3 functions for left LED on launchpad
void Toggle_Launchpad_Left_LED() ;

void TurnON_Launchpad_Left_LED();

void TurnOFF_Launchpad_Left_LED();

// 3 functions for right red LED on launchpad
void Toggle_Launchpad_Right_Red_LED();

void TurnON_Launchpad_Right_Red_LED();

void TurnOFF_Launchpad_Right_Red_LED();


// 3 functions for right green LED on launchpad
void Toggle_Launchpad_Right_Green_LED();

void TurnON_Launchpad_Right_Green_LED();

void TurnOFF_Launchpad_Right_Green_LED();

// 3 functions for rigth blue LED on launchpad
void Toggle_Launchpad_Right_Blue_LED();

void TurnON_Launchpad_Right_Blue_LED();

void TurnOFF_Launchpad_Right_Blue_LED();
