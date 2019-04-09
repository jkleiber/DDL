#ifndef REGISTER_DEFS_H
#define REGISTER_DEFS_H

//GPIO

//PINMODE
#define PINMODE1 (*(volatile int *) 0x4002C044)

//PINSEL
#define PINSEL0 (*(volatile int *) 0x4002C000)
#define PINSEL1 (*(volatile int *) 0x4002C004)

//Power Control
#define PCONP (*(volatile int *) 0x400FC0C4)

//Peripheral Clock
#define PCLKSEL0 (*(volatile int *) 0x400FC1A8)

//SPI
#define SPI_CON (*(volatile int *) 0x40020000)
#define SPI_STAT (*(volatile int *) 0x40020004)
#define SPI_DAT (*(volatile int *) 0x40020008)
#define SPI_CLK_CNT (*(volatile int *) 0x4002000C)


#endif