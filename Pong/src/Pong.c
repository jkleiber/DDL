#include <stdio.h>

#include "register_defs.h"
#include "SPI.h"

int main(void) {

    printf("Hello World\n");

    //Initialize SPI
    spi_setup();
    spi_master();
    
    while(1) {
        
    }
    return 0 ;
}
