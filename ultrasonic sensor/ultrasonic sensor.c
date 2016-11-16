#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/portpins.h>
#include <avr/pgmspace.h>

#define F_CPU 1000000 //must define this 
#define US_PORT PORTD //ultrasonic sensor PORTx
#define US_DDR DDRD //ultrasonic sensor DDRx
#define US_PIN0 PIND0 //ultrasonic sensor PINx0
#include <util/delay.h> //must define F_CPU in order to use this
//header to enable delay function in program

static volatile int pulse = 0;//integer  to access all though the program

static volatile int i = 0;// integer  to access all though the program

//FreeRTOS include files
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "lcd.h"
#include "bit.h"
#include "keypad.h"

//--------------------------------------------------------MASTER TASK BELOW------------------------------------------------

enum masterState {INIT,transmit1,transmit2} master_state;

void master_init(){
	master_state = INIT;
}


void master_Tick(){
}

void masterTask(){
	master_init();
	for(;;){
		master_Tick();
		vTaskDelay(1000);
	}
}

void StartMasterPulse(unsigned portBASE_TYPE Priority){
xTaskCreate(masterTask, (signed portCHAR *)"masterTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

//distance(in cm) = width of pulse output (in uS) / 58
//check register summary

//EICRA External interrupt control register A = MCUCR
//7-0 ....... 1 = ISC01 ,0 = ISC00

//EIMSK External Interrupt Mask Register = GICR
//7-0 ....... 0 = INT0

//TCCR1B Timer/Counter1 Control Register B = TCCR1B
//MCU Control Register for ATmega1284

int main(void)
{
	//DDRD = 0xF0; PORTD = 0x0F; //KEYPAD
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRA = 0xFF; PORTA = 0x00; // LCD control lines
	
	// Initializes the LCD display
	LCD_init();
	
	//ultrasonic sensor
	_delay_ms(50);//giving delay of 50ms
	US_DDR = 0b11111011; //set ultrasonic sensor as output w/ echo as input
	
	EIMSK |= (1<<INT0);
	EICRA |= (1<<ISC00);
	
	TCCR1A = 0;
	
	int16_t COUNTA = 0; //counts the number of clock cycles
	char SHOWA[16]; //store the clock cycles as pulse in centimeters in a char

	
	LCD_ClearScreen();
	_delay_ms(50);
	sei();
	//LCD_DisplayString(1, "Hello World");
	while(1)
	{
		//NOTE: I'm using PORTD so I can use INT0 on PIND2***** IMPORTANT*****
		US_PORT |= (1<<PIND0); //high
		_delay_us(15); //have to sample at a rate of at least 10microsecond
		US_PORT &= ~(1<<PIND0); //low
		_delay_us(20);
		
		COUNTA = pulse/58;
		//command for putting variable number in LCD(variable number, in which character to replace, which base is variable(ten here as we are counting number in base10))
		itoa(COUNTA,SHOWA,10);
		LCD_DisplayString(1, SHOWA);
		LCD_Cursor(5);
		LCD_WriteData('c');
		LCD_Cursor(6);
		LCD_WriteData('m');
		//LCD_WriteData(COUNTA + '0');
		//LCD_Cursor(1);
		_delay_ms(5000);
		//

	}
	//Start Tasks
	//RunSchedular
	
	//vTaskStartScheduler();
	
	return 0;
}

ISR(INT0_vect)
{
	if (i==1) //logic high to low
	{
		TCCR1B = 0x00; //stop timer/counter
		pulse = TCNT1;  //TC1 counter low/high byte, sets the pulse to the counter value
		TCNT1 = 0x00; //clear the counter
		i=0;
	}
	if (i==0) //logic low to high
	{
		TCCR1B|=(1<<CS11); //set CS11 to 1 instead of CS10, so I can get the Prescaler = Fcpu/8
		i=1;
	}
}
