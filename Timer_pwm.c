#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer_pwm.h"

void init_TIMER3(void)
{
	// Timer3를 이용 하여 motor speed 제어
	// fastpwm mode로 속도 제어
	// (1 << COM3A1) | (0 << COM3A0) : OC3A,OC3B로 pwm출력
	// TCNT3 의 값이 OCR3A, OCR3B값과 같을 때 clear, TCNT3 값이 0일 때 set
	//  (0 << WGM33) | (1 << WGM32)  | (1 << WGM31) | (1 << WGM30) : Fast pwm,10-bit 
	TCCR3A = (1 << COM3A1) | (0 << COM3A0) | (1 << COM3B1) | (0 << COM3B0) | (1 << WGM31) | (1 << WGM30);  
	//	(0 << CS12) | (1 << CS11) | (1 << CS10) : 010  prescale clk/64, Clock 주기 : 4uSec
	//  (0 << CS12) | (0 << CS11) | (0 << CS10) : 000  일 때 No Clock(Timer 정지 상태) 
	TCCR3B = (0 << WGM33) | (1 << WGM32) | (0 << CS32) | (1 << CS31) | (1 << CS30);  
	OCR3A = PWM_Default_A;  // OC3A에 의한 펄스 주기 : 200 * 0.5 uSec = 100uSec
	OCR3B = PWM_Default_B;
	TCNT3 = 0x0000;			   // Timer/Counter3 Register 초기화

	// TICIE1 ; Timer3 Output Compare A Match Interrupt Disable.
	ETIMSK &= ~((1 << OCIE3B)|(1 << OCIE3A));
}
void init_TIMER1(void)
{
	// Timer1를 이용 하여 motor speed 제어
	// fastpwm mode로 속도 제어
	// (1 << COM3A1) | (0 << COM3A0) : OC3A,OC3B로 pwm출력
	// TCNT3 의 값이 OCR3A, OCR3B값과 같을 때 clear, TCNT3 값이 0일 때 set
	//  (0 << WGM33) | (1 << WGM32)  | (1 << WGM31) | (1 << WGM30) : Fast pwm,10-bit 
	TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0) | (1 << WGM11) | (1 << WGM10);  
	//	(0 << CS12) | (1 << CS11) | (1 << CS10) : 010  prescale clk/64, Clock 주기 : 4uSec
	//  (0 << CS12) | (0 << CS11) | (0 << CS10) : 000  일 때 No Clock(Timer 정지 상태) 
	TCCR1B = (0 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (1 << CS10);  
	OCR1A = PWM_Max;  // OC3A에 의한 펄스 주기 : 200 * 0.5 uSec = 100uSec 왼쪽
	OCR1B = PWM_Max;  // 오른쪽
	TCNT1 = 0x0000;			   // Timer/Counter3 Register 초기화

	// TICIE1 ; Timer3 Output Compare A Match Interrupt Disable.
	ETIMSK &= ~((1 << OCIE1B)|(1 << OCIE1A));
}

