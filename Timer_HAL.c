//------------------------------------------
// Timer API (Application Programming Interface)
// Also known as BUTTON HAL (Hardware Abstraction Layer)
// HAL is a specific form of API that designs the interface with a certain hardware



#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

void Init10sTimer(){
    Timer32_initModule(TIMER32_1_BASE,
                       TIMER32_PRESCALER_1,
                       TIMER32_32BIT,
                       TIMER32_PERIODIC_MODE);
}

void StartOneShot10sTimer() {
    Timer32_setCount(TIMER32_1_BASE, 30000000); // 10s second period on 3MHz clock
    Timer32_startTimer(TIMER32_1_BASE, true);
}

int OneShot10sTimerExpired() {
    return (Timer32_getValue(TIMER32_1_BASE) == 0);
}


