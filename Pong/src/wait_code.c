#include "wait_code.h"

// Each tick is 2.4777 usec, and this function has a base runtime of 8.43 usec
// Calculate wait time with:
// t = 2.4777x + 8.43 --> t = usec, x = ticks
void wait_ticks(int count)
{
	volatile int ticks = count;
	while(ticks>0)
		ticks--;
}