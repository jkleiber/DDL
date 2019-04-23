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
#define BALL_X_START    40
#define BALL_Y_START    24
#define P1_X_START      3
#define P1_Y_START      20
#define P2_X_START      79
#define P2_Y_START      20
#define GOAL_TIME       100000

//Variables for pong paddle tracking
volatile int paddle1_x = P1_X_START;
volatile int paddle1_y = P1_Y_START;
volatile int paddle2_x = P2_X_START;
volatile int paddle2_y = P2_Y_START;

//Variables for pong ball tracking
volatile int ball_x = BALL_X_START;
volatile int ball_y = BALL_Y_START;
volatile int ball_x_spd = BALL_SPEED;
volatile int ball_y_spd = BALL_SPEED;
volatile int p1pos = 0;

//Variables for goal screen
volatile int goal_timer;

//Game state
int game_state;

void EINT0_IRQHandler(void)
{
    if(IOIntStatus != 0)
    {
        if((IO0IntStatF>>27 & 1) && !(gpio_read_single(0, 28)))
        {
            p1pos++;
        }
        else if ((IO0IntStatF>>28 & 1) && !(gpio_read_single(0, 27)))
        {
            p1pos--;
        }
    }
}

//Collision detection
void check_collision()
{
    int sound = FALSE;

    //Bounce off the wall
    if((ball_y + BALL_SIZE) >= MAX_ROW || ball_y <= 0)
    {
        ball_y_spd = -ball_y_spd;
        sound = TRUE;
    }

    //Bounce off the paddles
    if((ball_x == (paddle1_x + PADDLE_WIDTH) && (ball_y >= paddle1_y && ball_y < (paddle1_y + PADDLE_LENGTH)))
    || ((ball_x + BALL_SIZE) == paddle2_x && (ball_y >= paddle2_y && ball_y < (paddle2_y + PADDLE_LENGTH))))
    {
        ball_x_spd = -ball_x_spd;
        sound = TRUE;
    }

    //TODO: Make sound if needed (DMA?)
    if(sound)
    {

    }
}

//Goal detection
void check_goal()
{
    //TODO: Add goal sound effect
    //Did player 1 score?
    if(ball_x > SCREEN_WIDTH)
    {
        game_state = P1_GOAL;
    }
    //Did player 2 score?
    else if((ball_x + BALL_SIZE) < 0)
    {
        game_state = P2_GOAL;
    }
}

int main(void) 
{
    //Reset the LCD immediately
    gpio_write_single(LCD_RES_PORT, LCD_RES_PIN, LOW); //RESET pin (P2.7)(49)

    //Initialize goal variables
    goal_timer = 0;

    //TODO: Set up the audio output (DMA) and the sound effects

    //Quadrature Encoders on GPIO interrupts
    IO0IntEnF |= (0b11 << 27);
    IO0IntClr |= (0b11 << 27);
    ISER0 |= (1<<18);

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
        //Goal updates
        if((game_state == P1_GOAL) || (game_state == P2_GOAL))
        {
            //Reset positions
            ball_x = BALL_X_START;
            ball_y = BALL_Y_START;
            paddle1_x = P1_X_START;
            paddle1_y = P1_Y_START;
            paddle2_x = P2_X_START;
            paddle2_y = P2_Y_START;
            
            //Increment goal timer
            ++goal_timer;

            if(goal_timer >= GOAL_TIME)
            {
                game_state = PLAYING;
                goal_timer = 0;
            }
        }
        //Normal play updates
        else if(game_state == PLAYING)
        {
            //Check for collisions
            check_collision();

            //Detect goals
            check_goal();

            //Update the locations of the ball
            ball_x += ball_x_spd;
            ball_y += ball_y_spd;
        }   

        /** DISPLAY LOGIC **/
        //Clear the display
        clear_screen();

        //Draw a goal animation if someone scores
        if((game_state == P1_GOAL) || (game_state == P2_GOAL))
        {

        }
        else if(game_state == PLAYING)
        {
            //spi_write(0xD, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);
            //Update display
            draw_rect(paddle1_x, paddle1_y, PADDLE_LENGTH, PADDLE_WIDTH);
            draw_rect(ball_x, ball_y, BALL_SIZE, BALL_SIZE);
            draw_rect(paddle2_x, paddle2_y, PADDLE_LENGTH, PADDLE_WIDTH);
        }

        //Paint the updates to the screen
        draw_screen();

        //Slow down the run so the game is playable
        p1pos = p1pos;
        wait_ticks(1000);
    }

    return 0;
}
