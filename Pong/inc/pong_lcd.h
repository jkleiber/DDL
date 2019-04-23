#ifndef PONG_LCD_H
#define PONG_LCD_H

#include "SPI.h"
#include "GPIO.h"
#include "pin_defs.h"
#include "wait_code.h"
#include "constants.h"

#define SCREEN_WIDTH 84
#define SCREEN_HEIGHT 48
#define MAX_COL (SCREEN_WIDTH - 1)
#define MAX_ROW (SCREEN_HEIGHT - 1)

void lcd_start();

void clear_screen();

void draw_rect(int x, int y, int height, int width);

void draw_score(int p1_score, int p2_score);

//Test functions
void draw_checkers();
void draw_test();

void draw_screen();


#endif