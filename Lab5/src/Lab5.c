//Define ADC related registers
#define PCONP (*(volatile int *) 0x400FC0C4)
#define AD0CR (*(volatile int *) 0x40034000)
#define AD0DR0 (*(volatile int *) 0x40034010)
#define AD0INTEN (*(volatile int *) 0x4003400C)
#define AD0STAT (*(volatile int *) 0x40034030)

//Define DAC registers
#define DACR (*(volatile int *) 0x4008C000)

//Clock registers
#define PCLKSEL0 (*(volatile int *) 0x400FC1A8)

//Pin Registers
#define PINMODE1 (*(volatile int *) 0x4002C044)
#define PINSEL1 (*(volatile int *) 0x4002C004)

//GPIO pins
#define FIO0DIR (*(volatile unsigned int *) 0x2009C000)
#define FIO0PIN (*(volatile unsigned int *) 0x2009C014)
#define FIO2DIR (*(volatile unsigned int *) 0x2009C040)
#define FIO2PIN (*(volatile unsigned int *) 0x2009C054)

//Threshold for Light sensor
#define LIGHT_THRESH 1600

/**
 * @brief 
 * 
 * @param count 
 */
void wait_ticks(int count)
{
	volatile int ticks = count;
	while(ticks>0)
		ticks--;
}


void wait_micros(int us)
{
    int ticks = (int)((us - 8.43) / 2.477);
    wait_ticks(ticks);
}

/**
 * @brief 
 * 
 */
void config_adc()
{
    //Set the PCADC bit
    PCONP |= (1 << 12);

    //Set the PDN bit
    AD0CR |= (1 << 21);

    //Set the ADC clock source
    PCLKSEL0 |= (1 << 24);

    //Enable ADC0 pin 0
    PINSEL1 |= (1 << 14);
}


void config_dac()
{
    //Configure the clock source
    PCLKSEL0 |= (1 << 22);

    //Configure the PINSEL registers to enable AOUT
    PINSEL1 |= (1 << 21);
}


int main(void)
{
    /* Declare Local Variables */
    int light_status;
    int doorbell_status;
    int amp;

    //Initialize doorbell_status to -1 to indicate start
    doorbell_status = -1;

    //Set up the ADC device
    config_adc();

    //Set up the DAC device
    config_dac();

    //Configure the LED
    FIO0DIR |= (1 << 22);
    PINSEL1 &= ~(3 << 24);

    //Configure the Button
    FIO2DIR &= ~(1 << 8);

    //Run the program
    while(1)
    {
        //Start ADC conversion
        AD0CR |= (1 << 24);

        // Wait for conversion to complete
        while(!((AD0STAT >> 0) & 1)){}

        //Receive data from ADC
        light_status = ((AD0DR0 >> 4) & (0x7FF));

        //Turn LED on or off based on the ADC reading
        if(light_status <= LIGHT_THRESH)
        {
            FIO0PIN |= (1 << 22);
        }
        else
        {
            FIO0PIN &= ~(1 << 22);
        }
        
        //Read the button
        //If the button is pressed, then start the first bell
        if(((FIO2PIN >> 8) & 1) && doorbell_status <= 0)
        {
            doorbell_status = 1;
        }
        //If the button is un-pressed then start second bell
        else if(!((FIO2PIN >> 8) & 1))
        {
            if(doorbell_status > 0)
            {
                doorbell_status = 0;
            }
        }

        //If the doorbell is on the first ring, ring bell 1 at 659 Hz
        if(doorbell_status == 1)
        {
            //Scale amplitude down over time
            for(amp = 1023; amp >= 0; amp-=4)
            {
                //Run 5 times at the same amplitude before dropping down one increment
                for(int i = 0; i < 2; ++i)
                {
                    DACR = (amp << 6);
                    wait_ticks(301);
                    DACR = 0;
                    wait_ticks(301);
                }
            }

            //End of ring 1
            doorbell_status = 2;
        }
        //Otherwise, ring bell 2 at 523 Hz
        else if(doorbell_status == 0)
        {
            //Scale amplitude down over time
            for(amp = 1023; amp >= 0; amp-=4)
            {
                //Run 5 times at the same amplitude before dropping down one increment
                for(int i = 0; i < 2; ++i)
                {
                    DACR = (amp << 6);
                    wait_ticks(381);
                    DACR = 0;
                    wait_ticks(381);
                }
            }

            //End of ring 2
            doorbell_status = -1;
        }
        
    }

    return 0 ;
}
