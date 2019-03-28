//Define ADC related registers
#define PCONP (*(volatile int *) 0x400FC0C4)
#define AD0CR (*(volatile int *) 0x40034000)
#define AD0DR0 (*(volatile int *) 0x40034010)
#define AD0INTEN (*(volatile int *) 0x4003400C)
#define AD0STAT (*(volatile int *) 0x40034030)

//Clock registers
#define PCLKSEL0 (*(volatile int *) 0x400FC1A8)

//Pin Registers
#define PINMODE1 (*(volatile int *) 0x4002C044)
#define PINSEL1 (*(volatile int *) 0x4002C004)

//GPIO pins
#define FIO0DIR (*(volatile unsigned int *) 0x2009C000)
#define FIO0PIN (*(volatile unsigned int *) 0x2009C014)

//Threshold for Light sensor
#define LIGHT_THRESH 1600

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

    //Set channel 0 to have an interrupt
    //AD0INTEN |= 1;
}

int main(void)
{
    /* Declare Local Variables */
    int light_status;

    //Set up the ADC device
    config_adc();

    //Configure the LED
    FIO0DIR |= (1 << 22);
    PINSEL1 &= ~(3 << 24);
    FIO0PIN |= (1 << 22);

    while(1)
    {
        //Start ADC conversion
        AD0CR |= (1 << 24);

        // Wait for conversion to complete
        while(!((AD0STAT >> 0) & 1)){}

        //Receive data from ADC
        light_status = ((AD0DR0 >> 4) & (0x7FF));

        //Turn LED on or off based on the ADC reading
        if(light_status > LIGHT_THRESH)
        {
            FIO0PIN |= (1 << 22);
        }
        else
        {
            FIO0PIN &= ~(1 << 22);
        }
        

    }

    return 0 ;
}
