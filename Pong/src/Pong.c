#include <stdio.h>

#include "GPIO.h"
#include "pin_defs.h"
#include "pong_lcd.h"
#include "register_defs.h"
#include "SPI.h"

//Program constants
#define BALL_SIZE       2
#define BALL_SPEED      2
#define PADDLE_SPEED    1
#define PADDLE_LENGTH   10

//Variables for pong paddle tracking
int paddle1_x = 5;
int paddle1_y = 20;
int paddle2_x = 79;
int paddle2_y = 20;

//Variables for pong ball tracking
int ball_x = 24;
int ball_y = 42;
int ball_x_spd = BALL_SPEED;
int ball_y_spd = BALL_SPEED;


//TODO: Collision detection


//TODO: Goal detection


int main(void) 
{
    //Reset the LCD immediately
    gpio_write_single(LCD_RES_PORT, LCD_RES_PIN, 0); //RESET pin (P2.7)(49)

    //TODO: Set up the audio output (DMA) and the sound effects

    //TODO: Quadrature Encoders on GPIO interrupts

    //Wait some time so the RES is applied
    wait_ticks(10000);

    //Initialize SPI
    spi_setup();
    spi_master();
    
    //Run the program infinitely
    while(1) 
    {
        //Update the locations of the ball
        ball_x += ball_x_spd;
        ball_y += ball_y_spd;

        //TODO: Detect collisions and goals

        //TODO: Change ball physics if needed

        //TODO: Make sound if needed

        //Update display
        draw_paddle(paddle1_x, paddle1_y, PADDLE_LENGTH);
        draw_paddle(paddle2_x, paddle2_y, PADDLE_LENGTH);
        draw_ball(ball_x, ball_y, BALL_SIZE);
    }

    return 0;
}
