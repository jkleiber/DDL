#include <stdio.h>

#include "constants.h"
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
#define PADDLE_WIDTH    3

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
    gpio_write_single(LCD_RES_PORT, LCD_RES_PIN, LOW); //RESET pin (P2.7)(49)

    //TODO: Set up the audio output (DMA) and the sound effects

    //TODO: Quadrature Encoders on GPIO interrupts

    //Wait some time so the RES is applied
    wait_ticks(10000);

    //Unlatch the reset for the LCD
    gpio_write_single(LCD_RES_PORT, LCD_RES_PIN, HIGH);

    //Initialize SPI
    spi_setup();
    spi_master();

    //Start the LCD driver
    lcd_start();
    
    //Run the program infinitely
    while(1) 
    {
        //Update the locations of the ball
        ball_x += ball_x_spd;
        ball_y += ball_y_spd;

        //TODO: Detect collisions and goals

        //TODO: Change ball physics if needed

        //TODO: Make sound if needed

        //Clear the display
        draw_clear();

        //Update display
        draw_rect(paddle1_x, paddle1_y, PADDLE_LENGTH, PADDLE_WIDTH);
        draw_rect(paddle2_x, paddle2_y, PADDLE_LENGTH, PADDLE_WIDTH);
        draw_rect(ball_x, ball_y, BALL_SIZE, BALL_SIZE);

        //Slow down the run so the game is playable
        wait_ticks(1000);   //TODO: tune this value
    }

    return 0;
}
