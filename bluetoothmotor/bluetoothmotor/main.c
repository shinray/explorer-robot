/*
 * bluetoothmotor.c
 *
 * Created: 15/11/2016 14:29:07
 * Author : Admin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/portpins.h>
#include <avr/pgmspace.h>

// ucr stuff
#include <util/delay.h>
#include "usart_ATmega1284.h"
#include "lcd.h"

int main(void)
{
	DDRD = 0xE2; PORTD = 0x1D; // D0 = RX, D1 = TX, D5-D7 LCD control lines
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	LCD_init();
	initUSART(0);
    /* Replace with your application code */
	unsigned char counter = 0;
	unsigned char muhBuffer[256];
	unsigned char input = 0;
	LCD_ClearScreen();
	LCD_DisplayString(1,"Hello");
    while (1) 
    {
		if (counter < 30) counter++;
		else{
			if (USART_HasReceived(0)){
				LCD_Cursor(1);
				input = USART_Receive(0);
				//LCD_DisplayString(1,input);
				LCD_WriteData(input);
			}
			else LCD_ClearScreen();
		}
		_delay_ms(500);
    }
}

