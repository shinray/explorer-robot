//layout for hanging end of mount pointing to the right and wires on the left
//the pointer is facing towards me
//0CR3A = 105 facing the right
//OCR3A = 205 facing the center
//OCR3A = 300 facing the left

void servo_middle()
{
     
    OCR3A=205;   //middle position 90 degrees from 0 degree(left)
    
    //delay in state machine to give enough time for the servo to turn!!!!****
}

void servo_left_90()
{
    OCR3A=300; //180 degrees far right 
}

void servo_right_90()
{
    //zero(105) is left when wires facing me
    OCR3A=105;   //0 degree left
}

void servo_left_45()
{
    OCR3A=155; //45 degrees from 0
}

void servo_right_45()
{
    OCR3A=255; //135 degrees from 0
}

void init_servo()
{
    // USE TIMER3 for ultrasonic sensor too, but reset the ultrasonic sensor timer
	//Configure TIMER3
	TCCR3A|=(1<<COM3A1)|(1<<COM3B1)|(1<<WGM31);        //NON Inverted PWM
	TCCR3B|=(1<<WGM33)|(1<<WGM32)|(1<<CS31)|(1<<CS30); //PRESCALER=64 MODE 14(FAST PWM)

	ICR3=4999;  //fPWM=50Hz (Period = 20ms Standard).

	DDRB|=(1<<PB6)|(1<<PB7);   //PWM Pins as Out (remove this set it during main)
	
}