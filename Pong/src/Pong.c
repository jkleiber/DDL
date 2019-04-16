#include <stdio.h>

#include "GPIO.h"
#include "register_defs.h"
#include "SPI.h"

//Program constants
#define BALL_SIZE       2
#define BALL_SPEED      2
#define PADDLE_SPEED    1
#define PADDLE_LENGTH   10

//Pin Definitions
#define LCD_RES_PORT    2
#define LCD_RES_PIN     7

//Variables for pong paddle tracking
int paddle1_x = 5;
int paddle1_y = 20;
int paddle2_x = 79;
int paddle2_y = 20;

//Variables for pong ball tracking
int ball_x = 24;
int ball_y = 42;
int ball_x_spd = 1;
int ball_y_spd = 1;


//TODO: Collision detection


int main(void) 
{
    //Reset the LCD immediately
    gpio_write_single(2, 7, 0); //RESET pin (P2.7)(49)

    //TODO: Pin 47 (P2.5) = CS, Pin 48 (P2.6) = D/C not

    //TODO: Set up the audio output (DMA) and the sound effects

    //TODO: Quadrature Encoders on GPIO interrupts

    //TODO: Do the RST thing for the display

    //Initialize SPI
    spi_setup();
    spi_master();
    
    //Run the program infinitely
    while(1) 
    {
        //TODO: Update the locations of the ball

        //TODO: Detect collisions and goals

        //TODO: Change ball physics if needed

        //TODO: Make sound if needed

        //TODO: Update display

    }

    return 0;
}
