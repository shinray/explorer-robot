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

// FreeRTOS stuff
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"

// ucr stuff, temp stuff
//#include <util/delay.h>
#include "usart_ATmega1284.h"
#include "lcd.h"
#include "pwm.h"

//============== RTOS STUFF=======================
void StartPulse(void (*func)(int), char* name, unsigned portBASE_TYPE Priority)
{
	xTaskCreate(func, (signed portCHAR *)name, configMINIMAL_STACK_SIZE, NULL, Priority, NULL);
}

// global vars
unsigned char lastInput = 0; // holds the LAST INPUT from UART
unsigned char spd[11] = {140, 120, 100, 80, 60, 50, 40, 30, 20, 10, 0}; //slow to fast
unsigned char speed = 255; // aka stop_motors()

//============== Radio SM ========================
enum input_states {IINIT, ILISTEN} instate;

void inputTick()
{
	unsigned static char recv;
	switch(instate)
	{
		case IINIT:
			lastInput = NULL;
			speed = 255;
			instate = ILISTEN;
			break;
		case ILISTEN:
			if (USART_HasReceived(1)) {
				recv = USART_Receive(1);
				switch(recv) {
					case '0':
						speed = spd[0];
						break;
					case '1':
						speed = spd[1];
						break;
					case '2':
						speed = spd[2];
						break;
					case '3':
						speed = spd[3];
						break;
					case '4':
						speed = spd[4];
						break;
					case '5':
						speed = spd[5];
						break;
					case '6':
						speed = spd[6];
						break;
					case '7':
						speed = spd[7];
						break;
					case '8':
						speed = spd[8];
						break;
					case '9':
						speed = spd[9];
						break;
					case 'q':
						speed = spd[10];
						break;
					case 'F':
					case 'B':
					case 'L':
					case 'G':
					case 'H':
					case 'R':
					case 'I':
					case 'J':
					case 'D':
					case 'S':
						lastInput = recv;
						break;
					default:
						break;
				}
			} else lastInput = NULL;
			break;
		default:
			instate = IINIT;
			break;
	}
}

void inputTask()
{
	instate = IINIT;
	for(;;)
	{
		inputTick();
		vTaskDelay(50);
	}
}

//============== Motor SM =========================
enum motor_states {MINIT, MSTOP, MMOVE} motorstate;
	
void motorTick()
{
	switch(motorstate) {
		case MINIT:
			init_motors();
			motorstate = MSTOP;
			break;
		case MSTOP:
			stop_motors();
			if (lastInput != 'S' && lastInput != NULL) {
				motorstate = MMOVE;
			}
			break;
		case MMOVE:
			if (lastInput == 'S' || lastInput == 'D')
			{
				motorstate = MSTOP;
			}else {
				switch(lastInput) {
					// forward
					case 'F':
						motor1_forward(speed);
						motor2_forward(speed);
						break;
					// backward
					case 'B':
						motor1_backward(speed);
						motor2_backward(speed);
						break;
					// left
					case 'L':
						motor1_backward(speed);
						motor2_forward(speed);
						break;
					// right
					case 'R':
						motor1_forward(speed);
						motor2_backward(speed);
						break;
					// forward left
					case 'G':
						motor1_backward((255-speed)/2 + speed);
						motor2_forward(speed);
						break;
					// back left
					case 'H':
						motor1_forward((255-speed)/2 + speed);
						motor2_backward(speed);
						break;
					// forward right	
					case 'I':
						motor1_forward(speed);
						motor2_backward((255-speed)/2 + speed);
						break;
					// back right
					case 'J':
						motor1_backward(speed);
						motor2_forward((255-speed)/2 + speed);
						break;
					// disconnect or stop
					default:
						stop_motors();
						break;
				}
			}
			break;
		default:
			motorstate = MINIT;
			break;
	}
	//switch(lastInput) {
		//case 'S':
			//motorstate = MSTOP;
			//break;
		//case 'F':
			//motorstate = MFORW;
			//break;
		//case 'B':
			//motorstate = MBACK;
			//break;
		//case 'L':
			//break;
		//case 'R':
			//break;
		//case '0':
			//speed = spd[0];
			//break;
		//case '1':
			//speed = spd[1];
			//break;
		//case '2':
			//speed = spd[2];
			//break;
		//case '3':
			//speed = spd[3];
			//break;
		//case '4':
			//speed = spd[4];
			//break;
		//case '5':
			//speed = spd[5];
			//break;
		//// technically we don't support speeds above 6
		//case '6':
			//speed = spd[5];
			//break;
		//case '7':
			//speed = spd[5];
			//break;
		//case '8':
			//speed = spd[5];
			//break;
		//case '9':
			//speed = spd[5];
			//break;
		//case 'q':
			//speed = spd[5];
			//break;
		//// disconnect
		//case 'D':
			////stop_motors();
			//motorstate = MSTOP;
			//break;
		//default:
			//break;
	//}
}

void motorTask()
{
	motorstate = MINIT;
	for(;;) {
		motorTick();
		vTaskDelay(1000);
	}
}

int main(void) {
	
	// d6,d7 b3,b4for dc motor
	// USART (bluetooth) - rx1(in), tx1(out) (d0 and d1)
	DDRB = 0xFF;	PORTB = 0x00;
	DDRD = 0b11111010; PORTD = 0b00000101;
	
	StartPulse(inputTask, "btlisten", 1);
	StartPulse(motorTask, "driver", 1);
	
	return 0;
}



//
//int main(void)
//{
	//DDRD = 0xE2; PORTD = 0x1D; // D0 = RX, D1 = TX, D5-D7 LCD control lines
	//DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	//LCD_init();
	//initUSART(0);
	//
	//StartPulse(inputTask, "btlisten", 1);
	//
	//unsigned char counter = 0;
	////unsigned char muhBuffer[256];
	//unsigned char input = 0;
	//LCD_ClearScreen();
	//LCD_DisplayString(1,"Hello");
    //while (1) 
    //{
		//if (counter < 30) counter++;
		//else{
			//if (USART_HasReceived(0)){
				//LCD_Cursor(1);
				//input = USART_Receive(0);
				////LCD_DisplayString(1,input);
				//LCD_WriteData(input);
			//}
			//else LCD_ClearScreen();
		//}
		//_delay_ms(500);
    //}
//}

