#include <avr/io.h>
//#define F_CPU 16000000
#include <util/delay.h>

//Simple Wait Function
void Wait()
{
	uint8_t i;
	for(i=0;i<50;i++)
	{
		_delay_loop_2(0);
		_delay_loop_2(0);
		_delay_loop_2(0);
	}

}

void main()
{
    // USE TIMER3 for ultrasonic sensor too, but reset the ultrasonic sensor timer
	//Configure TIMER3
	TCCR3A|=(1<<COM3A1)|(1<<COM3B1)|(1<<WGM31);        //NON Inverted PWM
	TCCR3B|=(1<<WGM33)|(1<<WGM32)|(1<<CS31)|(1<<CS30); //PRESCALER=64 MODE 14(FAST PWM)

	ICR3=4999;  //fPWM=50Hz (Period = 20ms Standard).

	DDRB|=(1<<PB6)|(1<<PB7);   //PWM Pins as Out

	while(1)
	{	//
		OCR3A=105;   //-90 degree  =250  //120 degrees (minumum is 100) goes counterclockwise
		Wait();
		
		OCR3A=155;   //-90 degree  =250  //120 degrees (minumum is 100) goes counterclockwise
		Wait();

		OCR3A=205;   //-90 degree  =250  //120 degrees (minumum is 100) goes counterclockwise
		Wait();
		
		OCR3A=255;   //-90 degree  =250  //120 degrees (minumum is 100) goes counterclockwise
		Wait();

		OCR3A=300;  //0 degree = 375    //310 is too far
		Wait();

		//OCR1A=500;  //90 degree

		//Wait();

	}
}