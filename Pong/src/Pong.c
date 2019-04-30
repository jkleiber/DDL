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
#define BALL_SPEED      2
#define PADDLE_SPEED    1
#define PADDLE_LENGTH   10
#define PADDLE_WIDTH    2
#define BALL_X_START    40
#define BALL_Y_START    24
#define P1_X_START      3
#define P1_Y_START      20
#define P2_X_START      79
#define P2_Y_START      20
#define GOAL_TIME       1

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

void EINT3_IRQHandler(void)
{
    int status = IOIntStatus;
    if(IOIntStatus != 0)
    {
        
        if((IO0IntStatF>>27 & 1) && !(gpio_read_single(0, 28)) && paddle2_y < MAX_ROW - PADDLE_LENGTH)
        {
            paddle2_y+=PADDLE_SPEED;
        }
        else if ((IO0IntStatF>>27 & 1) && (gpio_read_single(0, 28)) && paddle2_y > 0)
        {
            paddle2_y-=PADDLE_SPEED;
        }
        IO0IntClr |= (0b11 << 27);

        if((IO0IntStatF>>2 & 1) && !(gpio_read_single(0, 3)) && paddle1_y < MAX_ROW - PADDLE_LENGTH)
        {
            paddle1_y+=PADDLE_SPEED;
        }
        else if ((IO0IntStatF>>2 & 1) && (gpio_read_single(0, 3)) && paddle1_y > 0)
        {
            paddle1_y-=PADDLE_SPEED;
        }
        IO0IntClr |= (0b11 << 2);
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

    //Bounce off the player 1 paddle
    if(ball_x >= paddle1_x && ball_x < (paddle1_x + PADDLE_WIDTH))
    {
        if(paddle1_y < (ball_y + BALL_SIZE) && (paddle1_y + PADDLE_LENGTH) > ball_y)
        {
            ball_x_spd = -ball_x_spd;
            sound = TRUE;
        }
    }
    //Bounce off the player 2 paddle
    else if ((ball_x + BALL_SIZE) >= paddle2_x && ball_x < (paddle2_x + PADDLE_WIDTH))
    {
        if(paddle2_y < (ball_y + BALL_SIZE) && (paddle2_y + PADDLE_LENGTH) > ball_y)
        {
            ball_x_spd = -ball_x_spd;
            sound = TRUE;
        }
    }

    //Make sound if needed
    if(sound)
    {
        wall_sound();
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

//Make sound
void wall_sound()
{
    for(int amp = 500; amp >= 400; amp-=3)
    {
        {
            DACR = (amp << 6);
            wait_ticks(300);
            DACR = 0;
            wait_ticks(300);
        }
    }
}

void point_sound()
{
     for(int amp = 600; amp >= 400; amp-=3)
    {
        {
            DACR = (amp << 6);
            wait_ticks(900);
            DACR = 0;
            wait_ticks(900);
        }
    }
}

int main(void) 
{
    //Reset the LCD immediately
    gpio_write_single(LCD_RES_PORT, LCD_RES_PIN, LOW); //RESET pin (P2.7)(49)

    //Initialize goal variables
    goal_timer = 0;

    //Quadrature Encoders on GPIO interrupts
    IO0IntEnF |= (0b11 << 27);
    IO0IntClr |= (0b11 << 27);
    IO0IntEnF |= (0b11 << 2);
    IO0IntClr |= (0b11 << 2);
    ISER0 |= (1<<21);

    /** Set up the audio output and the sound effects **/
    //Configure the clock source
    PCLKSEL0 |= (1 << 22);

    //Configure the PINSEL registers to enable AOUT
    PINSEL1 |= (1 << 21);

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
            
            if(goal_timer == 0)
            {
                point_sound();
            }

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
