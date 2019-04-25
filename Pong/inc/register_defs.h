#ifndef REGISTER_DEFS_H
#define REGISTER_DEFS_H

//GPIO
typedef struct GPIO_regs_t
{
    volatile unsigned int FIODIR;
    int rsvd[3];
    volatile unsigned int FIOMASK;
    volatile unsigned int FIOPIN;
    volatile unsigned int FIOSET;
    volatile unsigned int FIOCLR;

} GPIO_regs;

//Define FIO
#define FIO ((GPIO_regs *)0x2009c000)

//PINMODE
#define PINMODE0 (*(volatile int *)0x4002C040)
#define PINMODE1 (*(volatile int *)0x4002C044)
#define PINMODE2 (*(volatile int *)0x4002C048)
#define PINMODE3 (*(volatile int *)0x4002C04C)
#define PINMODE4 (*(volatile int *)0x4002C050)

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

//GPIO interrupts
#define IO0IntEnR (*(volatile int *) 0x40028090)
#define IO0IntEnF (*(volatile int *) 0x40028094)
#define IO0IntStatF (*(volatile int *) 0x40028088)
#define IO0IntClr (*(volatile int *) 0x4002808C)
#define ISER0 (*(volatile int *) 0xE000E100)
#define IOIntStatus (*(volatile int*) 0x40028080)


//Sound-define DAC registers
#define DACR (*(volatile int *) 0x4008C000)

//External interrupts
//#define EXTINT (*(volatile int *) 0x400FC140)
//#define EXTMODE (*(volatile int *) 0x400FC148)
//#define EXTPOLAR (*(volatile int *) 0x400FC14C)

#endif