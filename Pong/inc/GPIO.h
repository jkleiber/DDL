#ifndef GPIO_H
#define GPIO_H

#include "wait_code.h"
#include "register_defs.h"

void gpio_write_single(const int port, const int pin, const int value);
void gpio_bus_out_write(unsigned int bits, const int *ports, const int *pins, int num_bits);

unsigned int gpio_read_single(const int port, const int pin);
unsigned int gpio_bus_in_read(const int *ports, const int *pins, int num_pins);

#endif