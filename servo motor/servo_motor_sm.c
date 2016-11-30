enum servoState {SINIT, left, center, right} servo_state;

void servo_init(){
	servo_state = INIT;
}


void servo_Tick(){
    switch(servo_state){
        case SINIT:
        // USE TIMER3 for ultrasonic sensor too, but reset the ultrasonic sensor timer
        	//Configure TIMER3
        	TCCR3A|=(1<<COM3A1)|(1<<COM3B1)|(1<<WGM31);        //NON Inverted PWM
        	TCCR3B|=(1<<WGM33)|(1<<WGM32)|(1<<CS31)|(1<<CS30); //PRESCALER=64 MODE 14(FAST PWM)
        
        	ICR3=4999;  //fPWM=50Hz (Period = 20ms Standard).
        	
        	servo_state = right; //start from right and sweep left
            break;
        case left:
            OCR3A=300;   //0 degree left
            _delay_ms(2000);
            servo_state = middle;
            break;
        case middle:
            OCR3A=205;
            _delay_ms(2000);
            servo_state = right;
            break;
        case right:
            OCR3A=105; //180 degrees far right 
            _delay_ms(2000);
            servo_state = left;
            break;
        default:
            break;
    }
}

void servoTask(){
	servo_init();
	for(;;){
		servo_Tick();
		vTaskDelay(1000);
	}
}

void StartServoPulse(unsigned portBASE_TYPE Priority){
xTaskCreate(servoTask, (signed portCHAR *)"servoTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}