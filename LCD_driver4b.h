/*
 * LCD_driver4b.h
 *
 *  Created on: Dec 23, 2022
 *      Author: bigjo
 */

#ifndef LCD_DRIVER4B_H_
#define LCD_DRIVER4B_H_

// macros created for facilitating
#define SET_BIT(VAR,BIT) VAR|=(1<<BIT)
#define CLR_BIT(VAR,BIT) VAR&=(~(1<<BIT))
#define GET_BIT(VAR,BIT) ((VAR>>BIT)&(0X01))
#define GET_BIT_down(VAR,BIT) VAR&(1<<BIT) // PULL DOWN SWITCH
#define GET_BIT_up(VAR,BIT) !(VAR&(1<<BIT)) // PULL up SWITCH
#define TOGGLE_BIT(VAR,BIT) VAR^=(1<<BIT)





void lcd_command(char command);
void lcd_data(char data);
void lcd_init();
void lcd_string(char * ptr);
void lcd_goto(char row, char col);
void LCD_intgerToString(int data);
void lcd_clear();



#endif /* LCD_DRIVER4B_H_ */
