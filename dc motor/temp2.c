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

//#define F_CPU 1000000 //must define this
//#define F_CPU 16000000
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
#include "pwm.h"
//#include "keypad.h"

//--------------------------------------------------------Ultrasonic TASK BELOW------------------------------------------------

enum UltrasonicState {UINIT,capture_distance} ultrasonic_state;

void ultrasonic_init(){
	ultrasonic_state = UINIT;
	//ultrasonic sensor
	//_delay_ms(50);//giving delay of 50ms
	//US_DDR = 0b11111011; //set ultrasonic sensor as output w/ echo as input
	//
	//EIMSK |= (1<<INT0);
	//EICRA |= (1<<ISC00);
	//
	//TCCR1A = 0;
	
	//_delay_ms(50);//giving delay of 50ms
}

int16_t COUNTA = 0; //counts the number of clock cycles
char SHOWA[16]; //store the clock cycles as pulse in centimeters in a char
unsigned char j =0;
void ultrasonic_Tick(){
	switch(ultrasonic_state){
		case UINIT:
			ultrasonic_state = capture_distance;
			break;
		case capture_distance:
			//reset all the values?
			US_DDR = 0b11111011; //set ultrasonic sensor as output w/ echo as input
	
			EIMSK |= (1<<INT0); //enables PIND2(INT0) connected to ECHO to detect logic change
			EICRA |= (1<<ISC00); //enables any logic change on INT0 to generate interrupt request
	
			TCCR3A = 0; //
	
			//LCD_ClearScreen();
			//_delay_ms(50);
			sei();
		
			//NOTE: I'm using PORTD so I can use INT0 on PIND2***** IMPORTANT*****
			if(j == 0)
			{
				US_PORT |= (1<<PIND0); //high
				j++;
				break;
			}
			else{
				//_delay_us(15); //have to sample at a rate of at least 10microsecond
				US_PORT &= ~(1<<PIND0); //low
				//_delay_us(20);
			
				COUNTA = pulse/58;
				//command for putting variable number in LCD(variable number, in which character to replace, which base is variable(ten here as we are counting number in base10))
				itoa(COUNTA,SHOWA,10);
				LCD_DisplayString(1, SHOWA);
				LCD_Cursor(5);
				LCD_WriteData('c');
				LCD_Cursor(6);
				LCD_WriteData('m');

				_delay_ms(4000);
				//
				j = 0;
				break;
			}
		default:
			ultrasonic_state = UINIT;
			break;
	}
}

void ultrasonicTask(){
	ultrasonic_init();
	for(;;){
		ultrasonic_Tick();
		vTaskDelay(4);
	}
}

void StartUltrasonicPulse(unsigned portBASE_TYPE Priority){
	xTaskCreate(ultrasonicTask, (signed portCHAR *)"ultrasonicTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

//--------------------------------------------------------CAR MOVE TASK BELOW------------------------------------------------

enum car_move {INIT, forward, backward, stop} car_state;

void car_init(){
	car_state = INIT;
}

static unsigned char count = 0;
void car_Tick(){
	
	switch(car_state){
		case INIT:
			init_motors();
			//delay
			count = 0;
			car_state = forward;
			break;
		case forward:
			motor1_forward(0); //forward and backwards is reversed
			motor2_forward(0); 
			count++;
			//LCD_Cursor(9);
			itoa(count,SHOWA,10);
			LCD_DisplayString(1, SHOWA);
			if(count > 100)
			{
				count = 0;
				car_state = stop;
			}
			break;
		case backward:
			motor1_backward(140);
			motor2_backward(140);
			count++;
			if(count > 100)
			{
				count = 0;
				car_state = stop;
			}
			break;
		case stop:
			stop_motors();
			break;
		default:
			break;
	}
}

void carTask(){
	car_init();
	for(;;){
		car_Tick();
		vTaskDelay(100);
	}
}

void StartCarPulse(unsigned portBASE_TYPE Priority){
	xTaskCreate(carTask, (signed portCHAR *)"carTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
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
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRA = 0xFF; PORTA = 0x00; // LCD control lines
	DDRB = 0xFF; PORTB = 0xFF;

	// Initializes the LCD display
	LCD_init();
	//ultrasonic sensor old code----
	
	//ultrasonic sensor old code end----
	//LCD_DisplayString(1, "Hello World");

	//Start Tasks
	StartUltrasonicPulse(1);
	//StartCarPulse(2);
	//RunSchedular
	
	vTaskStartScheduler();
	
	return 0;
}

ISR(INT0_vect)
{
	if (i==1) //logic high to low
	{
		TCCR3B = 0x00; //stop timer/counter disables counter
		pulse = TCNT3;  //TC1 counter low/high byte, sets the pulse to the counter value
		TCNT3 = 0x00; //clear the counter value
		i=0;
	}
	if (i==0) //logic low to high
	{
		TCCR3B|=(1<<CS31); //set CS11 to 1 instead of CS10, so I can get the Prescaler = Fcpu/8
		i=1;
	}
}










////-----------------------------
//
//
//
//
//#include <stdint.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <stdbool.h>
//#include <string.h>
//#include <math.h>
//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <avr/eeprom.h>
//#include <avr/portpins.h>
//#include <avr/pgmspace.h>
//
////#define F_CPU 1000000 //must define this
//#define US_PORT PORTD //ultrasonic sensor PORTx
//#define US_DDR DDRD //ultrasonic sensor DDRx
//#define US_PIN0 PIND0 //ultrasonic sensor PINx0
////#include <util/delay.h> //must define F_CPU in order to use this
//
////header to enable delay function in program
//
//static volatile int pulse = 0;//integer  to access all though the program
//
//static volatile int i = 0;// integer  to access all though the program
//
////FreeRTOS include files
//#include "FreeRTOS.h"
//#include "task.h"
//#include "croutine.h"
//#include "lcd.h"
//#include "bit.h"
//#include "PWM.h"
//
////--------------------------------------------------------MASTER TASK BELOW------------------------------------------------
//
//enum car_move {INIT, forward, backward, stop} car_state;
//
//void car_init(){
//car_state = INIT;
//}
//
//
//void car_Tick(){
//static unsigned char count = 0;
//switch(car_state){
//case INIT:
//init_motors();
////delay
//count = 0;
//car_state = forward;
//break;
//case forward:
//motor1_forward(0); //forward and backwards is reversed
//motor2_forward(0);
//count++;
//if(count > 10)
//{
//count = 0;
//car_state = backward;
//}
//break;
//case backward:
//motor1_backward(0);
//motor2_backward(0);
//count++;
//if(count > 10)
//{
//count = 0;
//car_state = stop;
//}
//break;
//case stop:
//stop_motors();
//break;
//default:
//break;
//}
//}
//
//void carTask(){
//car_init();
//for(;;){
//car_Tick();
//vTaskDelay(1000);
//}
//}
//
//void StartCarPulse(unsigned portBASE_TYPE Priority){
//xTaskCreate(carTask, (signed portCHAR *)"carTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
//}
//
////distance(in cm) = width of pulse output (in uS) / 58
////check register summary
//
////EICRA External interrupt control register A = MCUCR
////7-0 ....... 1 = ISC01 ,0 = ISC00
//
////EIMSK External Interrupt Mask Register = GICR
////7-0 ....... 0 = INT0
//
////TCCR1B Timer/Counter1 Control Register B = TCCR1B
////MCU Control Register for ATmega1284
//
//int main(void)
//{
//DDRB = 0xFF; PORTB = 0xFF;
//DDRD = 0b11111011; PORTD = 0xFF;
////DDRA = 0x00; //PORTA = 0xFF:
////Start Tasks
//StartCarPulse(1);
////RunSchedular
//
//vTaskStartScheduler();
//
//return 0;
//}