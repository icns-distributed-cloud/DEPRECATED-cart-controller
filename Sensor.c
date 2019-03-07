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

/*************************SONAR*****************************/

void init_SONAR(void){

	DDRF &= ~(0x0C);
    DDRF|=0x03; // 트리거
	TCCR0 = 0b00001000;
}

void get_SONAR(){

	//초음파 값 받는 함수
	
	// TCNTx - 타이머/카운터 레지스터 (실제 카운트되는 수치가 저장되는 레지스터)
	// TCCRx - 타이머/카운터 제어 레지스터(주로 동작모드 및 프리스케일러 설정) 

/******TIMER1 사용한 초음파 센서 코드******/

/*  	trig1_1;      _delay_us(10); trig1_0;  //초음파 트리거1
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
    while(!echo1); TCNT0=0;      TCCR0|=2; //high가 되면, 카운터 시작, 8분주=0.5us
    while( echo1) {if(TCNT0==255){TCNT0_H++; TCCR0=0;}} //low가 되면 끝
	TCCR0 &= 0x08; //카운터 정지//
    range_L_L = TCNT0; 
	range_L_H = (TCNT0_H*256);
	cnt1=(range_L_L+range_L_H)/116; //cm변환
	TCNT0_H=0; TCNT0=0;
    _delay_ms(50);

    TCNT0_H=0;
    trig2_1;       _delay_us(10); trig2_0; //초음파 트리거2
    while(!echo2); TCNT0=0;      TCCR0|=2; //high가 되면, 카운터 시작, 8분주=0.5us
    while( echo2); {if(TCNT0==255){TCNT0_H++; TCCR0=0;}} //low가 되면 끝
	TCCR0 &= 0x08; //카운터 정지//
	range_R_L = TCNT0; 
	range_R_H = (TCNT0_H*256);
	cnt2=(range_R_L+range_R_H)/116; //cm변환
	TCNT0_H=0; TCNT0=0;
    _delay_ms(50);
    
}
//

void y_SONAR(void){//핀설정 및 초음파센서 값 받아 모터값 연동할 함수
	get_SONAR();

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
	
	Motor_Sonar( n_v1 , n_v2 );

}


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
	//TCCR0 = (0<<FOC0)|(1<<WGM00)|(1<<COM01)|(0<<COM00)|(1<<WGM01)|(1<<CS02)|(0<<CS01)|(1<<CS00);  //FAST_PWM_MODE & CLK/256 (분주비:256)
	//TCNT0 = 0x00; //타이머 초기화
	//OCR0  = 0x00; //OUTPUT_COMPARE_REGISTER 0 초기화
	TIMSK=0x04;         //0000 0100 TCNT1 overflow interrupt enable
	TCCR1A = 0b00000000;    //0000 0000  normal mode 0~ 0xFFF
	TCCR1B = 0b00000011;     //00/0/0 0/100  N : 1             //FAST_PWM_MODE & CLK/256 (분주비:256)
	TCCR1C = 0x00;    //0000 0000
	TCNT1 = 0;     //tcnt1 = 0x8000;
}

ISR(INT0_vect) 
{
	count_L++;
}

ISR(INT1_vect)
{
	count_R++;
}

ISR(TIMER1_OVF_vect){//2.5ms 101=>256분주 111=>1024분주

	TCNT1=0; //타이머0을 가지고 0.0025초 오버플로 인터럽트 //144개의 카운트 
	RPM_L=count_L;//60*(count_L/200)/0.1;// rpm=(60*m)/(주기*분해능)=(60*m)/(1s*1000) // 나누기 기어비 
    RPM_R=count_R;//60*(count_R/200)/0.1;
	count_L=0;
	count_R=0;

}

/*
void PID(void) {          // PID 제어 함수

pwmduty = OCR3A;
desired = (pwmduty/256)*920;     // 목표 회전수 계산. MAX rpm = 920. //초음파 센서 값 삽입
measured = (1/encT_0)*60;        // ex_int0에서 검출한 회전주기를 이용해 rpm 검출
  
 // calculate the motor signal according the PID equation.
 // the derivative and the integral are approximated using simple linear approximations.

 error_funct = desired - measured;
 motor_signal0 = old_motor_signal + KP * (error_funct - old_error_funct) + KI * delta_t * (error_funct + old_error_funct) / 2 + (KD / delta_t) * (error_funct - 2 * old_error_funct + old_error_funct2);
 // 선형근사를 이용한 출력 계산식.
 if (motor_signal0 < 0) { // limiting the output
  motor_signal0 = 0;
 }
 if (motor_signal0 > limit) {
  motor_signal0 = limit;
 }

 old_motor_signal = motor_signal0; // update
 old_error_funct2 = old_error_funct;
 old_error_funct = error_funct;

}
*/
