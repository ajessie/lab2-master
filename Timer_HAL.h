//------------------------------------------
// TIMER API
// Also known as TIMER HAL (Hardware Abstraction Layer)
// HAL is a specific form of API that designs the interface with a certain hardware



// Functions related to 32-bit timer. Notice how we change the name to a generic name.
// Initializes the 32-bit timer, named Timer 1
void Init10sTimer();
void StartOneShot10sTimer();
int OneShot10sTimerExpired();

