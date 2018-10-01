// Lab 2
// ECE 2534
// Brian Worek

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdio.h>
#include <stdlib.h>
#include <LED_HAL.h>
#include <Timer_HAL.h>


#define EGG_MAX_YEARS 0
#define CHILD_MAX_YEARS 7

#define RADIUS_EGG 10
#define RADIUS_CHILD 11
#define RADIUS_ADULT 13

#define HALF_CHILD 6
#define THIRD_CHILD 4
#define HALF_ADULT 7
#define THIRD_ADULT 4

#define RADIUS_3 3
#define RADIUS_2 2
#define RADIUS_1 1

#define COL_0 16
#define COL_1 32
#define COL_2 48

#define ROW_0 32
#define ROW_1 48
#define ROW_2 64

#define CENTER_X 64
#define TOP_Y 48
#define BOTTOM_Y 64
#define LEFT_X 32
#define RIGHT_X 96

#define PLAY_TARGET 7

// Global parameters with current application settings

typedef enum {egg, child, adult, dead} age_t;
typedef enum {baud9600, baud19200, baud38400, baud57600} UARTBaudRate_t;

UARTBaudRate_t currentBaudRate;

typedef struct Tamagotchi
{
    age_t age;
    int years;
    int x;
    int y;
    int energy;
    int happy;
    int playCount;
} Tamagotchi;



//-----------------------------------------------------------------------
// Character Graphics API
//
// The 128*128 pixel screen is partitioned in a grid of 8 rows of 16 characters
// Each character is a plotted in a rectangle of 8 pixels (wide) by 16 pixels (high)
//
// The lower-level graphics functions are taken from the Texas Instruments Graphics Library
//
//            C Application        (this file)
//                   |
//                 GRLIB           (graphics library)
//                   |
//             CrystalFontz Driver (this project, LcdDriver directory)
//                   |
//                font data        (this project, fonts directory)

Graphics_Context g_sContext;

void InitGraphics() {
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&g_sContext,
                         &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_ROYAL_BLUE);
    GrContextFontSet(&g_sContext, &g_sFontCmtt16);
    Graphics_clearDisplay(&g_sContext);
}

void LCDClearDisplay() {
    Graphics_clearDisplay(&g_sContext);
}

void LCDDrawChar(unsigned row, unsigned col, int8_t c) {
    Graphics_drawString(&g_sContext,
                        &c,
                        1,
                        8 * (col % 16),
                        16 * (row % 8),
                        OPAQUE_TEXT);
}

//------------------------------------------
// UART API
//
// We are using EUSCI_A0, which is a user UART in MSP432 that is accessible
// on your laptop as 'XDS 110 Class Application/User UART'. It usually shows up as COM3.
//
// The low-level UART functions are taken from the MSP432 Driverlib, Chapter 24.
//
// The Baud Rate Computation Procedure is taken from the
// User Guide MSP432P4 Microcontroller, Chapter 24, page 915
//
//  Baud rate computation:
//  - System Clock SMCLK in MSP432P4 is 3MHz (Default)
//  - Baud Rate Division Factor N = 3MHz / Baudrate
//      Eg. N9600 = 30000000 / 9600 = 312.5
//  - If N>16 -> oversampling mode
//      Baud Rate Divider              UCBF  = FLOOR(N/16)     = INT(312.5/16)     = 19
//      First modulation stage select  UCBRF = FRAC(N/16) * 16 = FRAC(312.5/16)*16 = 8


eUSCI_UART_Config uartConfig = {
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,               // SMCLK Clock Source = 3MHz
     19,                                           // UCBR   = 19
     8,                                            // UCBRF  = 8
     0xAA,                                         // UCBRS  = 0xAA
     EUSCI_A_UART_NO_PARITY,                       // No Parity
     EUSCI_A_UART_LSB_FIRST,                       // LSB First
     EUSCI_A_UART_ONE_STOP_BIT,                    // One stop bit
     EUSCI_A_UART_MODE,                            // UART mode
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION // Oversampling
};

void InitUART() {
    UART_initModule(EUSCI_A0_BASE, &uartConfig);
    UART_enableModule(EUSCI_A0_BASE);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
        GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
}

void WriteBR(){
    char text[16] = "";
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    sprintf(text, "%u", currentBaudRate);
    Graphics_drawString(&g_sContext, (int8_t *) text, -1, 106, 0, true);
}

void UARTSetBaudRate(UARTBaudRate_t t) {
    currentBaudRate = t;
    switch (currentBaudRate){
    case baud9600:
        uartConfig.clockPrescalar = 19;
        uartConfig.firstModReg = 8;
        uartConfig.secondModReg = 0xAA;
        break;
    case baud19200:
        uartConfig.clockPrescalar = 9;
        uartConfig.firstModReg = 12;
        uartConfig.secondModReg = 0x44;
        break;
    case baud38400:
        uartConfig.clockPrescalar = 4;
        uartConfig.firstModReg = 14;
        uartConfig.secondModReg = 0x10;
        break;
    case baud57600:
        uartConfig.clockPrescalar = 3;
        uartConfig.firstModReg = 4;
        uartConfig.secondModReg = 0x04;
        break;
    }

    InitUART();
}

void UARTToggleBaudRate() {
    switch (currentBaudRate){
    case baud9600:
        UARTSetBaudRate(baud19200);
        break;
    case baud19200:
        UARTSetBaudRate(baud38400);
        break;
    case baud38400:
        UARTSetBaudRate(baud57600);
        break;
    case baud57600:
        UARTSetBaudRate(baud9600);
        break;
    }
    WriteBR();
}

bool UARTHasChar() {
    return (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
                == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);
}

uint8_t UARTGetChar() {
    if (UARTHasChar())
        return UART_receiveData(EUSCI_A0_BASE);
    else
        return 0;
}

bool UARTCanSend() {
    return (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
                == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG);
}

void UARTPutChar(uint8_t t) {
    while (!UARTCanSend()) ;
    UART_transmitData(EUSCI_A0_BASE,t);
}

//------------------------------------------
// Red LED API
void InitRedLED() {
    GPIO_setAsOutputPin    (GPIO_PORT_P1,    GPIO_PIN0);   // red LED on Launchpad
    GPIO_setOutputLowOnPin (GPIO_PORT_P1,    GPIO_PIN0);
}

void RedLEDToggle() {
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

//------------------------------------------
// Display API
void ClearDisplay() {
    LCDClearDisplay();
}

//-----------------------------------------------------------------------
// TAMAGOTCHI FUNCTIONS

// Egg Drawing
void DrawTamagotchiEgg(Tamagotchi *game_tama){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_fillCircle(&g_sContext, game_tama->x, game_tama->y, RADIUS_EGG);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_ORANGE);
    Graphics_fillCircle(&g_sContext, game_tama->x-4 , game_tama->y-5, RADIUS_3);
    Graphics_fillCircle(&g_sContext, game_tama->x-1 , game_tama->y+8, RADIUS_2);
    Graphics_fillCircle(&g_sContext, game_tama->x+4 , game_tama->y+1, RADIUS_2);
}

void DrawChildEars(Tamagotchi *game_tama){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_NAVY);
    Graphics_Rectangle left_ear = {game_tama->x-7,
                                      game_tama->y-RADIUS_CHILD-4,
                                      game_tama->x-3,
                                      game_tama->y-RADIUS_CHILD+2};
    Graphics_fillRectangle(&g_sContext, &left_ear);

    Graphics_Rectangle right_ear = {game_tama->x+3,
                                      game_tama->y-RADIUS_CHILD-4,
                                      game_tama->x+7,
                                      game_tama->y-RADIUS_CHILD+2};
    Graphics_fillRectangle(&g_sContext, &right_ear);
}

void DrawChildEyes(Tamagotchi *game_tama){
    //Left
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_fillCircle(&g_sContext, game_tama->x-HALF_CHILD, game_tama->y-THIRD_CHILD+1, RADIUS_2);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_fillCircle(&g_sContext, game_tama->x-HALF_CHILD+1, game_tama->y-THIRD_CHILD+1, RADIUS_1);
    //Right
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_fillCircle(&g_sContext, game_tama->x+HALF_CHILD, game_tama->y-THIRD_CHILD+1, RADIUS_2);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_fillCircle(&g_sContext, game_tama->x+HALF_CHILD+1, game_tama->y-THIRD_CHILD+1, RADIUS_1);
}

void DrawChildMouth(Tamagotchi *game_tama){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_NAVY);
    Graphics_Rectangle mouth = {game_tama->x-HALF_CHILD+1,
                                game_tama->y+HALF_CHILD,
                                game_tama->x+HALF_CHILD-1,
                                game_tama->y+HALF_CHILD};
    Graphics_fillRectangle(&g_sContext, &mouth);
    Graphics_Rectangle mouthLeft = { game_tama->x-HALF_CHILD+1,
                                     game_tama->y+HALF_CHILD-2,
                                     game_tama->x-HALF_CHILD+1,
                                     game_tama->y+HALF_CHILD};
    Graphics_fillRectangle(&g_sContext, &mouthLeft);
    Graphics_Rectangle mouthRight = {game_tama->x+HALF_CHILD-1,
                                     game_tama->y+HALF_CHILD-2,
                                     game_tama->x+HALF_CHILD-1,
                                     game_tama->y+HALF_CHILD};
    Graphics_fillRectangle(&g_sContext, &mouthRight);
}

void DrawChildArms(Tamagotchi *game_tama){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_LIME_GREEN);
    Graphics_Rectangle leftArm = { game_tama->x-RADIUS_CHILD-3,
                                     game_tama->y+2,
                                     game_tama->x-RADIUS_CHILD+2,
                                     game_tama->y+6};
    Graphics_fillRectangle(&g_sContext, &leftArm);
    Graphics_Rectangle rightArm = { game_tama->x+RADIUS_CHILD-2,
                                     game_tama->y+2,
                                     game_tama->x+RADIUS_CHILD+3,
                                     game_tama->y+6};
    Graphics_fillRectangle(&g_sContext, &rightArm);
}

void DrawChildLegs(Tamagotchi *game_tama){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_LIME_GREEN);
    Graphics_Rectangle leftArm = { game_tama->x-5,
                                   game_tama->y+RADIUS_CHILD,
                                   game_tama->x-2,
                                   game_tama->y+RADIUS_CHILD+4};
    Graphics_fillRectangle(&g_sContext, &leftArm);
    Graphics_Rectangle rightArm = { game_tama->x+2,
                                    game_tama->y+RADIUS_CHILD,
                                    game_tama->x+5,
                                    game_tama->y+RADIUS_CHILD+4};
    Graphics_fillRectangle(&g_sContext, &rightArm);
}

//Child Drawing
void DrawTamagotchiChild(Tamagotchi *game_tama){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_LIME_GREEN);
    Graphics_fillCircle(&g_sContext, game_tama->x, game_tama->y, RADIUS_CHILD);

    DrawChildEars(game_tama);
    DrawChildEyes(game_tama);
    DrawChildMouth(game_tama);
    DrawChildArms(game_tama);
    DrawChildLegs(game_tama);
}

// Adult Drawaing
void DrawAdultEars(Tamagotchi *game_tama){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_NAVY);
    Graphics_Rectangle left_ear = {game_tama->x-7,
                                      game_tama->y-RADIUS_ADULT-4,
                                      game_tama->x-3,
                                      game_tama->y-RADIUS_ADULT+2};
    Graphics_fillRectangle(&g_sContext, &left_ear);

    Graphics_Rectangle right_ear = {game_tama->x+3,
                                      game_tama->y-RADIUS_ADULT-4,
                                      game_tama->x+7,
                                      game_tama->y-RADIUS_ADULT+2};
    Graphics_fillRectangle(&g_sContext, &right_ear);
}

void DrawAdultEyes(Tamagotchi *game_tama){
    //Left
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_fillCircle(&g_sContext, game_tama->x-HALF_CHILD, game_tama->y-THIRD_ADULT+1, RADIUS_2);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_fillCircle(&g_sContext, game_tama->x-HALF_CHILD+1, game_tama->y-THIRD_ADULT+1, RADIUS_1);
    //Right
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_fillCircle(&g_sContext, game_tama->x+HALF_ADULT, game_tama->y-THIRD_ADULT+1, RADIUS_2);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_fillCircle(&g_sContext, game_tama->x+HALF_ADULT+1, game_tama->y-THIRD_ADULT+1, RADIUS_1);
}

void DrawAdultMouth(Tamagotchi *game_tama){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_NAVY);
    Graphics_Rectangle mouth = {game_tama->x-HALF_ADULT+1,
                                game_tama->y+HALF_ADULT,
                                game_tama->x+HALF_ADULT-1,
                                game_tama->y+HALF_ADULT};
    Graphics_fillRectangle(&g_sContext, &mouth);
    Graphics_Rectangle mouthLeft = { game_tama->x-HALF_ADULT+1,
                                     game_tama->y+HALF_ADULT,
                                     game_tama->x-HALF_ADULT+1,
                                     game_tama->y+HALF_ADULT+2};
    Graphics_fillRectangle(&g_sContext, &mouthLeft);
    Graphics_Rectangle mouthRight = {game_tama->x+HALF_ADULT-1,
                                     game_tama->y+HALF_ADULT,
                                     game_tama->x+HALF_ADULT-1,
                                     game_tama->y+HALF_ADULT+2};
    Graphics_fillRectangle(&g_sContext, &mouthRight);
}

void DrawAdultArms(Tamagotchi *game_tama){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    Graphics_Rectangle leftArm = { game_tama->x-RADIUS_ADULT-3,
                                     game_tama->y+2,
                                     game_tama->x-RADIUS_ADULT+2,
                                     game_tama->y+6};
    Graphics_fillRectangle(&g_sContext, &leftArm);
    Graphics_Rectangle rightArm = { game_tama->x+RADIUS_ADULT-2,
                                     game_tama->y+2,
                                     game_tama->x+RADIUS_ADULT+3,
                                     game_tama->y+6};
    Graphics_fillRectangle(&g_sContext, &rightArm);
}

void DrawAdultLegs(Tamagotchi *game_tama){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    Graphics_Rectangle leftArm = { game_tama->x-5,
                                   game_tama->y+RADIUS_ADULT,
                                   game_tama->x-2,
                                   game_tama->y+RADIUS_ADULT+4};
    Graphics_fillRectangle(&g_sContext, &leftArm);
    Graphics_Rectangle rightArm = { game_tama->x+2,
                                    game_tama->y+RADIUS_ADULT,
                                    game_tama->x+5,
                                    game_tama->y+RADIUS_ADULT+4};
    Graphics_fillRectangle(&g_sContext, &rightArm);
}

DrawTamagotchiAdult(Tamagotchi *game_tama){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    Graphics_fillCircle(&g_sContext, game_tama->x, game_tama->y, RADIUS_ADULT);

    DrawAdultEars(game_tama);
    DrawAdultEyes(game_tama);
    DrawAdultMouth(game_tama);
    DrawAdultArms(game_tama);
    DrawAdultLegs(game_tama);
}

void DrawMeterNames(){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_ROYAL_BLUE);

    char text[16] = "";

    sprintf(text, "Age:");
    Graphics_drawString(&g_sContext, (int8_t *) text, -1, 0, 0, true);
    sprintf(text, "BR:");
    Graphics_drawString(&g_sContext, (int8_t *) text, -1, 80, 0, true);
    sprintf(text, "Energy:");
    Graphics_drawString(&g_sContext, (int8_t *) text, -1, 0, 96, true);
    sprintf(text, "Happy:");
    Graphics_drawString(&g_sContext, (int8_t *) text, -1, 0, 112, true);
}

void WriteAge(Tamagotchi *game_tama){
    char text[16] = "";
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    sprintf(text, "%u", game_tama->years);
    Graphics_drawString(&g_sContext, (int8_t *) text, -1, 36, 0, true);
}

void WriteEnergyMeter(Tamagotchi *game_tama){
    char text[16] = "";

    if (game_tama->energy == 0){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        sprintf(text, "_____");
    }
    else if (game_tama->energy == 1){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        sprintf(text, "*____");
    }
    else if (game_tama->energy == 2){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
        sprintf(text, "**___");
    }
    else if (game_tama->energy == 3){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
        sprintf(text, "***__");
    }
    else if (game_tama->energy == 4){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_LIME_GREEN);
        sprintf(text, "****_");
    }
    else if (game_tama->energy == 5){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_LIME_GREEN);
        sprintf(text, "*****");
    }
    Graphics_drawString(&g_sContext, (int8_t *) text, -1, 64, 96, true);
}

void WriteHappyMeter(Tamagotchi *game_tama){
    char text[16] = "";

    if (game_tama->happy == 0){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        sprintf(text, "_____");
    }
    else if (game_tama->happy == 1){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        sprintf(text, "*____");
    }
    else if (game_tama->happy == 2){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
        sprintf(text, "**___");
    }
    else if (game_tama->happy == 3){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
        sprintf(text, "***__");
    }
    else if (game_tama->happy == 4){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_LIME_GREEN);
        sprintf(text, "****_");
    }
    else if (game_tama->happy == 5){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_LIME_GREEN);
        sprintf(text, "*****");
    }
    Graphics_drawString(&g_sContext, (int8_t *) text, -1, 64, 112, true);
}

//Draw the tamagotchi with the correct age
void DrawTamagotchi(Tamagotchi *game_tama){
    if (game_tama->age == egg){
        DrawTamagotchiEgg(game_tama);
    }
    else if (game_tama->age == child){
        DrawTamagotchiChild(game_tama);
    }
    else if (game_tama->age == adult){
        DrawTamagotchiAdult(game_tama);
    }
}

//Erases the Tamagotchi from its current position.
void EraseTamagotchi(Tamagotchi *game_tama){
    if (game_tama->age == adult){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_ROYAL_BLUE);
        Graphics_Rectangle erasureRect = { game_tama->x-18,
                                            game_tama->y-17,
                                            game_tama->x+18,
                                            game_tama->y+17};
        Graphics_fillRectangle(&g_sContext, &erasureRect);
    }
    else if (game_tama->age == child){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_ROYAL_BLUE);
        Graphics_Rectangle erasureRect = { game_tama->x-18,
                                            game_tama->y-15,
                                            game_tama->x+18,
                                            game_tama->y+16};
        Graphics_fillRectangle(&g_sContext, &erasureRect);
    }
    else if (game_tama->age == egg){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_ROYAL_BLUE);
        Graphics_fillCircle(&g_sContext, game_tama->x, game_tama->y, RADIUS_EGG);
    }
}

DrawHorizontalBorder(int y_pos){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BURLY_WOOD);
    int i;
    for (i = 1; i < 128; i=i+8){
        Graphics_Rectangle fencePost = {i, y_pos, i + 3, y_pos + 10};
        Graphics_fillRectangle(&g_sContext, &fencePost);
    }
}

DrawVerticalBorder(int x_pos){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BURLY_WOOD);
    Graphics_Rectangle fencePostLong = {x_pos, 28, x_pos + 3, 84};
    Graphics_fillRectangle(&g_sContext, &fencePostLong);
}

//Draws borders around the Tamagotchi play area
DrawBorders(){
    DrawHorizontalBorder(18);
    DrawHorizontalBorder(84);
    DrawVerticalBorder(1);
    DrawVerticalBorder(121);
}

DrawTombstone(){
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    ClearDisplay();

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GRAY);
    Graphics_Rectangle rect = { 64-25,
                                64-20,
                                64+25,
                                64+40};
    Graphics_fillRectangle(&g_sContext, &rect);
    Graphics_fillCircle(&g_sContext, 64, 64-20, 25);
}

EngraveTombstone(){
    char text[16] = "";
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_GRAY);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    sprintf(text, "RIP");
    Graphics_drawString(&g_sContext, (int8_t *) text, -1, 48, 56, true);
}

//Clear and redraw the whole display
void UpdateWholeDisplay(Tamagotchi *game_tama) {
    ClearDisplay();
    DrawMeterNames();
    DrawBorders();
    DrawTamagotchi(game_tama);
    WriteAge(game_tama);
    WriteBR();
    WriteEnergyMeter(game_tama);
    WriteHappyMeter(game_tama);
}

int IsAlive(Tamagotchi *game_tama){
    if (game_tama->energy==0 && game_tama->happy==0)
        return 0;
    else
        return 1;
}

void DisplayDeathScreen(){
    DrawTombstone();
    EngraveTombstone();
}

void TamagotchiDie(Tamagotchi *game_tama){
    if (game_tama->age != dead){
        DisplayDeathScreen();
        game_tama->age = dead;
    }
}

void DecrementEnergy(Tamagotchi *game_tama){
    if (game_tama->energy > 0){
        game_tama->energy--;
        WriteEnergyMeter(game_tama);
    }
}

void DecrementHappy(Tamagotchi *game_tama){
    if (game_tama->happy > 0){
        game_tama->happy--;
        WriteHappyMeter(game_tama);
    }
}

void IncrementEnergy(Tamagotchi *game_tama){
    if (game_tama->energy < 5){
        game_tama->energy++;
        WriteEnergyMeter(game_tama);
    }
}

void IncrementHappy(Tamagotchi *game_tama){
    if (game_tama->happy < 5){
        game_tama->happy++;
        WriteHappyMeter(game_tama);
    }
}

MoveLeft(Tamagotchi *game_tama){
    if (game_tama->x == RIGHT_X){
        game_tama->playCount++;
        EraseTamagotchi(game_tama);
        game_tama->x = CENTER_X;
        DrawTamagotchi(game_tama);
    }
    else if (game_tama->x == CENTER_X){
        game_tama->playCount++;
        EraseTamagotchi(game_tama);
        game_tama->x = LEFT_X;
        DrawTamagotchi(game_tama);
    }
}

MoveRight(Tamagotchi *game_tama){
    if (game_tama->x == LEFT_X){
        game_tama->playCount++;
        EraseTamagotchi(game_tama);
        game_tama->x = CENTER_X;
        DrawTamagotchi(game_tama);
    }
    else if (game_tama->x == CENTER_X){
        game_tama->playCount++;
        EraseTamagotchi(game_tama);
        game_tama->x = RIGHT_X;
        DrawTamagotchi(game_tama);
    }
}

MoveUp(Tamagotchi *game_tama){
    if (game_tama->y == BOTTOM_Y){
        game_tama->playCount++;
        EraseTamagotchi(game_tama);
        game_tama->y = TOP_Y;
        DrawTamagotchi(game_tama);
    }
}

MoveDown(Tamagotchi *game_tama){
    if (game_tama->y == TOP_Y){
        game_tama->playCount++;
        EraseTamagotchi(game_tama);
        game_tama->y = BOTTOM_Y;
        DrawTamagotchi(game_tama);
    }
}

void GrowOlder(Tamagotchi *game_tama){
    game_tama->years++;
    WriteAge(game_tama);

    if (game_tama->age == egg && game_tama->years > EGG_MAX_YEARS){
        EraseTamagotchi(game_tama);
        game_tama->age = child;
        game_tama->x = CENTER_X;
        DrawTamagotchi(game_tama);
    }
    else if (game_tama->age == child && game_tama->years > CHILD_MAX_YEARS && game_tama->energy > 3 && game_tama->happy > 3){
        EraseTamagotchi(game_tama);
        game_tama->age = adult;
        DrawTamagotchi(game_tama);
    }
}

//Process the character received from UART
void processChar(uint8_t c, Tamagotchi *game_tama) {
    UARTPutChar(c);

    if (c == 'r')
        UARTToggleBaudRate();
    else if (c == 'f')
        IncrementEnergy(game_tama);
    else if (game_tama->energy > 0){
        if (c == 'w')
            MoveUp(game_tama);
        else if (c == 'a')
            MoveLeft(game_tama);
        else if (c == 's')
            MoveDown(game_tama);
        else if (c == 'd')
            MoveRight(game_tama);

        if (game_tama->playCount >= PLAY_TARGET){
            IncrementHappy(game_tama);
            DecrementEnergy(game_tama);
            game_tama->playCount = 0;
        }
    }


}

void SetupNewGame(Tamagotchi *game_tama){
    game_tama->age = egg;
    game_tama->years = 0;
    game_tama->x = CENTER_X;
    game_tama->y = BOTTOM_Y;
    game_tama->energy = 5;
    game_tama->happy = 5;
    game_tama->playCount = 0;

    UpdateWholeDisplay(game_tama);
    StartOneShot10sTimer();
    StartOneShot5sTimer();
}

//-----------------------------------------------------------------------

int main(void) {
    uint8_t c;
    Tamagotchi myTamagotchi;

    WDT_A_hold(WDT_A_BASE);

    InitGraphics();
    InitUART();
    InitLEDs();
    Init10sTimer();
    Init5sTimer();

    SetupNewGame(&myTamagotchi);

    char hasBeen10s = 0;

    while (1) {

        if (UARTHasChar()) {
            c = UARTGetChar();
            if (myTamagotchi.age != dead)
                processChar(c, &myTamagotchi);
            UARTPutChar(c);
        }

        if (OneShot5sTimerExpired()){
            //Only do stuff if its still alive
            if (IsAlive(&myTamagotchi)){
                //Every 10s, grow older and decrease happy.
                if (hasBeen10s){
                    GrowOlder(&myTamagotchi);
                    DecrementHappy(&myTamagotchi);
                    hasBeen10s = 0;
                }
                else
                    hasBeen10s = 1;
                //Every 5s, decrease energy.
                DecrementEnergy(&myTamagotchi);
            }

            //Check if it died after doing stuff
            if (!IsAlive(&myTamagotchi))
                TamagotchiDie(&myTamagotchi);

            StartOneShot5sTimer();
        }
    }
}
