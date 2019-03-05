#define PINSEL1 (*(volatile int *)0x4002C004)
#define I2C_SCLL (*(volatile int *)0x4001C014)
#define I2C_SCLH (*(volatile int *)0x4001C010)
#define I2C_CONCLR (*(volatile int *)0x4001C018)
#define I2C_CONSET (*(volatile int *)0x4001C000)
#define I2C_DAT (*(volatile int *)0x4001C008)
#define I2C_STAT (*(volatile int *)0x4001C004)
#define I2C_FEED() I2C_CONCLR = (1<<3); while(((I2C_CONSET>>3) & 1) == 0){}
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

#define MAX_WRITE_BUF 50
#define MAX_READ_BUF 50

/* Global variables */
int mode = FARENHEIT;
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

void I2C_write(int data, int stop)
{
    switch(I2C_DAT)
    {
        //Slave bus error?
        case 0x00:
            I2C_CONSET = 0x14;
            I2C_CONCLR = 0x08;
            break;
        //Master data transmission after START
        case 0x08:
        case 0x10:
            I2C_DAT = data;
            I2C_CONSET = 0x04;  //set AA bit
            I2C_CONCLR = 0x08;  //clear SI flag
            break;
        //Transmit data to slave device
        case 0x18:
            I2C_DAT = data;
            I2C_CONSET = 0x04;  //set AA bit
            I2C_CONCLR = 0x08;  //clear SI flag
            break;
        //Stop writing due to NACK
        case 0x20:
        case 0x30:
            I2C_CONSET = 0x14;  //set AA and STO bits
            I2C_CONCLR = 0x08;  //clear SI flag
            break;
        //Keep writing (or stop)
        case 0x28:
            //If nothing else needs to be written, stop writing
            if(stop)
            {
                I2C_CONSET = 0x14;  //set AA and STO bits
                I2C_CONCLR = 0x08;  //clear SI flag
            }
            //Otherwise, keep writing
            else
            {
                I2C_DAT = data;
                I2C_CONSET = 0x04;  //set AA bit
                I2C_CONCLR = 0x08;  //clear SI flag
            }
            break;
        //Lost comms, send start to revive when possible
        case 0x38:
            I2C_CONSET = 0x24;
            I2C_CONCLR = 0x08;
            break; 
        //Start I2C if needed
        default:
            break;
    }
}


int I2C_read(int stop)
{
    /* Define local variables */
    int data;

    //Initialize data to -1
    data = -1;

    switch(I2C_DAT)
    {
        //READ
        //Prepare for data to be read in
        case 0x40:
            I2C_CONSET = 0x04;
            I2C_CONCLR = 0x08;
            break;
        //Slave rejected read request
        case 0x48:
            I2C_CONSET = 0x14;
            I2C_CONCLR = 0x08;
            break;
        //Read data. Choose to continue or not
        case 0x50:
            //Read next data off the buffer
            data = I2C_DAT;
            
            //If the whole message has been read, we are done
            if(stop)
            {
                I2C_CONCLR = 0x0C;
            }
            //Otherwise keep reading
            else
            {
                I2C_CONSET = 0x04;
                I2C_CONCLR = 0x08;
            }
            break;

        case 0x58:
            //Read next data off the buffer
            data = I2C_DAT;
            
            //Stop transmission
            I2C_CONSET = 0x14;
            I2C_CONCLR = 0x08;
            break;
        default:
            break;
    }
    
    return data;
}

/*
void I2C_start_read(int slave_addr, int len)
{
    I2C_CONSET = 0x20;
    //while(I2C_STAT != 0x08 && I2C_STAT != 0x10){}
    I2C_write(slave_addr, FALSE);
}


void I2C_start_write(int slave_addr)
{
    I2C_CONSET = 0x20;
    //while(I2C_STAT != 0x08 && I2C_STAT != 0x10){}
    I2C_write(slave_addr, FALSE);
}*/

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
    I2C_write(MCPADDR<<1, FALSE);
    I2C_write(0x0A, FALSE);
    I2C_write(0x20, TRUE);
    I2C_stop();
}

void MCP_DIR(void)
{
    I2C_start();
    I2C_write(MCPADDR<<1, FALSE);
    I2C_write(IODIRA, FALSE);
    I2C_write(0x00, TRUE);

    I2C_start();
    I2C_write(MCPADDR<<1, FALSE);
    I2C_write(IODIRB, FALSE);
    I2C_write(0x00, TRUE);

    I2C_stop();
}

void write_Num(int num, int side)
{
    //Write to MCPADDR
    I2C_start();
    I2C_write(MCPADDR<<1, FALSE);
    I2C_write(side, FALSE);

    switch(num)
    {
        default:
            I2C_write(0, TRUE);
            break;
        case 0:
            I2C_write(0b111111, TRUE);
            break;
        case 1:
            I2C_write(0b0000110, TRUE);
            break;
        case 2:
            I2C_write(0b1011011, TRUE);
            break;
        case 3:
            I2C_write(0b1001111, TRUE);
            break;
        case 4:
            I2C_write(0b1100110, TRUE);
            break;
        case 5:
            I2C_write(0b1101101, TRUE);
            break;
        case 6:
            I2C_write(0b1111101, TRUE);
            break;
        case 7:
            I2C_write(0b0000111, TRUE);
            break;
        case 8:
            I2C_write(0b1111111, TRUE);
            break;
        case 9:
            I2C_write(0b1100111, TRUE);
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

    //Notify temperature sensor of write operation (to select correct register)
    I2C_start();
    I2C_write(LM75ADDR << 1, FALSE);

    //Select temperature register 0
    I2C_write(0, TRUE);

    //Notify temperature sensor of impending read operation
    I2C_start();
    I2C_write((LM75ADDR << 1) | READ, FALSE);

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
{
    //Set SCL and SDA0 on PINSEL1
    PINSEL1 |= (1<<22) | (1<<24);
    PCLKSEL0 |= (1<<14);
    //Configure I2C Frequency
    I2C_SCLL = 3;
    I2C_SCLH = 3;

    I2C_interrupt();
    //Initialize I2C
    I2C_init();
    MCP_bank();
    MCP_DIR();

    int temperature;

    while(1)
    {
        temperature = read_temperature();
        write_big_num(temperature);
    }
}
