//------------------------------------------
// TIMER API
// Also known as TIMER HAL (Hardware Abstraction Layer)
// HAL is a specific form of API that designs the interface with a certain hardware



void Init10sTimer();
void StartOneShot10sTimer();
int OneShot10sTimerExpired();

void Init6sTimer();
void StartOneShot6sTimer();
int OneShot6sTimerExpired();
