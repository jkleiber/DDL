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
#define TRUE 1
#define FALSE 0

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

int red = 0;        //tracks how many red LEDs are off using bit shifting
int green = 0;      //tracks how many green LEDs are off using bit shifting
int blue = 0;       //tracks how many blue LEDs are off using bit shifting
int latch = -1;     //triggers to track which butten the player can press on their turn

// Each tick is 2.4777 usec, and this function has a base runtime of 8.43 usec
// Calculate wait time with:
// t = 2.4777x + 8.43 --> t = usec, x = ticks
void wait_ticks(int count)
{
	volatile int ticks = count;
	while(ticks>0)
		ticks--;
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
void bus_out_write(unsigned int bits, const int *ports, const int *pins, int num_bits)
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
unsigned int read_single(const int port, const int pin)
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
unsigned int bus_in_read(const int *ports, const int *pins, int num_pins)
{
    //Declare and initialize local variables
    int output = 0;
    int state;

    //Iterate through each port/pin combo and read data
    for (int i = 0; i < num_pins; ++i)
    {
        state = read_single(ports[i], pins[i]);

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

/**
 * buttonPress - given a button press, determine if a light should be turned off
 * 
 * @param curButton: the code of the button that just got pressed
 * @param turn: whose turn is it
 * @param reset: reset the turn and allow all buttons to be pressed
 */
void buttonPress(int curButton, int turn, int reset)
{
    //Declare local variables
    int port;
    int pin;

    // Check to see if the first button was pressed (if it is allowed to be)
    if ((curButton == latch && latch == 1) || (curButton == 1 && (latch == -1 || reset)))
    {
        wait_ticks(9000); //wait about 22 msec
        
        //Get port and pin from player turn
        port = turn == 0 ? left_button_ports[2] : right_button_ports[2];
        pin = turn == 0 ? left_button_pins[2] : right_button_pins[2]; 
        
        //Prevent switch bounce
        if(read_single(port,pin))
        {
            //Lock player in to blue LEDs and turn a blue LED off
            latch = 1;
            if(blue < 7)
            {
                blue = (blue << 1) | 1;
                bus_out_write(blue, blue_ports, blue_pins, 3);
            }

            //Wait for player to stop pressing button
            while(read_single(port,pin))
            {
                wait_ticks(8000);
            }
        }
    }
    // Check to see if the second button was pressed (if it is allowed to be)
    else if ((curButton == latch && latch == 2) || (curButton == 2 && (latch == -1 || reset)))
    {
        wait_ticks(9000); //wait about 22 msec
        
        //Find port and pin from player turn data
        port = turn == 0 ? left_button_ports[1] : right_button_ports[1];
        pin = turn == 0 ? left_button_pins[1] : right_button_pins[1]; 
        
        //Avoid switch bounce
        if(read_single(port,pin))
        {
            //Lock the player in to the green LED and turn one off
            latch = 2;
            if(green < 31)
            {
                green = (green << 1) | 1;
                bus_out_write(green, green_ports, green_pins, 5);
            }

            //Wait for the player to let go
            while(read_single(port,pin))
            {
                wait_ticks(8000);
            }
        }
    }
    // Check to see if the third button was pressed (if it is allowed to be)
    else if ((curButton == latch && latch == 4) || (curButton == 4 && (latch == -1 || reset)))
    {
        wait_ticks(9000); //wait about 22 msec

        //Get port and pin of button based off of turn
        port = turn == 0 ? left_button_ports[0] : right_button_ports[0];
        pin = turn == 0 ? left_button_pins[0] : right_button_pins[0]; 

        //Only allow actual button presses
        if(read_single(port,pin))
        {
            //Lock player into using the Red buttons and turn an LED off
            latch = 4;
            if(red < 127)
            {
                red = (red << 1) | 1;
                bus_out_write(red, red_ports, red_pins, 7);
            }

            //Wait for the player to let go
            while(read_single(port,pin))
            {
                wait_ticks(8000);
            }
        }
    }
}

/**
 * Main function
 */
int main(void)
{
    /* Declare and initialize local variables*/
    int curButton = -1; //tracks the last button press
    int othButton = -1; //tracks the other player's button press
    int turn = 0;       //tracks the players turns (0 or 1)

    //Set the buttons to be pull-down resistors
    PINMODE4 |= (0x3F<<20);
    PINMODE0 |= (0xF<<8) | (0x3<<20);

    //Turn on the LEDs
    bus_out_write(red,red_ports,red_pins,7);
    bus_out_write(blue,blue_ports,blue_pins,3);
    bus_out_write(green,green_ports,green_pins,5);

    /* Run the program forever */
    while (1)
    {
        //If it is player 0's turn
        if (turn == 0)
        {
            // Reset the other player's buttons
            othButton = 0;

            // Read player 0's buttons until their turn is over (when player 1 goes)
            do
            {
                //Get the current button from a bulk read
                curButton = bus_in_read(left_button_ports, left_button_pins, 3);
                buttonPress(curButton, turn, FALSE);
                
                //See if the other player pressed any buttons
                othButton = bus_in_read(right_button_ports, right_button_pins, 3);

                //Check for game over condition
                if(red >= 127 && green >= 31 && blue >= 7)
                {
                    othButton = -1;
                    break;
                }

            } while (othButton == 0);
            
            //Switch turns and update LEDs of other player
            turn = 1;
            buttonPress(othButton, turn, TRUE);
        }
        else
        {
            //Reset other player's buttons
            othButton = 0;

            //Read player 1's buttons until their turn is over
            do
            {
                //Read and process player 1 button input
                curButton = bus_in_read(right_button_ports, right_button_pins, 3);
                buttonPress(curButton, turn, FALSE);

                //Read other buttons to detect turn end
                othButton = bus_in_read(left_button_ports, left_button_pins, 3);

                //If the game is over, break the loop
                if(red >= 127 && green >= 31 && blue >= 7)
                {
                    othButton = -1;
                    break;
                }

            } while (othButton == 0);
            
            //Switch turn
            turn = 0;
            buttonPress(othButton, turn, TRUE);
        }

        //If the game is over, wait for 3 seconds and then restart
        if(red >= 127 && green >= 31 && blue >= 7)
        {
            //wait 3 seconds
            wait_ticks(1210796); 
            
            //Reset game management variables
            red = 0;
            blue = 0;
            green = 0;
            latch = -1;
            turn = 0;

            //Turn LEDs back on
            bus_out_write(red,red_ports,red_pins,7);
            bus_out_write(blue,blue_ports,blue_pins,3);
            bus_out_write(green,green_ports,green_pins,5);
        }
    }
}
