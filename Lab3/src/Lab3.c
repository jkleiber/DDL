//Define all the registers we need
#define CCLKCFG (*(volatile int *) 0x400FC104)      //Use this register to divide Fcco to produce the 9 MHz clock
#define CLKSRCSEL (*(volatile int *) 0x400FC10C)    //Set the clock source with this register
#define PLL0CFG (*(volatile int *) 0x400FC084)      //This register is used to set the N and M constants for PLL0
#define PLL0CON (*(volatile int *) 0x400FC080)      //Control PLL0 with this control register
#define PLL0FEED (*(volatile int *) 0x400FC08C)     //Feed sequence needed to allow control of PLL0
#define PLL0STAT (*(volatile int *) 0x400FC088)     //PLL0 status register
#define CLKOUTCFG (*(volatile int *) 0x400FC1C8)

/* Define all program macros */
#define PLL_FEED() PLL0FEED = 0xAA; PLL0FEED = 0x55
#define PLL_DISCONNECT() PLL0CON &= ~(1 << 1); PLL_FEED()
#define PLL_CONNECT() PLL0CON |= (1 << 1); PLL_FEED()
#define PLL_DISABLE() PLL0CON &= ~(1); PLL_FEED()
#define PLL_ENABLE() PLL0CON |= 1; PLL_FEED()
#define PLL_CONFIG(M, N) PLL0CFG |= (M); PLL_FEED(); PLL0CFG |= (N << 16); PLL_FEED() 
#define CCLK_DIV(K) CCLKCFG |= K
#define CLKOUT_ENABLE() CLKOUTCFG |= (1 << 8)
#define CLKOUT_DISABLE() CLKOUTCFG &= ~(1 << 8)

//Output port and pin
#define OUT_PORT 2
#define OUT_PIN 12

int main(void)
{
    //Allow the clock to be output onto P1.27
    CLKOUT_ENABLE();

    //Disconnect PLL0
    PLL_DISCONNECT();

    //Disable PLL0
    PLL_DISABLE();

    //Choose the RC clock for our PLL source
    CLKSRCSEL &= ~(3);

    //Configure PLL0 constants
    PLL_CONFIG(35, 0);

    //Enable PLL0
    PLL_ENABLE();

    //Check for PLL0 to achieve lock
    while(!(PLL0STAT >> 26)) {}

    //Now that PLL0 has locked, choose the correct clock divider configuration
    CCLK_DIV(32);

    //Connect PLL0
    PLL_CONNECT();

    //Run the program infinitely
    while(1){}
}
