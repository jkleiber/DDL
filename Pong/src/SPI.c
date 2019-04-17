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


int spi_write(int data, int cs_port, int cs_pin, int active)
{
    //Enable the chip
    gpio_write_single(cs_port, cs_pin, active);

    //Write the data to the SPI data register
    SPI_DAT = data;

    //Wait for SPIF bit in SPI status register to be 1
    while(((SPI_STAT>>1) & 1) == 0){}

    //TODO: read the SPI status register

    //Read the SPI data register
    int resp = SPI_DAT;

    //Disable the chip
    gpio_write_single(cs_port, cs_pin, !active);

    //Return the response
    return resp;
}