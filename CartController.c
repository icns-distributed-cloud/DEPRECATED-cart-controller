/**************************************************************
TrashCarrier
Made by HHC
**************************************************************/
#  ifndef atmega128
	#define atmega128
#  endif
#define FOSC 16000000// Clock Speed

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Timer_pwm.h"
#include "UART.h"
#include "Motor.h"
#include "Buffer.h"
#include "Sensor.h"

int put_char(char c, FILE *stream)
{
	tx0_char(c);
	return 0;   
}

void init_DEVICES(void)
{
	cli();         			// Disable all interrupts
	fdevopen(put_char,0);
	init_UART0();
	init_BUFFER();
	init_MOTOR();
	init_TIMER3();	
	init_SONAR();
	init_ENCODER();
	init_PSD();
	sei();                 // Re-enable interrupts
}

//char command;
int main(){

	init_DEVICES();	
	MotorStop();

	while(1)
	{ 
	//MotorGoFoward();
	//OCR3A = 300+Add_Speed;
	//OCR3B = 300+Add_Speed;
//	OCR3A = 600;
//	OCR3B = 600;
	PID();

	}			 	
	return 0;
}
/*
SIGNAL(USART0_RX_vect)
{
	char c;
	// 입력된 문자를 처리 하는 동안 새로운 Interrupt가 발생 하지 않도록
	// Rx Interrupt를 Disable 한다.
	UCSR0B &= ~(1 << RXCIE0);	// Rx Complete Interrupt disable
	
	c = command;
	command = c;
	tx0_char(c);
	MotorCommand(c);
	
	UCSR0B |= (1 << RXCIE0);
}*/

