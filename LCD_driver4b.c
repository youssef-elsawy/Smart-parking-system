/*
 * LCD_driver4b.c
 *
 *  Created on: Dec 23, 2022
 *      Author: bigjo
 */

#include <stdio.h>
#include "stdlib.h"
#include <avr/io.h>
#include <util/delay.h>
#include "LCD_driver4b.h"

/*
 * Function Name: lcd_command
 * Function Returns: void
 * Function Arguments: char
 * Function Description: used to execute the required input command
 */
void lcd_command(char command){
    CLR_BIT(PORTB,1); // LCD_RS: register select (command) --> "0"
    CLR_BIT(PORTB,2); // LCD_RW: R/W input allows the user to write information to the LCD or read information from it. R/W --> 1 when reading; R/W --> 0 when writing.
    /*
     * make "and" operation between data pins (PORTA) and 00001111 to preserve the value of (A0 --> A3) and clear the value of (A4 --> A7)
     * make "and" operation between data pins (PORTA) and 11110000 to preserve the value of the last 4 bits of command and clear the first 4 bits
     * make "or" operation between the result to pass the last 4 bits of command to PORTA while preserving the value of (A0 --> A3)
     */
    PORTA=(PORTA & 0X0F)|(command & 0XF0); // send the last 4 bits of command to data pins
    _delay_ms(1);
    SET_BIT(PORTB,3); //LCD _ENABLE: When data is supplied to data pins, a high-to-low pulse must be applied to this pin in order for the LCD to latch in the data present at the data pins.
    _delay_ms(1);
    CLR_BIT(PORTB,3); //LCD _ENABLE

    PORTA =(PORTA & 0X0F)|(command << 4); // send the first 4 bits of command to data pins by left shift the command by 4 bits
    _delay_ms(1);
     SET_BIT(PORTB,3); //LCD _ENABLE: When data is supplied to data pins, a high-to-low pulse must be applied to this pin in order for the LCD to latch in the data present at the data pins.
     _delay_ms(1);
     CLR_BIT(PORTB,3); //LCD _ENABLE
     _delay_ms(100);
}

/*
 * Function Name: lcd_data
 * Function Returns: void
 * Function Arguments: char
 * Function Description: used to write the required input data (single character)
 */
void lcd_data(char data){
    SET_BIT(PORTB,1); // LCD_RS: register select (data) --> "1"
    CLR_BIT(PORTB,2); // LCD_RW
    PORTA=(PORTA & 0X0F)|(data & 0XF0); // send the last 4 bits of data to data pins
    _delay_ms(1);
    SET_BIT(PORTB,3); //LCD _ENABLE
    _delay_ms(1);
    CLR_BIT(PORTB,3); //LCD _ENABLE

     PORTA =(PORTA & 0X0F)|(data << 4); // send the first 4 bits of data to data pins by left shift the command by 4 bits
     _delay_ms(1);
     SET_BIT(PORTB,3); //LCD _ENABLE
      _delay_ms(1);
      CLR_BIT(PORTB,3); //LCD _ENABLE
      _delay_ms(100);
    }

/*
 * Function Name: lcd_init
 * Function Returns: void
 * Function Arguments: void
 * Function Description: used to initialize LCD by setting (A4 to A7) as output, set LCD as 4 bit mode, clear screen then set cursor off.
 */
void lcd_init(){
    DDRA|=0XF0;       // set D4 TO D7 as output
    SET_BIT(DDRB,1); // LCD_RS
    SET_BIT(DDRB,2); // LCD_RW
    SET_BIT(DDRB,3); // LCD_EN
    CLR_BIT(PORTB,3); //LCD _EN
    _delay_ms(2000);

    //set as 4 BIT MODE
   lcd_command(0X33);
    lcd_command(0X32);
    lcd_command(0X28);

   lcd_command(0X02); // set to HOME position
    lcd_command(0X01); //CLEAR screen
    lcd_command(0X0C); // CURSOR Off
}

/*
 * Function Name: lcd_string
 * Function Returns: void
 * Function Arguments: string
 * Function Description: used to get a string and save it in an array and write it on the screen letter by letter by calling (lcd_data) function
 */
void lcd_string(char * ptr){
    char i =0;
    // passing the letters of the string letter by letter to lcd_data function to write it till end of string (null terminator exist)
    while(ptr[i]!=0){
        lcd_data(ptr[i]);
        i++;
    }
}

/*
 * Function Name: lcd_goto
 * Function Returns: void
 * Function Arguments: 2 char inputs
 * Function Description: used to moving cursor to specific address by getting the row and the column required
 */
void lcd_goto(char row, char col){
	// if row =0, the address wil be 0X00 to 0X0F according to the column
    if (row==0){
        lcd_command(0X80+(0X0F&col));
    }else lcd_command(0X80+(0X4F&(col+0XF0))); // if row !=0, the address wil be 0X40 to 0X4F according to the column
}

/*
 * Function Name: LCD_intgerToString
 * Function Returns: void
 * Function Arguments: int
 * Function Description: used to convert integer input to string
 */
void LCD_intgerToString(int data) // from 500 to "500"
{
   char buff[16]; /* String to hold the ascii result */
   itoa(data,buff,10); /* 10 for decimal */
   lcd_string(buff);
}

/*
 * Function Name: lcd_clear
 * Function Returns: void
 * Function Arguments: void
 * Function Description: used to call the lcd_command function to execute clear screen command
 */
void lcd_clear(){
    lcd_command(0X01);
}
