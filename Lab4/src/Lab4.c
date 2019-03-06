#define PINSEL1 (*(volatile int *)0x4002C004)
#define I2C_SCLL (*(volatile int *)0x4001C014)
#define I2C_SCLH (*(volatile int *)0x4001C010)
#define I2C_CONCLR (*(volatile int *)0x4001C018)
#define I2C_CONSET (*(volatile int *)0x4001C000)
#define I2C_DAT (*(volatile int *)0x4001C008)
#define I2C_STAT (*(volatile int *)0x4001C004)
#define I2C_FEED() I2C_CONCLR = (1<<3); while(((I2C_CONSET>>3) & 1) == 0){}
#define I2STAT (*(volatile int *)0x4001C004)
#define ISER (*(volatile int *) 0xE000E100)
#define PCLKSEL0 (*(volatile int *)0x400FC1AB)
#define IODIRA 0x0
#define IODIRB 0x1
#define LM75ADDR 0x48
#define MCPADDR 0x20
#define READ 1
#define WRITE 0
#define TRUE 1
#define FALSE 0
#define CELSIUS 0
#define FARENHEIT 1
#define LEFT 0x13
#define RIGHT 0x12

#define FIO2DIR (*(volatile int *) 0x2009C040)
#define FIO2PIN (*(volatile int *) 0x2009C054)

/* Global variables */
int mode = CELSIUS;

int stat;

void wait_ticks(int count)
{
    volatile int ticks = count;
    while(ticks>0)
        ticks--;
}

void I2C_init(void)
{
    I2C_CONSET = 0x40;

}

void I2C_start(void)
{
    I2C_CONSET = 1<<3;
    I2C_CONSET |= 1<<5;

    I2C_FEED();
    I2C_CONCLR = 1<<5;
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

void I2C_interrupt(void)
{
//    ISER |= 1<<10;
}

void MCP_bank(void)
{
    I2C_start();
    I2C_write(MCPADDR<<1);
    I2C_start();
    I2C_write(0x0A);
    I2C_start();
    I2C_write(0x20);
    I2C_stop();
}

void MCP_DIR(void)
{
    I2C_start();
    I2C_write(MCPADDR<<1);
    //I2C_start();
    I2C_write(IODIRA);
    //I2C_start();
    I2C_write(0x00);
    I2C_start();
    I2C_write(MCPADDR<<1);
    //I2C_start();
    I2C_write(IODIRB);
    //I2C_start();
    I2C_write(0x00);
    I2C_stop();
}

void write_Num(int num, int side)
{
    //Write to MCPADDR
    I2C_start();
    I2C_write(MCPADDR<<1);
    I2C_write(side);

    switch(num)
    {
        default:
            I2C_write(0x0);
            break;
        case 0:
            I2C_write(0b10111110);
            break;
        case 1:
            I2C_write(0b0000110);
            break;
        case 2:
            I2C_write(0b11011010);
            break;
        case 3:
            I2C_write(0b11001111);
            break;
        case 4:
            I2C_write(0b01100110);
            break;
        case 5:
            I2C_write(0b11101100);
            break;
        case 6:
            I2C_write(0b11111100);
            break;
        case 7:
            I2C_write(0b10000110);
            break;
        case 8:
            I2C_write(0b11111111);
            break;
        case 9:
            I2C_write(0b11101110);
            break;
    }
    I2C_stop();
}

int read_temperature()
{
    //Declare local variables
    float temperature = 0.0f;
    int temp = 0;
    int msb = 0;
    int lsb = 0;

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

    //Release control of I2C
    I2C_stop();
    
    //Combine the bits, making sure to disregard the 5 most least significant bits of LSB
    temp = (msb << 3) | (lsb >> 5);

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

void write_big_num(int temp)
{
    int tens = temp / 10;
    write_Num(tens,LEFT);
    int ones = temp % 10;
    write_Num(ones,RIGHT);
}

int main(void) 
{    //Set SCL and SDA0 on PINSEL1
    PINSEL1 |= (1<<22) | (1<<24);
    PCLKSEL0 |= (1<<14);    
    //Configure I2C Frequency
    I2C_SCLL = 5;
    I2C_SCLH = 5;

    I2C_interrupt();
    //Initialize I2C
    I2C_init();
    MCP_bank();
    MCP_DIR();

    //Make P2.12 an input
    FIO2PIN &= ~(1 << 12);

    int temperature;
    int i;

    while(1)
    {
        for(i = 0; i < 100; ++i)
        {
            temperature += read_temperature();
        }
        temperature /= 100;
        
        write_big_num(temperature);

        if((FIO2PIN >> 12) & 1)
        {
            mode = mode == CELSIUS ? FARENHEIT : CELSIUS;

            wait_ticks(8000);
            while((FIO2PIN >> 12) & 1){}
        }
    }
}
