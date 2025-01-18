/*
 * main.c
 *
 *  Created on: Dec 23, 2022
 *      Author: bigjo
 */

#include <avr/io.h>
#include <util/delay.h>
#include"LCD_driver4b.h"
#include <avr/interrupt.h>


// function deceleration
void Read_Sensor();
void delay(int );


//GLOBAL VARIABLES DECLARATION
int S1=0, S2=0, S3=0, S4=0;
int flag1=0, flag2=0, flag3=0, flag4=0, flag5=0, FLAG6=0;
int slot = 4;

// interrupt service routine executed when pushbutton is activated
ISR(INT0_vect){
	flag5=1;

}

int main(){
	SET_BIT(SREG,7); // enable global interrupt    
	SET_BIT(GICR,6); // enable modular interrupt, external pin interrupt (INT0)
	// these bits define the level or edge on the external INT0 pin that activate the interrupt
	CLR_BIT(MCUCR,0); // falling edge of INT0 generates interrupt request
	SET_BIT(MCUCR,1);

//entrance sensors direction as inputs
	CLR_BIT(DDRC,0);
	CLR_BIT(DDRC,1);
//slots sensors direction as inputs
	CLR_BIT(DDRB,4);
	CLR_BIT(DDRB,5);
	CLR_BIT(DDRB,6);
	CLR_BIT(DDRB,7);
//exit sensors direction as inputs
	CLR_BIT(DDRC,3);
	CLR_BIT(DDRC,4);
//slots led direction as outputs
	SET_BIT(DDRD,0);
	SET_BIT(DDRC,2);
	SET_BIT(DDRC,7);
	SET_BIT(DDRD,3);
// BUTTON direction as input
	CLR_BIT(DDRD,2);
//BUZZER direction as output
	SET_BIT(DDRA,3);

	// activate mode (14) which is (FAST PWM) mode with top is ICR1 (input capture register)
	SET_BIT(TCCR1A,1); //WGM11
	SET_BIT(TCCR1B,3); //WGM12
	SET_BIT(TCCR1B,4); //WGM13

	// activate prescaler 8
	SET_BIT(TCCR1B,1); //CS11

	// clear OC1A and OC1B on compare match with non-inverting mode
	SET_BIT(TCCR1A,5); // COM1B1
	SET_BIT(TCCR1A,7); // COM1A1

	// set the top value in the input capture register
	ICR1=39999; // get this value from the equation ( 50hz = 16Mhz / 8(1+TOP) ) to be stored in the input capture register as the top value

	TCNT1=0; // clear the timer register at the beginning

	// set the pin direction as outputs for both motors
	SET_BIT(DDRD,5); // OC1A ( output compare register 1A)
	SET_BIT(DDRD,4); // OC1B ( output compare register 1B)

	/* F = 16 MHz/8 = 2MHz
	// Ttick= 1 / 2MHz = 0.5 micro second
	//  to get servo at zero degree position @ 1.5 millisecond --> 1.5 millisecond / 0.5 microsecond = 3000
	//  to get servo at 90 degree position @ 2 millisecond --> 2 millisecond / 0.5 microsecond = 4000
	*/

	// set the motors initially at angle 0
	OCR1A=3000;//0 degree
	OCR1B=3000;//0 degree

	Read_Sensor(); // calling function to check status of slots sensors
	int total = S1+S2+S3+S4; // adding the values of s1, s2, s3 and s4 to get the total
	slot = slot-total; // get the number of free slots

	lcd_init(); // initialize the LCD
	lcd_goto(0,0); // move the cursor to address of row 0 and column 0
	lcd_string("welcome"); // write the string on LCD
	delay(1); // calling the delay function to delay 1 second
	lcd_clear(); //clear the screen

	flag5=0; // set flag5 initially equal 0

	while(1){
		Read_Sensor(); // calling function to check status of slots sensors
/************************************************************************************************************************/
		lcd_goto(0,0); // move the cursor to address of row 0 and column 0
		lcd_string("free slots:  "); // write the string on LCD
		LCD_intgerToString(slot); // write the equivalent value of variable (slot) on the screen

		lcd_goto(1,0); // move the cursor to address of row 1 and column 0
		// condition that check if s1==1 (sensor 1 read an obstacle) and the flag5 ==1 (button is pressed using interrupt)
		if(S1==1 && flag5==1){
			lcd_string("S1:F");
			SET_BIT(PORTD,0); //turn on the S1 LED
			FLAG6=1;

		}
		// condition that check if s1==1 (slot sensor 1 read an obstacle) and the flag5 ==0 (button is not pressed)
		else if(S1==1 && flag5==0){
			lcd_string("S1:X");
			CLR_BIT(PORTD,0); //turn off the S1 LED
			SET_BIT(PORTA,3); // turn on the buzzer
		}
		// otherwise the previous conditions
		else{
			lcd_string("S1:E");
			CLR_BIT(PORTD,0); //turn off the S1 LED
			CLR_BIT(PORTA,3); //turn off the buzzer

		}
		// condition that check if s1==0 (slot sensor 1 doesn't read an obstacle anymore), the flag5 ==1 (button is pressed) and FLAG6==1 to reset the flags
		if(S1==0 && FLAG6==1 && flag5==1){
					flag5=0;
					FLAG6=0;
		}

		lcd_goto(1,4); // move the cursor to address of row 1 and column 4
		// condition that check if s2==1 (slot sensor 2 read an obstacle)
		if(S2==1){
			lcd_string("S2:F");
			SET_BIT(PORTC,2); //turn on the S2 LED
			// condition to check if flag5==1 (button is pressed) the flag will be reset
			if (flag5==1){
				flag5=0;
			}
		}
		// otherwise the previous conditions
		else{
			lcd_string("S2:E");
			CLR_BIT(PORTC,2); //turn off the S2 LED
		}

		lcd_goto(1,8); // move the cursor to address of row 1 and column 8
		// condition that check if s3==1 (slot sensor 3 read an obstacle)
		if(S3==1){
			lcd_string("S3:F");
			SET_BIT(PORTC,7); //turn on the S3 LED
			// condition to check if flag5==1 (button is pressed) the flag will be reset
			if (flag5==1){
							flag5=0;
						}
		}
		// otherwise the previous conditions
		else{
			lcd_string("S3:E");
			CLR_BIT(PORTC,7); //turn off the S3 LED
		}

		lcd_goto(1,12); // move the cursor to address of row 1 and column 12
		// condition that check if s4==1 (slot sensor 4 read an obstacle)
		if(S4==1){
			lcd_string("S4:F");
			SET_BIT(PORTD,3); //turn on the S4 LED
			// condition to check if flag5==1 (button is pressed) the flag will be reset
			if (flag5==1){
							flag5=0;
						}
		}
		// otherwise the previous conditions
		else{
			lcd_string("S4:E");
			CLR_BIT(PORTD,3); //turn off the S4 LED
		}


		/************************************************************** entrance sensors **********************************************************/
		// condition that check if the first entrance sensor read an obstacle (==0, as pull up) and the flag1 ==0
		if(GET_BIT(PINC,0) == 0 && flag1==0){
			if(slot>0){
				flag1=1; //flag1 raised when no. of slots greater than zero and entrance sensor 1 read an obstacle
				if(flag2==0){
					OCR1A=4000; // +90 degree (entrance gate open)
				}
				// otherwise when no. of slots equal zero
			}else{
				lcd_goto(1,0); // move the cursor to address of row 1 and column 0
				lcd_string("PARKING IS FULL ");
				delay(1); // calling the delay function to delay 1 second

			}
			// condition that check if the first entrance sensor doesnt read an obstacle anymore (==1, as pull up) and the flag1 ==1 (entrance sensor 1 read an obstacle before) and flag2==0 (entrance sensor 2 doesn't read an obstacle)
		}else if(GET_BIT(PINC,0) == 1 && flag1==1 && flag2==0){
			OCR1A=3000; // 0 degree (entrance gate closed again)
			flag1=0;
		}
		// condition that check if the second entrance sensor read an obstacle (==0, as pull up) and the flag2 ==0
		if(GET_BIT(PINC,1) == 0 && flag2==0){
			flag2=1; //flag2 raised when entrance sensor 2 read an obstacle

		}
		// condition that check if both flags equal 1 (both entrance sensors read an obstacle)
		if(flag1==1 && flag2==1){
			slot = slot-1; // decrease the number of free slots by one
			while(GET_BIT(PINC,1) == 0); // busy wait during entrance sensor 2 read an obstacle (wait till the car pass the second entrance sensor)
			OCR1A=3000; // 0 degree (entrance gate closed)
			flag1=0, flag2=0;
		}

		/************************************************************** exit sensors **********************************************************/
		// condition that check if the first exit sensor read an obstacle (==0, as pull up) and the flag3 ==0
		if(GET_BIT(PINC,3) == 0 && flag3==0){
			if(slot<4){
				flag3=1; //flag1 raised when no. of slots less than 4 and exit sensor 1 read an obstacle
				if(flag4==0){
					OCR1B=4000;// +90 degree (exit gate open)
					}
				// otherwise when no. of slots equal 4
			}else{
				lcd_goto(1,0); // move the cursor to address of row 1 and column 0
				lcd_string("PARKING IS EMPTY");
				delay(1); // calling the delay function to delay 1 second
					}
			// condition that check if the first exit sensor doesnt read an obstacle anymore (==1, as pull up) and the flag3 ==1 (exit sensor 1 read an obstacle before) and flag4==0 (exit sensor 2 doesn't read an obstacle)
		}else if(GET_BIT(PINC,3) == 1 && flag3==1 && flag4==0){
			OCR1B=3000; // 0 degree (exit gate closed again)
			flag3=0;
		}
		// condition that check if the second exit sensor read an obstacle (==0, as pull up) and the flag4 ==0
		if(GET_BIT(PINC,4) == 0 && flag4==0){
			flag4=1;  //flag4 raised when exit sensor 2 read an obstacle
				}
		// condition that check if both flags equal 1 (both exit sensors read an obstacle)
		if(flag3==1 && flag4==1){
			slot = slot+1; // increase the number of free slots by one
			while(GET_BIT(PINC,4) == 0); // busy wait during exit sensor 2 read an obstacle (wait till the car pass the second exit sensor)
			OCR1B=3000; // 0 degree (exit gate closed)
			flag3=0, flag4=0;
				}


	}
}

/*
 * Function Name: Read_Sensor
 * Function Returns: void
 * Function Arguments: void
 * Function Description: used to check the status of slot sensors (if they are occupied or not)
 */
void Read_Sensor(){
S1=0, S2=0, S3=0, S4=0;

if(GET_BIT(PINB,4) == 0){S1=1;}
if(GET_BIT(PINB,5) == 0){S2=1;}
if(GET_BIT(PINB,6) == 0){S3=1;}
if(GET_BIT(PINB,7) == 0){S4=1;}

}

/*
 * Function Name: delay
 * Function Returns: void
 * Function Arguments: int
 * Function Description: used to delay an interval of time corresponding to the input argument
 */
void delay(int t){
	int i=0; // reset counter
	int x;
	// turn on CTC mode
	SET_BIT(TCCR0,3);
	CLR_BIT(TCCR0,6);
	// use prescalar 1024
	SET_BIT(TCCR0,0);
	SET_BIT(TCCR0,2);
	CLR_BIT(TCCR0,1);
	// set value to 125
	OCR0=125;
	if (t==1){
        x=125; // number of overflows to get 1 second
	}else if (t==2){
	    x=250; // number of overflows to get 2 seconds
	}else if (t==4){
	    x=500; // number of overflows to get 4 seconds
	}else if (t==6){
	    x=750; // number of overflows to get 6 second
	}


	while(i<=x){

	   while(GET_BIT(TIFR,1)==0); // busy wait till the register to be full and the flag to be raised
	   TCNT0=0b00000000; // clear the register to be counted in
	   i++; // increment counter
	   SET_BIT(TIFR,1); // clear Timer0 output compare flag bit(OCF0)
		}
		TCCR0=0x00; //reset the timer register
}
