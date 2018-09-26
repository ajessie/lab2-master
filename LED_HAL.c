#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

//------------------------------------------
// LED API
void InitLEDs() {
    GPIO_setAsOutputPin    (GPIO_PORT_P2,    GPIO_PIN6);   // color LED red on booster board
    GPIO_setOutputLowOnPin (GPIO_PORT_P2,    GPIO_PIN6);

    GPIO_setAsOutputPin    (GPIO_PORT_P2,  GPIO_PIN0);    // Right red LED on launchpad
    GPIO_setOutputLowOnPin (GPIO_PORT_P2,  GPIO_PIN0);

    GPIO_setAsOutputPin    (GPIO_PORT_P2,  GPIO_PIN1);    // Right green LED on launchpad
    GPIO_setOutputLowOnPin (GPIO_PORT_P2,  GPIO_PIN1);

    GPIO_setAsOutputPin    (GPIO_PORT_P2,  GPIO_PIN2);    // Right blue LED on launchpad
    GPIO_setOutputLowOnPin (GPIO_PORT_P2,  GPIO_PIN2);

    GPIO_setAsOutputPin    (GPIO_PORT_P1,  GPIO_PIN0);    // Left LED on launchpad
    GPIO_setOutputLowOnPin (GPIO_PORT_P1,  GPIO_PIN0);
}

// 3 functions for LED on booster
void Toggle_Booster_LED(){
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN6);
}

void TurnON_Booster_LED(){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
}

void TurnOFF_Booster_LED(){
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
}

// 3 functions for left LED on launchpad
void Toggle_Launchpad_Left_LED() {
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

void TurnON_Launchpad_Left_LED(){
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

void TurnOFF_Launchpad_Left_LED(){
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

// 3 functions for right red LED on launchpad
void Toggle_Launchpad_Right_Red_LED() {
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);
}

void TurnON_Launchpad_Right_Red_LED() {
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
}

void TurnOFF_Launchpad_Right_Red_LED() {
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
}


// 3 functions for right green LED on launchpad
void Toggle_Launchpad_Right_Green_LED() {
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
}

void TurnON_Launchpad_Right_Green_LED() {
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
}

void TurnOFF_Launchpad_Right_Green_LED() {
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
}

// 3 functions for rigth blue LED on launchpad
void Toggle_Launchpad_Right_Blue_LED() {
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
}

void TurnON_Launchpad_Right_Blue_LED() {
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
}

void TurnOFF_Launchpad_Right_Blue_LED() {
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
}

