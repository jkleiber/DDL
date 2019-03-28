//I2C control registers
#define PINSEL1 (*(volatile int *)0x4002C004)
#define I2C_SCLL (*(volatile int *)0x4001C014)
#define I2C_SCLH (*(volatile int *)0x4001C010)
#define I2C_CONCLR (*(volatile int *)0x4001C018)
#define I2C_CONSET (*(volatile int *)0x4001C000)
#define I2C_DAT (*(volatile int *)0x4001C008)
#define I2C_STAT (*(volatile int *)0x4001C004)
#define ISER (*(volatile int *) 0xE000E100)
#define PCLKSEL0 (*(volatile int *)0x400FC1A8)


//I2C macros
#define I2C_FEED() I2C_CONCLR = (1<<3); while(((I2C_CONSET>>3) & 1) == 0){}     //wait for bus to be available


//GPIO registers
#define FIO2DIR (*(volatile int *) 0x2009C040)
#define FIO2PIN (*(volatile int *) 0x2009C054)


//Logical operators
#define TRUE 1
#define FALSE 0


//I2C addresses and registers
#define IODIRA 0x0      //address of IODIRA on MCP
#define IODIRB 0x1      //address of IODIRB on MCP
#define LM75ADDR 0x48   //address of LM75
#define MCPADDR 0x20    //address of MCP
#define LEFT 0x13       //register of GPIO for left display
#define RIGHT 0x12      //register of GPIO for right display


//Program constants
#define READ 1          //read data bit
#define WRITE 0         //write data bit
#define CELSIUS 0       //celsius mode
#define FARENHEIT 1     //farenheit mode


/* Global variables */
int mode = CELSIUS;     //units to operate in
int stat;               //debugging variable for reading I2C_STAT


/**
 * @brief Wait a number of cycles before proceeding with code execution
 * 
 * @param count Number of cycles to wait
 */
void wait_ticks(int count)
{
    volatile int ticks = count;
    while(ticks>0)
        ticks--;
}


/**
 * @brief Sets up the I2C device as master
 * 
 */
void I2C_init(void)
{
    I2C_CONSET = 0x40;
}


/**
 * @brief Start I2C communication
 * 
 */
void I2C_start(void)
{
    //Take control of the bus
    I2C_CONSET = 1<<3;
    I2C_CONSET |= 1<<5;

    //Once control is held, begin communication
    I2C_FEED();
    I2C_CONCLR = 1<<5;
}


/**
 * @brief Write a value to the bus
 * 
 * @param data The address or data byte to write
 */
void I2C_write(int data)
{
    //Load byte to write, then send when possible
    I2C_DAT = data;
    I2C_FEED();
}


/**
 * @brief Reads the next byte off of I2C, and sends back ACK or NACK appropriately
 * 
 * @param stop Send back a NACK if this is the last desired byte
 * @return int The value read over I2C
 */
int I2C_read(int stop)
{
    //Wait for data, then save the data
    I2C_FEED();
    int data = I2C_DAT;
    
    //If stop, send a NACK
    if(stop)
    {
        I2C_CONCLR = 1<<2;
    }
    //otherwist ACK
    else
    {
        I2C_CONSET = 1<<2;
    }
    
    return data;
}


/**
 * @brief: releases control of the I2C bus
 * 
 */
void I2C_stop(void)
{
    I2C_CONSET = 1<<4;
    I2C_CONCLR = 1<<3;
    while(I2C_CONSET>>4 & 1)
    {}
}


/**
 * @brief: sets the bank bit and SEQOP bits to the ideal values
 * 
 */
void MCP_bank(void)
{
    //Communicate with the MCP
    I2C_start();
    I2C_write(MCPADDR<<1);
    
    //Choose IOCON register
    I2C_start();
    I2C_write(0x0A);
    
    //Write to BANK and SEQOP
    I2C_start();
    I2C_write(0x20);
    
    //Release control
    I2C_stop();
}


/**
 * @brief: sets the GPIOs on the MCP as outputs
 * 
 */
void MCP_DIR(void)
{
    //Set the GPIO A block as an output
    I2C_start();
    I2C_write(MCPADDR<<1);
    I2C_write(IODIRA);
    I2C_write(0x00);

    //Set the GPIO B bloc as an output
    I2C_start();
    I2C_write(MCPADDR<<1);
    I2C_write(IODIRB);
    I2C_write(0x00);

    //Release control of the I2C
    I2C_stop();
}


/**
 * @brief: Writes a number to one of the seven segment displays
 * 
 * @param num: the number to write
 * @param side: the side to write to
 */
void write_Num(int num, int side)
{
    //Begin communication with MCP and choose the correct side
    I2C_start();
    I2C_write(MCPADDR<<1);
    I2C_write(side);

    //Write a number to the GPIO bits we just chose
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

    //End communication with the MCP chip
    I2C_stop();
}


/**
 * @brief: Reads the temperature from the LM75 sensor over I2C
 * 
 * @return: int the temperature sample in the correct units
 */
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


/**
 * @brief: writes a two digit number to two seven segment displays
 * 
 * @param temp: the temeperature to display
 */
void write_big_num(int temp)
{
    //Extract the tens digit and assign it to the left display
    int tens = temp / 10;
    write_Num(tens,LEFT);

    //Extract the ones digit and assign it to the right display
    int ones = temp % 10;
    write_Num(ones,RIGHT);
}


/**
 * @brief: Main function for initializing devices and running main logic loop
 * 
 * @return int: exit status (should not be reached)
 */
int main(void) 
{   
    //Set SCL and SDA0 on PINSEL1
    PINSEL1 |= (1<<22) | (1<<24);
    PCLKSEL0 |= (1<<14);    

    //Configure I2C Frequency
    I2C_SCLL = 5;
    I2C_SCLH = 5;

    //Initialize I2C
    I2C_init();

    //Set up the MCP chip
    MCP_bank();
    MCP_DIR();

    //Make P2.12 an input (for the button)
    FIO2PIN &= ~(1 << 12);

    //Track the temperature and the number of samples
    int temperature;
    int i;

    //Run this code forever
    while(1)
    {
        //Take 100 samples of temperature and average them
        for(i = 0; i < 100; ++i)
        {
            temperature += read_temperature();
        }
        temperature /= 100;
        
        //Display the temperature on the seven segment displays
        write_big_num(temperature);

        //If the button is pressed, change the mode of operation
        if((FIO2PIN >> 12) & 1)
        {
            //Toggle farenheit and celsius
            mode = mode == CELSIUS ? FARENHEIT : CELSIUS;

            //Avoid reading switch bounce
            wait_ticks(8000);
            while((FIO2PIN >> 12) & 1){}
        }
    }
}
