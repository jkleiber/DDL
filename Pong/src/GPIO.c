#include "GPIO.h"


/**
 * write_single - Function that writes to a single port and pin combination
 */
void gpio_write_single(const int port, const int pin, const int value)
{
    FIO[port].FIODIR |= (1 << pin);
    
    //If value is not 0, output 1
    if(value)
    {
        FIO[port].FIOPIN |= (1 << pin);
    }
    //Otherwise output 0
    else
    {
        FIO[port].FIOPIN &= ~(1 << pin);
    }
    
}

/**
 * bus_out_write - Function that can write a series of bits to a set of pins.
 * The maximum number of bits this function can write is the size of unsigned int
 * 
 * @param bits: bits to set
 * @param *ports: list of ports to set (corresponds to pins)
 * @param *pins: list of pins to alter based on bit list
 * @param num_bits: number of bits to write
 */
void gpio_bus_out_write(unsigned int bits, const int *ports, const int *pins, int num_bits)
{
    for (int i = 0; i < num_bits; i++)
    {
        FIO[ports[i]].FIODIR |= 1 << pins[i];
        if ((bits >> i) & 1)
        {
            FIO[ports[i]].FIOPIN &= ~(1 << pins[i]);
        }
        else
        {
            FIO[ports[i]].FIOPIN |= (1 << pins[i]);
        }
    }
}

/**
 * read_single - Function that reads a single port and pin combination
 *               and returns the current state
 */
unsigned int gpio_read_single(const int port, const int pin)
{
    FIO[port].FIODIR &= ~(1 << pin);
    return ((FIO[port].FIOPIN >> pin) & 1);
}

/**
 * bus_in_read - Function that can read input from a list of pins.
 * 
 * @param *ports: list of ports to read (corresponds to pins)
 * @param *pins: list of pins to read from
 * @param num_pins: number of pins to read
 * @return: an unsigned int with the readings from the bus
 */
unsigned int gpio_bus_in_read(const int *ports, const int *pins, int num_pins)
{
    //Declare and initialize local variables
    int output = 0;
    int state;

    //Iterate through each port/pin combo and read data
    for (int i = 0; i < num_pins; ++i)
    {
        state = gpio_read_single(ports[i], pins[i]);

        //If the pin value is HIGH, set this bit to 1
        if (state)
        {
            output = (output << 1) | 1;
        }
        //Otherwise set this bit to 0
        else
        {
            output = output << 1;
        }
    }

    return output;
}


/* int read_encoder(void)
{
    int seqA = 0;
    int seqB = 0;
    int left = 0;
    int right = 0;
    for (int i = 0; i < 4; i++)
    {
        int A_val = EXTINT &= 1;
        int B_val = EXTINT &= 0b10;
        
        // Record the A and B signals in seperate sequences
        seqA <<= 1;
        seqA |= A_val;
        
        seqB <<= 1;
        seqB |= B_val;
        
        // Mask the MSB four bits
        seqA &= 0b00001111;
        seqB &= 0b00001111;
        
        // Compare the recorded sequence with the expected sequence
        if (seqA == 0b00001001 && seqB == 0b00000011) {
        //cnt1++;
        left = 1;
        }
        
        if (seqA == 0b00000011 && seqB == 0b00001001) {
        //cnt2++;
        right = 1;
        }
    }
    if(left == 1)
    {
        return 1;
    }
    else if (right == 1)
    {
        return 2;
    }
    else
    {
        return 0;
    }

}
*/

