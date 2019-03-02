#define PINSEL1 (*(volatile int *)0x4002C004)
#define I2C_SCLL (*(volatile int *)0x4001C014)
#define I2C_SCLH (*(volatile int *)0x4001C010)
#define I2C_CONCLR (*(volatile int *)0x4001C018)
#define I2C_CONSET (*(volatile int *)0x4001C000)
#define I2STAT (*(volatile int *)0x4001C004)
#define I2C_DAT (*(volatile int *)0x4001C008)
#define I2C_FEED() I2C_CONCLR = (1<<3); while(I2C_CONSET>>3 & 1){}
#define LM75ADDR 0x48
#define MCPADDR 0x20
#define READ 1
#define WRITE 0
#define TRUE 1
#define FALSE 0
#define CELSIUS 0
#define FARENHEIT 1

/* Global variables */
int mode = CELSIUS;

void I2C_init(void)
{
    I2C_CONSET = 0x40;
}

void I2C_start(void)
{
    I2C_CONSET = 1<<3;
    I2C_CONSET = 1<<5;
    I2C_FEED();
}

void I2C_write(int data)
{
    I2C_DAT = data;
    I2C_FEED();
}

int I2C_read(int stop)
{
    I2C_FEED();
    int data = I2C_DAT;
    if(stop)
    {
        I2C_CONCLR = 1<<2;
    }
    else
    {
        I2C_CONSET = 1<<2;
    }
    
    return data;
}

void I2C_stop(void)
{
    I2C_CONSET = 1<<4;
    I2C_CONCLR = 1<<3;
    while(I2C_CONSET>>4 & 1)
    {}
}


int read_temperature()
{
    //Declare local variables
    float temperature;
    int temp;
    int msb;
    int lsb;

    //Start I2C transmission
    I2C_start();

    //Notify temperature sensor of write operation (to select correct register)
    I2C_write(LM75ADDR << 1);

    //Select temperature register 0
    I2C_write(0);

    //Start up for the read operation
    I2C_start();

    //Notify temperature sensor of impending read operation
    I2C_write((LM75ADDR << 1) | READ);

    //Now that the temperature is ready to be read, read it in
    msb = I2C_read(FALSE);
    lsb = I2C_read(TRUE);
    
    //Combine the bits, making sure to disregard the 5 most least significant bits of LSB
    temp = (msb << 3) + (lsb >> 5);

    //Calculate the temperature value in celsius
    //If the value is negative...
    if((msb >> 7) & 1)
    {
        //Take the two's complement of temperature and convert to celsius
        temp = ~(temp) + 1;
        temperature = (-temp) * 0.125;
    }
    //If the value is positive, just multiply by 0.125
    else
    {
        temperature = temp * 0.125;
    }
    
    //Determine if a temperature conversion is needed
    if(mode == FARENHEIT)
    {
        temperature = ((9.0 / 5.0) * temperature) + 32.0;
    }

    //Return an integer version of temeperature, rounded up to the nearest ones place.
    return (int)(temperature + 0.5);
}

int main(void) 
{
    //Set SCL and SDA0 on PINSEL1
    PINSEL1 |= (1<<22) | (1<<24);
    
    //Configure I2C Frequency
    I2C_SCLL = 5;
    I2C_SCLH = 5;

    //Initialize I2C
    I2C_init();

    while(1)
    {

    }
}
