#ifndef SPI_H
#define SPI_H

#include "register_defs.h"

/**
 * @brief 
 * 
 */
void spi_setup();

/**
 * @brief 
 * 
 */
void spi_master();

void spi_set_phase(int cpha);

void spi_set_polarity(int cpol);

#endif