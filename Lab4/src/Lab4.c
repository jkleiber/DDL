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
