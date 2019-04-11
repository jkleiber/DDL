#include <stdio.h>

#include "register_defs.h"
#include "SPI.h"

int main(void) {

    printf("Hello World\n");

    //TODO: Pin 47 (P2.5) = CS, Pin 48 (P2.6) = D/C not, Pin 49 (P2.7) = RST

    //TODO: Set up the audio output (DMA) and the sound effects

    //TODO: Quadrature Encoders on GPIO interrupts

    //TODO: Do the RST thing for the display

    //Initialize SPI
    spi_setup();
    spi_master();
    
    //Run the program infinitely
    while(1) 
    {
        
    }

    return 0;
}
