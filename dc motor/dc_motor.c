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
#include "PWM.h"

//--------------------------------------------------------MASTER TASK BELOW------------------------------------------------

enum car_move {INIT, forward, backward, stop} car_state;

void car_init(){
	car_state = INIT;
}


void car_Tick(){
	static unsigned char count = 0;
	switch(car_state){
		case INIT:
			init_motors();
			//delay
			count = 0;
			car_state = forward;
			break;
		case forward:
			motor1_forward(140); //forward and backwards is reversed
			motor2_forward(140);
			count++;
			if(count > 10)
			{
				count = 0;
				car_state = stop;
			}
			break;
		case backward:
			motor1_backward(140);
			//motor2_backward(140);
			count++;
			if(count > 10)
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
		vTaskDelay(1000);
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
	DDRB = 0xFF; PORTB = 0xFF;
	DDRD = 0b11111011; PORTD = 0xFF;
	//Start Tasks
	StartCarPulse(1);
	//RunSchedular
	
	vTaskStartScheduler();
	
	return 0;
}