#include <stdio.h>

#include "constants.h"
#include "GPIO.h"
#include "pin_defs.h"
#include "pong_lcd.h"
#include "register_defs.h"
#include "SPI.h"
#include "wait_code.h"

//Program constants
#define BALL_SIZE       3
#define BALL_SPEED      1
#define PADDLE_SPEED    1
#define PADDLE_LENGTH   10
#define PADDLE_WIDTH    2

//Variables for pong paddle tracking
volatile int paddle1_x = 3;
volatile int paddle1_y = 20;
volatile int paddle2_x = 79;
volatile int paddle2_y = 20;

//Variables for pong ball tracking
volatile int ball_x = 40;
volatile int ball_y = 24;
volatile int ball_x_spd = BALL_SPEED;
volatile int ball_y_spd = BALL_SPEED;


//Collision detection
void check_collision()
{
    //Bounce off the wall
    if((ball_y + BALL_SIZE) >= MAX_ROW || ball_y <= 0)
    {
        ball_y_spd = -ball_y_spd;
    }

    //Bounce off the paddles
    if((ball_x == (paddle1_x + PADDLE_WIDTH) && (ball_y >= paddle1_y && ball_y < (paddle1_y + PADDLE_LENGTH)))
    || ((ball_x + BALL_SIZE) == paddle2_x && (ball_y >= paddle2_y && ball_y < (paddle2_y + PADDLE_LENGTH))))
    {
        ball_x_spd = -ball_x_spd;
    }
    
}

//Goal detection
void check_goal()
{
    //Did player 1 score?
    if(ball_x > SCREEN_WIDTH)
    {

    }
    //Did player 2 score?
    else if((ball_x + BALL_SIZE) < 0)
    {

    }
}

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
        /** UPDATE FUNCTIONS **/
        //Check for collisions
        check_collision();

        //Update the locations of the ball
        ball_x += ball_x_spd;
        ball_y += ball_y_spd;

        //TODO: Detect goals

        //TODO: Change ball physics if needed

        //TODO: Make sound if needed

        /** DISPLAY LOGIC **/
        //Clear the display
        clear_screen();

        //spi_write(0xD, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);
        //Update display
        draw_rect(paddle1_x, paddle1_y, PADDLE_LENGTH, PADDLE_WIDTH);
        draw_rect(ball_x, ball_y, BALL_SIZE, BALL_SIZE);
        draw_rect(paddle2_x, paddle2_y, PADDLE_LENGTH, PADDLE_WIDTH);

        //Test the screen
        //draw_checkers();

        //Paint the updates to the screen
        draw_screen();

        //Slow down the run so the game is playable
        wait_ticks(1000);
    }

    return 0;
}
