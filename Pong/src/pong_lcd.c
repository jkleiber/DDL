#include "pong_lcd.h"

void lcd_start()
{
    //Set the appropriate clock phase and polarity
    spi_set_polarity(0);    //the clock idles at low
    spi_set_phase(0);       //the LCD samples data on the leading edge of CLK
}



void draw_rect(int x, int y, int height, int width)
{
    //Declare local variables
    int bank_start;     //start of the Y bank for the rectangle
    int bank_start_bit; //the first bit to use in the first Y bank
    int bank_end;       //last Y bank to write to
    int bank_end_bit;   //last bit in last Y bank to write to
    int cmd;            //command to write to SPI
    int col_end;        //end column on LCD 
    int row_end;        //end row on LCD
    int px_out;         //pixel data to write out to screen

    //Ensure inputs are in range
    x = x > 83 ? 83 : x;    //if x is greater than 83 (max col index), set x to 83
    x = x < 0 ? 0 : x;      //if x is less than 0, set x to 0
    y = y > 47 ? 47 : y;    //if y is greater than 47 (max row index), set y to 47
    y = y < 0 ? 0 : y;      //if y is less than 0, set y to 0

    //Calculate the last column and row of the rectangle
    col_end = x + width;                    //get the last column
    col_end = col_end > 83 ? 83 : col_end;  //make sure the last col is in bounds
    row_end = y + height;                   //get the last row
    row_end = row_end > 47 ? 47 : row_end;  //make sure the last row is in bounds

    /** Calculate the Y RAM address range **/
    //Determine the bank and bit of the first point
    bank_start = y / 8;
    bank_start_bit = y % 8;

    //Determine the bank and bit of the end point
    bank_end = (y + height) / 8;
    bank_end_bit = (y + height) % 8;

    /** Form the rectangle layer by layer (column by column) **/
    for(int col = x; col <= col_end; ++col)
    {
        /** Set the X RAM address **/
        //Reset the D/C pin
        gpio_write_single(LCD_DC_PORT, LCD_DC_PIN, LOW);

        //Determine the byte sequence
        cmd = 1 << 7;
        cmd |= col;

        //Send the SPI command
        spi_write(cmd, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);

        /** Set the Y RAM address **/
        //For each bank necessary, set the y pixels
        for(int b = bank_start; b <= bank_end; ++b)
        {
            //Write to the Y RAM to choose this bank
            cmd = 1 << 6;                                           //set DB6
            cmd |= b;                                               //choose the bank
            spi_write(cmd, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);   //write to select the bank bit

            //Get ready for a column of pixels to appear
            px_out = 0;

            //Set the appropriate pixels in this bank
            for(int px = bank_start_bit; px >= 0; --px)
            {
                px_out |= 1 << px;
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



void draw_score(int p1_score, int p2_score)
{

}


void draw_clear()
{
    //Make sure D/C is set to low
    gpio_write_single(LCD_DC_PORT, LCD_DC_PIN, LOW);

    //Write 0b1000 to the display to clear its contents
    spi_write(8, LCD_SPI_CS_PORT, LCD_SPI_CS_PIN, LOW);
}