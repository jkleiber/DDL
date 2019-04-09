#include "SPI.h"

void spi_setup()
{
    //Set the SPI power control bit
    PCONP |= (1 << 8);

    //Set the SPI clock equal to CCLK
    PCLKSEL0 |= (1 << 16);

    //Configure SPI output
    PINSEL1 |= (3) | (3 << 2) | (3 << 4);   //Configure SSEL, MISO, MOSI respectively
    PINSEL0 |= (3 << 30);                   //Configure SCK
}


void spi_master()
{
    //Set the SPI clock frequency to be greater or equal to 8
    SPI_CLK_CNT = 8;

    //Set this SPI device as master
    SPI_CON |= (1 << 5);
}


void spi_set_polarity(int cpol)
{
    //Choose clock polarity (0 or 1)
    int polarity = cpol == 1;

    //Apply CPOL = 1 to SPI control register
    if(polarity)
    {
        SPI_CON |= (1 << 4);
    }
    //Apply CPOL = 0 to SPI control register
    else
    {
        SPI_CON &= ~(1 << 4);
    }
}


void spi_set_phase(int cpha)
{
    //Choose clock polarity (0 or 1)
    int phase = cpha == 1;

    //Apply CPHA = 1 to SPI control register
    if(phase)
    {
        SPI_CON |= (1 << 3);
    }
    //Apply CPHA = 0 to SPI control register
    else
    {
        SPI_CON &= ~(1 << 3);
    }
} 