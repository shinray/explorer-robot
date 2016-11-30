/*
 * CFile1.c
 *
 * Created: 11/15/2016 10:29:22 PM
 *  Author: Edward
 */ 

//inverted mode 
void motor1_forward(unsigned char pwm)
{
	OCR0A = pwm;
	OCR0B = 255;
}

void motor1_backward(unsigned char pwm)
{
	OCR0A = 255;
	OCR0B = pwm;
}

void motor2_forward(unsigned char pwm)
{
	OCR2A = pwm;
	OCR2B = 255;
}

void motor2_backward(unsigned char pwm)
{
	OCR2A = 255;
	OCR2B = pwm;
}

void stop_motors()
{
	OCR0A = 255;
	OCR0B = 255;
	OCR2A = 255;
	OCR2B = 255;	
}

void init_motors()
{
	TCCR0A = TCCR2A = 0xF3; //set up the inverted PWM output for Timer0 and Timer3
	TCCR0B = TCCR2B = 0x02; //set f_cpu/8
	stop_motors();
	//speed = ?
}