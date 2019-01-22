#define FIO0DIR (*(volatile unsigned int *)0x2009c000)
#define FIO0PIN (*(volatile unsigned int *)0x2009c014)

// Each tick is 2.4777 usec, and this function has a base runtime of 8.43 usec
// Calculate wait time with:
// t = 2.4777x + 8.43 --> t = usec, x = ticks
void wait_ticks(int count)
{
	volatile int ticks = count;
	while(ticks>0)
		ticks--;
}

int main(void) {
	FIO0DIR |= (1<<22); // configure LED's port bit as an output

	// run the program forever
	while(1)
	{
		FIO0PIN |= (1<<22); // turn on LED
		wait_ticks(8957);	// for 22.2 ms
		FIO0PIN &= ~(1<<22);// turn off LED
		wait_ticks(17916);	// for 44.4 ms
	}

	return 0;
}
