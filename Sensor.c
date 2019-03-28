//Header 파일 선언부
#define  F_CPU 16000000UL // 16 MHz
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Motor.h"
#include "Timer_pwm.h"
#include "Sensor.h"
#include "UART.h"

/*******FOR ENCODER TIMER*************/
//unsigned int time=0x0000;

/*******FOR PID CONTROL*************/
volatile double KP = 0.02835;
volatile double KI = 0.02169;
volatile double KD = -0.001494;
volatile float delta_t = 0.1;
volatile int limit = 300;

/*************************SONAR*****************************/

void init_SONAR(void){

	DDRF &= ~(0x0C);
    DDRF|=0x03; // 트리거
	TCCR0 = 0b00001000;
}

void SONAR(){
	
	cli();
	//초음파 값 받는 함수
	
	// TCNTx - 타이머/카운터 레지스터 (실제 카운트되는 수치가 저장되는 레지스터)
	// TCCRx - 타이머/카운터 제어 레지스터(주로 동작모드 및 프리스케일러 설정) 

/******TIMER1 사용한 초음파 센서 코드******/
/*
  	trig1_1;      _delay_us(10); trig1_0;  //초음파 트리거1
    while(!echo1); TCNT1=0;      TCCR1B=2; //high가 되면, 카운터 시작, 8분주=0.5us
    while( echo1); TCCR1B=0; //low가 되면 끝
    cnt1=TCNT1/116; //cm변환
    _delay_ms(50);
    
    trig2_1;       _delay_us(10); trig2_0; //초음파 트리거2
    while(!echo2); TCNT1=0;      TCCR1B=2; //high가 되면, 카운터 시작, 8분주=0.5us
    while( echo2); TCCR1B=0; //low가 되면 끝
    cnt2= TCNT1/116; //cm변환
    _delay_ms(50);
*/
/******확장 TIMER0 사용한 초음파 센서 코드******/
 	TCNT0_H=0;
    trig1_1;      _delay_us(10); trig1_0;  //초음파 트리거1
    while(!echo1); 
	TCNT0=0;      TCCR0|=2; //high가 되면, 카운터 시작, 8분주=0.5us
    while( echo1) {if(TCNT0==255){TCNT0_H++; TCNT0=0;}} //low가 되면 끝
	TCCR0 &= 0x08; //카운터 정지//
    range_L_L = TCNT0; 
	range_L_H = (TCNT0_H*256);
	cnt1=(range_L_L+range_L_H)/116; //cm변환
	TCNT0_H=0; TCNT0=0;
    _delay_ms(50);
	
    trig2_1;       _delay_us(10); trig2_0; //초음파 트리거2
    while(!echo2); 
	TCNT0=0;      TCCR0|=2; //high가 되면, 카운터 시작, 8분주=0.5us
    while( echo2){if(TCNT0==255){TCNT0_H++; TCNT0=0;}} //low가 되면 끝
	TCCR0 &= 0x08; //카운터 정지//
	range_R_L = TCNT0; 
	range_R_H = (TCNT0_H*256);
	cnt2=(range_R_L+range_R_H)/116; //cm변환
	TCNT0_H=0; TCNT0=0;
    _delay_ms(50);

	// normalize range from 0 to 600
	if (cnt1 > NORM_MAX) cnt1 = NORM_MAX;
	if (cnt1 < NORM_MIN) cnt1 = NORM_MIN;
	if (cnt2 > NORM_MAX) cnt2 = NORM_MAX;
	if (cnt2 < NORM_MIN) cnt2 = NORM_MIN;


	norm1 = ((float)cnt1 - NORM_MIN)/(NORM_MAX - NORM_MIN) * RANGE_MAX;
	norm2 = ((float)cnt2 - NORM_MIN)/(NORM_MAX - NORM_MIN) * RANGE_MAX;
	// sonar range MIN: 1, MAX: 400
	// speed range MAX: 1023

	// W1_MIN, W2_MIN: 400
	diff1 = norm2 - norm1;
	diff2 = norm1 - norm2;
	//n_v1 = diff1 * diff1 * diff1 + W1_MIN;
	//n_v2 = diff2 * diff2 * diff2 + W2_MIN;

	n_v1 = diff1 * 10 + W1_MIN;
	n_v2 = diff2 * 10 + W2_MIN;
	
	PID(n_v1,n_v2);
	sei();
}
//

/*************************PSD*****************************/


void init_PSD()
{
	DDRA = 0x00;   //A를 입력핀 으로 사용 왼쪽 PSD
	DDRC = 0x00; //B를 입력핀 으로 사용 오른쪽 PSD
	//DDRF &= 0b00111111;

}

void get_PSD()  
{		
		char PA[8]= {0,0,0,0,0,0,0,0};
		char PC[8]= {0,0,0,0,0,0,0,0};
		char CC;

		for(int i=0 ; i<50; i++)
		{CC=128;
			for(int i=0; i<8 ; i++){
			if((PINA & CC) != 0) PA[7-i]++;
			CC /= 2;
			}
		CC=128;
			for(int i=0; i<8; i++){
			if((PINC & CC) != 0) PC[7-i]++;
			CC /= 2;
			}		
		}

		for(int i=0; i<8; i++)
		{
			if(PA[7-i]>40) PA[7-i]=1;
			else           PA[7-i]=0;
			if(PC[7-i]>40) PC[7-i]=1;
			else           PC[7-i]=0;
		}
		
		for(int i=0; i<7; i++)
		{
			if(PA[i]*PA[i+1]==1) FPA[i]=1;
			else FPA[i]=0;
			if(PC[i]*PC[i+1]==1) FPC[i]=1;
			else FPC[i]=0;
		}
	
	weightR=0;
	weightL=0;
	unsigned int temp=1;
	
	for(int i=0; i<7; i++)
	{
		weightL += FPA[6-i]*temp;
		weightR += FPC[i]*temp;
		temp*=2;
	}
	if(weightL > 8 ||  weightR > 8) //FORNT OBS
	{		if(weightL > weightR) drive=1;
			else				  drive=2;
	}
	else if(weightL != 0 || weightR != 0) //SIDE OBS
	{		if(weightL > weightR) drive=3;
			else				  drive=4;
	}
	else 								//NO OBS
	{
		drive =0;
	}	

}	

void drive_PSD()  //drive 가 홀수면 우회전 짝수면 좌회전
{
	if(drive == 1) 
	{TurnRight();
   	_delay_ms(10);
	}
	else if(drive == 2) 
	{TurnLeft();
	_delay_ms(10);  
    }
	else if(drive == 3) 
	{
	MotorGoFoward();
	OCR3A = 1000 ;
	OCR3B = 900- weightL*100;
	}
	else if(drive == 4) 
	{
	MotorGoFoward();
	OCR3A = 900 - weightL*100;
	OCR3B = 1000 ;
	}
 	else					//NO obs
	{
	OCR3A = 1000;
	OCR3B = 1000;
	MotorGoFoward();
	}
}

void avoid_PSD()		
{
	get_PSD();
	/*if(drive==0) 
	{y_SONAR();}

	else 
	{
	drive_PSD(); 
	}
	*/

}

/*************************ENCODER*****************************/

void init_ENCODER()
{
	EICRA=0x0a; //외부 인터럽트 INT0, INT2는  FALLING_EDGE로 함
 	EIMSK=0x03; //외부 인터럽트 INT0, INT2만 허가, INT0는 PD0 / INT2는 PD2에서 사용됨
	TCCR1A = 0;
  	TCCR1B = 0;
  	timer1_counter = 59286;   // preload timer 65536-16MHz/256/2Hz (34286 for 0.5sec) (59286 for 0.1sec)
  	TCNT1 = timer1_counter;   // preload timer
  	TCCR1B |= (1 << CS12);    // 256 prescaler 
  	TIMSK |= (1 << TOIE1);   // enable timer overflow interrupt

}

ISR(INT0_vect) 
{
	count_L++;
}

ISR(INT1_vect)
{
	count_R++;
}

ISR(TIMER1_OVF_vect){//0.1s

	TCNT1 = timer1_counter;   // set timer
  	M_L=count_L;
	count_L=0;
	
  	M_R=count_R;
	count_R=0;

}

void PID(unsigned int x, unsigned int y) {          // PID 제어 함수
	
	Desired_Speed_L = n_v1;    // 목표 회전수
	Desired_Speed_R = n_v2;

	Measured_Encoder_L = M_L;
	Measured_Encoder_R = M_R;

	Error_L = Desired_Speed_L - 145*exp(0.0015*Measured_Encoder_L);
	Error_R = Desired_Speed_R - 145*exp(0.0015*Measured_Encoder_R);

	Motor_Signal_L = Old_Motor_L + KP*(Error_L - Old_Error_L) + KI*delta_t*(Error_L + Old_Error_L)/2 + (KD/delta_t) * (Error_L - 2 * Old_Error_L + Old_Error_2_L);
	Motor_Signal_R = Old_Motor_R + KP*(Error_R - Old_Error_R) + KI*delta_t*(Error_R + Old_Error_R)/2 + (KD/delta_t) * (Error_R - 2 * Old_Error_R + Old_Error_2_R);

	//update new speed

	MotorGoFoward();
	if (Motor_Signal_L <900 && Motor_Signal_L >400){
 		OCR3A = Motor_Signal_L;  //set motor speed 
	}
	else{
		if (Motor_Signal_L>900){
			OCR3A = 900;
		}
		else if (Motor_Signal_L<400){
			OCR3A = 400;
		}
	}
	if (Motor_Signal_R <900 && Motor_Signal_R >400){
 		OCR3B = Motor_Signal_R;   //set motor speed 
	}
	else{
		if (Motor_Signal_R>900){
			OCR3B = 900;
		}
		else if (Motor_Signal_R<400){
			OCR3B = 400;
		}
	}
	
 	Old_Motor_L = Motor_Signal_L; // update
 	Old_Motor_R = Motor_Signal_R; // update

	Old_Error_2_L = Old_Error_L;
 	Old_Error_2_R = Old_Error_R;

	Old_Error_L = Error_L;
	Old_Error_R = Error_R;


}

