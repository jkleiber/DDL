#ifndef PONG_LCD_H
#define PONG_LCD_H

#include "SPI.h"
#include "GPIO.h"
#include "pin_defs.h"

void draw_paddle(int x, int y, int length);

void draw_ball(int x, int y, int size);

void draw_score(int p1_score, int p2_score);

//TODO: add any other draw functions needed

#endif