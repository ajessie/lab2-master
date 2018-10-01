//------------------------------------------
// TIMER API
// Also known as TIMER HAL (Hardware Abstraction Layer)
// HAL is a specific form of API that designs the interface with a certain hardware



void Init10sTimer();
void StartOneShot10sTimer();
int OneShot10sTimerExpired();

void Init5sTimer();
void StartOneShot5sTimer();
int OneShot5sTimerExpired();
