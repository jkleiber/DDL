#ifndef PONG_LCD_H
#define PONG_LCD_H

#include "SPI.h"
#include "GPIO.h"
#include "pin_defs.h"
#include "constants.h"

void lcd_start();

void draw_rect(int x, int y, int height, int width);

void draw_score(int p1_score, int p2_score);

void draw_clear();

//TODO: add any other draw functions needed

#endif