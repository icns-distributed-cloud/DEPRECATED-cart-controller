#include <avr/io.h>
#include <avr/interrupt.h>
#include "Sensor.h"
#include "Motor.h"
#include "Timer_pwm.h"

void init_MOTOR(){
	
	//MotorPort &= ~(Motor_Mask);                                                                                                                                                                                                                                                                                                                
	MotorPort_DDR |= Motor_Mask;
	MotorPort_PWMDDR = 0x18;
	asm volatile(" NOP");

}
void MotorGoFoward()//직진
{	

//	MotorStop();
//	delay_1mSec(10);
//	PORTF = 0x02;
 	MotorPort |= (1<<MotorL_1)|(1<<MotorR_1);
	MotorPort &= ~((1<<MotorL_2)|(1<<MotorR_2));
	asm volatile(" NOP");
}
void MotorGoBackward()//후진 
{
//	PORTF = 0x03;
	MotorPort &= ~((1<<MotorL_1)|(1<<MotorR_1));
	MotorPort |= (1<<MotorL_2)|(1<<MotorR_2);
	asm volatile(" NOP");
}
void TurnRight()//오른쪽턴
{
//	PORTF = 0x04;
	OCR3A = 700;
	OCR3B = 600;
	MotorPort |= (1<<MotorL_1)|(1<<MotorR_2);
	MotorPort &= ~((1<<MotorL_2)|(1<<MotorR_1));
}
void TurnLeft() //왼쪽턴
{
//	PORTF = 0x05;
	OCR3A = 700;
	OCR3B = 600;
	
	MotorPort |= (1<<MotorL_2)|(1<<MotorR_1);
	MotorPort &= ~((1<<MotorL_1)|(1<<MotorR_2));
}
void MotorStop()//정지
{
//	PORTF = 0x06;
	MotorPort &= ~((1<<MotorL_2)|(1<<MotorR_1)|(1<<MotorL_1)|(1<<MotorR_2));
	asm volatile(" NOP");
}
void MotorCommand(char command)//유아트용 명령어
{	
	MotorPort |= 0x00;
	if ( command == 'w') avoid_PSD();
	else if ( command == 'x') MotorGoBackward();
	else if ( command == 's')
 	{   DDRE=1;
		MotorStop();
		PORTE = 1;
	
	}
	else if ( command == 'a')  TurnLeft();
	else if ( command == 'd')  TurnRight();
	else if ( command == 'q')  Left3();
	else if ( command == 'z')  Left4();
	else if ( command == 'e')  Right3();
	else if ( command == 'c')  Right4();

}
void Motor_Sonar(unsigned int x, unsigned int y)
{
	MotorGoFoward();
	OCR3A = (x);//필요한 만큼 조절
	OCR3B = (y);//필요한 만큼 조절
}
void Left1(int a)//Custom 좌회전
{
	MotorGoFoward();
	OCR3A = 1000 - a;//필요한 만큼 조절
	OCR3B = 1000;
}

void Left2(void)//강한 좌회전
{
	MotorGoFoward();
	OCR3A = 1000 - 400;
	OCR3B = 1000;
}

void Left3(void)//강한 좌회전
{
	MotorGoFoward();
	OCR3A = 1000 - 800;
	OCR3B = 1000;
}
void Left4(void)//강한 좌회전
{
	MotorGoFoward();
	OCR3A = 0;
	OCR3B = 1023;
}

void Right1(int b)//Custom 우회전
{
	MotorGoFoward();
	OCR3A = 1000;
	OCR3B = 1000-b;//필요한 만큼 조절
}


void Right2(void)//강한 우회전
{
	MotorGoFoward();
	OCR3A = 1023;
	OCR3B = 900-500;
}

void Right3(void)//강한 우회전
{
	MotorGoFoward();
	OCR3A = 1023;
	OCR3B = 900-800;
}
void Right4(void)//강한 우회전
{
	MotorGoFoward();
	OCR3A = 1023;
	OCR3B = 0;
}
