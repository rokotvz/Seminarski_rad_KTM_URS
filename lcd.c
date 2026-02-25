/*
 * lcd.c
 *
 *  Created on: Oct 29, 2025
 *      Author: Korisnik
 */

#include "lcd.h"

#define LCD_ADDR (0x27 << 1)    // Adresa I2C LCD-a (shift za HAL)
#define LCD_BACKLIGHT 0x08
#define ENABLE 0x04

// Pomoćna funkcija za slanje jednog bajta naredbe ili podataka
static void LCD_SendInternal(uint8_t data) {
    uint8_t data_u = data & 0xF0;
    uint8_t data_l = (data << 4) & 0xF0;
    uint8_t data_t[4];

    // slanje visokog nibbla
    data_t[0] = data_u | LCD_BACKLIGHT | ENABLE;
    data_t[1] = data_u | LCD_BACKLIGHT;
    // slanje niskog nibbla
    data_t[2] = data_l | LCD_BACKLIGHT | ENABLE;
    data_t[3] = data_l | LCD_BACKLIGHT;

    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data_t, 4, 100);
}

// Šalje naredbu (command)
void LCD_SendCommand(char cmd)
{
    LCD_SendInternal(cmd);
}

//void LCD_SendCommand(char cmd) {
//    LCD_SendInternal(cmd & 0xF0);
//}

// Šalje jedan znak
void LCD_SendData(char data) {
    uint8_t data_u = data & 0xF0;
    uint8_t data_l = (data << 4) & 0xF0;
    uint8_t data_t[4];

    data_t[0] = data_u | LCD_BACKLIGHT | ENABLE | 0x01;
    data_t[1] = data_u | LCD_BACKLIGHT | 0x01;
    data_t[2] = data_l | LCD_BACKLIGHT | ENABLE | 0x01;
    data_t[3] = data_l | LCD_BACKLIGHT | 0x01;

    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data_t, 4, 100);
}

// Inicijalizacija LCD-a
void LCD_Init(void) {
    HAL_Delay(50);
    LCD_SendCommand(0x02); // reset
    LCD_SendCommand(0x28); // 4-bit mode, 2 linije, 5x8 font
    LCD_SendCommand(0x0C); // Display ON, cursor OFF
    LCD_SendCommand(0x06); // pomak kursora nakon upisa
    LCD_SendCommand(0x01);
    LCD_Clear();
}

// Brisanje ekrana
void LCD_Clear(void) {
    LCD_SendCommand(0x01);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t addr;

    if(row == 0)
        addr = 0x80 + col;
    else
        addr = 0xC0 + col;

    LCD_SendCommand(addr);
}


/*
// Postavljanje kursora
void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t pos_Addr;
    switch(row) {
        case 0: pos_Addr = 0x80 + col; break;
        case 1: pos_Addr = 0xC0 + col; break;
        default: pos_Addr = 0x80 + col; break;
    }
    LCD_SendCommand(pos_Addr);
}
*/
// Slanje stringa
void LCD_SendString(char *str) {
    while (*str) {
        LCD_SendData(*str++);
    }
}



