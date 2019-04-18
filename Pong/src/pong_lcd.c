#include "pong_lcd.h"

//Screen data
int screen[48][84];

void lcd_start()
{
    //Set the appropriate clock phase and polarity
    spi_set_polarity(0);    //the clock idles at low
    spi_set_phase(0);       //the LCD samples data on the leading edge of CLK

    //Set up the display!
    spi_write(0x21, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);  //Notify about extended instruction set
    spi_write(0xB0, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);  //Set Vop
    spi_write(0x04, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);  //Set temperature coeff
    spi_write(0x13, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);  //LCD bias mode
    
    //Modify display mode
    spi_write(0x20, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);  //Control byte
    spi_write(0x0C, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);  //Normal display mode
}

void clear_screen()
{
    //Set screen bits to 0
    for(int col = 0; col < 83; ++col)
    {
        for(int row = 0; row < 47; ++row)
        {
            screen[row][col] = 0;
        }
    }
}

void draw_rect(int x, int y, int height, int width)
{
    //Don't draw anything if it is out of bounds
    if(x < 0 || (x + width) > 83 || y < 0 || (y + height) > 47)
    {
        return;
    }

    //Make the rectangle column by column
    for(int col = x; col < (x + width); ++col)
    {
        for(int row = y; row < (y + height); ++row)
        {
            screen[row][col] = 1;
        }
    }
}



void draw_score(int p1_score, int p2_score)
{

}


void draw_screen()
{
    //Declare local variables
    int bank;
    int cmd;
    int col;
    int px_out;
    int row;

    //Go through each bank of Y bits layer by layer (column by column)
    for(col = 0; col < 83; ++col)
    {
        /** Set X RAM address **/
        //Reset the D/C pin
        gpio_write_single(LCD_DC_PORT, LCD_DC_PIN, LOW);

        //Determine the byte sequence
        cmd = 1 << 7;
        cmd |= col;

        //Send the SPI command
        spi_write(cmd, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);

        //Go through the six banks
        for(bank = 0; bank < 6; ++bank)
        {
            //Write to the Y RAM to choose this bank
            cmd = 1 << 6;                                           //set DB6
            cmd |= bank;                                            //choose the bank
            spi_write(cmd, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);   //write to select the bank bit

            //Reset px out so the display has no artifacts
            px_out = 0;

            //Assign display data to this bank
            for(int i = 0; i < 8; ++i)
            {
                row = (bank * 8) + i;
                px_out |= screen[row][col] << i;
            }

            //Raise the D/C register for writing to the display
            gpio_write_single(LCD_DC_PORT, LCD_DC_PIN, HIGH);

            //Write the pixels to the display
            spi_write(px_out, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);

            //Lower the D/C pin for selecting RAM addresses again
            gpio_write_single(LCD_DC_PORT, LCD_DC_PIN, LOW);
        }
    }
}