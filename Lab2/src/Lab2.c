//Declare the GPIO_regs struct
typedef struct GPIO_regs_t
{
    volatile unsigned int FIODIR;
    int rsvd[3];
    volatile unsigned int FIOMASK;
    volatile unsigned int FIOPIN;
    volatile unsigned int FIOSET;
    volatile unsigned int FIOCLR;

} GPIO_regs;

//Define FIO
#define FIO ((GPIO_regs *)0x2009c000)

//Define the PINMODE registers
#define PINMODE0 (*(volatile int *)0x4002C040)
#define PINMODE1 (*(volatile int *)0x4002C044)
#define PINMODE2 (*(volatile int *)0x4002C048)
#define PINMODE3 (*(volatile int *)0x4002C04C)
#define PINMODE4 (*(volatile int *)0x4002C050)

/* Set the port/pin combos used to control the LEDs*/
//Red LEDs located at p00, p01, p06, p07, p08, p09, p018
const int red_ports[] = {0, 0, 0, 0, 0, 0, 0};
const int red_pins[] = {0, 1, 6, 7, 8, 9, 18};

//Green LEDs located at p017, p015, p016, p023, p024
const int green_ports[] = {0, 0, 0, 0, 0};
const int green_pins[] = {17, 15, 16, 23, 24};

//Blue LEDs located at p025, p026, p02
const int blue_ports[] = {0, 0, 0};
const int blue_pins[] = {25, 26, 2};

//Left Button Ports located at p210, p211, p212
const int left_button_ports[] = {2, 2, 2};
const int left_button_pins[] = {10, 11, 12};

//Right Button Ports located at p04, p05, p010
const int right_button_ports[] = {0, 0, 0};
const int right_button_pins[] = {4, 5, 10};

/**
 * bus_out_write - Function that can write a series of bits to a set of pins.
 * The maximum number of bits this function can write is the size of unsigned int
 * 
 * @param bits: bits to set
 * @param *ports: list of ports to set (corresponds to pins)
 * @param *pins: list of pins to alter based on bit list
 * @param num_bits: number of bits to write
 */
void bus_out_write(unsigned int bits, int *ports, int *pins, int num_bits)
{
    for (int i = 0; i < num_bits; i++)
    {
        if ((bits >> i) & i)
        {
            FIO[ports[i]].FIOPIN |= (1 << pins[i]);
        }
        else
        {
            FIO[ports[i]].FIOPIN &= ~(1 << pins[i]);
        }
    }
}

/**
 * read_single - Function that reads a single port and pin combination
 *               and returns the current state
 */
unsigned int read_single(int port, int pin)
{
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
unsigned int bus_in_read(int *ports, int *pins, int num_pins)
{
    //Declare and initialize local variables
    int output = 0;

    //Iterate through each port/pin combo and read data
    for (int i = 0; i < num_pins; ++i)
    {
        //If the pin value is HIGH, set this bit to 1
        if (read_single(ports[i], pins[i]))
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

/**
 * Main function
 */
int main(void)
{
    /* Declare and initialize local variables*/
    int turn = 0;       //tracks the players turns (0 or 1)
    int curButton = -1; //tracks the last button press
    int lastTurn = -1;  //tracks who moved last
    int red = 0;        //tracks how many red LEDs are off using bit shifting
    int green = 0;      //tracks how many green LEDs are off using bit shifting
    int blue = 0;       //tracks how many blue LEDs are off using bit shifting
    int latch = -1;     //triggers to track which butten the player can press on their turn
    int othButton = -1; //

    /* Run the program forever */
    while (1)
    {
        //If it is player 0's turn
        if (turn == 0)
        {
            //
            othButton = 0;

            //
            do
            {
                //
                curButton = bus_in_read(left_button_ports, left_button_pins, 3);

                //TODO: make this a function
                if (curButton == latch || (curButton == 1 && latch == -1))
                {
                    latch = 1;
                    blue <<= 1;
                }
                else if (curButton == latch || (curButton == 2 && latch == -1))
                {
                    latch = 2;
                    green <<= 1;
                }
                else if (curButton == latch || (curButton == 4 && latch == -1))
                {
                    latch = 4;
                    red <<= 1;
                }

                //
                othButton = bus_in_read(right_button_ports, right_button_pins, 3);

            } while (othButton == 0);

            turn = 1;
        }
        else
        {
            //
            othButton = 0;

            //
            do
            {
                //
                curButton = bus_in_read(right_button_ports, right_button_pins, 3);

                //TODO: make this a function
                if (curButton == latch || (curButton == 1 && latch == -1))
                {
                    latch = 1;
                    blue <<= 1;
                }
                else if (curButton == latch || (curButton == 2 && latch == -1))
                {
                    latch = 2;
                    green <<= 1;
                }
                else if (curButton == latch || (curButton == 4 && latch == -1))
                {
                    latch = 4;
                    red <<= 1;
                }

                //
                othButton = bus_in_read(left_button_ports, left_button_pins, 3);

            } while (othButton == 0);

            turn = 1;
        }
    }
}
