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
char buffer1[50], buffer2[50];
	
	init_DEVICES();	
	MotorStop();

	while(1){ 
		
		SONAR();

    // SONAR BLUETOOTH DISPLAY
		itoa(cnt1,buffer1,10); //char으로 변환
   		itoa(cnt2,buffer2,10);

    	SCI_OutChar('L'); 
		SCI_OutChar(32);  // space bar
    	SCI_OutString(buffer1); //엔코더값
    	SCI_OutChar(32);
    	SCI_OutChar('R');
		SCI_OutChar(32); 
    	SCI_OutString(buffer2);
   		SCI_OutChar(LF);  // 다음줄
   		SCI_OutChar(CR);
		
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

