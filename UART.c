// S/W Environment : AVR Studio + WINAVR Compiler
// Target : M128
// Crystal: 16Mhz
//
// Made by chowk.
// UART init and basic function

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Buffer.h"
#include "UART.h"

#define F_CPU 16000000               // Clock Speed
//define BAUD1 115200        // R-PI
#define BAUD0 9600            // HC-06
//#define BAUD1 9600



// #define MYUBRR1 (F_CPU/8/BAUD1-1)      // Double the USART Transmission Speed
#define MYUBRR0 (F_CPU/8/BAUD0-1)

void init_UART0(void);
unsigned char rx0_char(void);
void tx0_char(unsigned char data);
//void uart1_init(void);



// UART1 initialize
// Char size: 8 bit, Parity: Disabled

/*
void init_UART1(void)
{
   unsigned int ubrr;   

   ubrr = MYUBRR1;         // Set baud rate

   UBRR1H = (unsigned char)(ubrr>>8);
   UBRR1L = (unsigned char)ubrr;
   // Double the USART Transmission Speed
   UCSR1A = (1<<U2X1); 
   // Enable receiver and transmitter
   UCSR1B = (1<<RXEN1)|(1<<TXEN1);
   // Set frame format: 8data, 1stop bit
   UCSR1C = (0<<USBS1)|(3<<UCSZ10);
}
*/

void init_UART0(void)
{
   unsigned int ubrr;   

   ubrr = MYUBRR0;         // Set baud rate

   UBRR0H = (unsigned char)(ubrr>>8);
   UBRR0L = (unsigned char)ubrr;
   // Double the USART Transmission Speed
   UCSR0A = (1<<U2X); 
   // Enable receiver and transmitter //RXCIE: SREG가 set되고 얘도 set이면 RXcomplete interrupt발생, UDRn Data를 읽어서 사용가능
   UCSR0B = (1<<RXEN)|(1<<TXEN)|(1 << RXCIE); 
   // Set frame format: 8data, 1stop bit // USBS1: stop bit, UCSZ10: 8bit data set
   UCSR0C = (0<<USBS)|(3<<UCSZ0); //원하면 9비트도 가능
}



unsigned char rx0_char(void)
{
   // Wait for data to be received
   while ( !(UCSR0A & (1<<RXC)) ); //receive 
   // Get and return received data from buffer
   return UDR0;
}


// UART1 을 이용한 출력
void tx0_char(unsigned char data)
{
   // Wait for empty transmit buffer
   while ( !(UCSR0A & (1<<UDRE))); //transmit buffet의 상태 표시 set이면 empty
   // Put data into buffer, sends the data
   UDR0 = data;
}


/* 16진수로 바꾸기
void SCI_OutUHex(unsigned short number){
  if(number >= 0x10) {
    SCI_OutUHex(number / 0x10);
    SCI_OutUHex(number % 0x10);
  }
  else if(number < 0xA){
    SCI_OutChar(number + '0');
  }
  else{
    SCI_OutChar((number - 0x0A) + 'A');
  }
} 
*/
/*
unsigned char rx1_char(void)
{
   // Wait for data to be received
   while ( !(UCSR1A & (1<<RXC1)) );
   // Get and return received data from buffer
   return UDR1;
}

// UART1 을 이용한 출력
void tx1_char(unsigned char data)
{
   // Wait for empty transmit buffer
   while ( !(UCSR1A & (1<<UDRE1)));
   // Put data into buffer, sends the data
   UDR1 = data;
}
*/

//-------------------------SCI_OutChar to USART------------------------

void SCI_OutChar(char letter){ 
   tx0_char(letter);
}

//-------------------------SCI_InChar from USART------------------------

unsigned short SCI_InChar(){ 
  return (rx0_char());
}

void SCI_OutString(char *pt)
{ char letter;
  while((letter = *pt++)){
    SCI_OutChar(letter);
  }
}

 //USART1, Rx Interrupt 처리 Routine                                           

/*
ISR(USART0_RX_vect)
{
	char c;
	// 입력된 문자를 처리 하는 동안 새로운 Interrupt가 발생 하지 않도록
	// Rx Interrupt를 Disable 한다.
	UCSR0B &= ~(1 << RXCIE0);   // Rx Complete Interrupt disable

	c = rx0_char();

	if(c=='a') Add_Speed+=25;
	
	UCSR0B |= (1 << RXCIE0); 
}*/
/*
ISR(USART0_RX_vect)
{
   char c;

   // 입력된 문자를 처리 하는 동안 새로운 Interrupt가 발생 하지 않도록
   // Rx Interrupt를 Disable 한다.
   UCSR0B &= ~(1 << RXCIE0);   // Rx Complete Interrupt disable

   c = UDR0;
   // 입력된 문자가 CR or BS Code 가 아닌 경우
   if(!((c == CR) | (c == BS))){
      // Buffer에 문자를 저장 한다.
      putchar2buffer(c);
      // Buffer에 문자를 저장 하는 것이 성공한 경우 문자를 모니터에 출력 한다.
      // buffer_full_flag 가 TRUE 인 경우는 현재 Buffer 가 Full 상태이기 때문에
      // 문자를 Buffer에 저장 하지 못한 경우 이다.
      if(buffer_full_flag == FALSE)tx0_char(c);
   }
   // 만약 입력된 문자가 BS Code인 경우 Buffer에서 문자 하나를 재거 한다.
   if(c == BS){
      backspace2buffer();
      tx0_char(BS);
   }

   // 새로운 문자를 받을 수 있도록 Rx Interrupt를 Enable 한다.
   UCSR0B |= (1 << RXCIE0); // Rx Complete Interrupt enable

   // 만약 입력된 문자가 CR Code인 경우 Tx Interrupt를 Enable 한다.
   // CTx Interrupt가 Enable 되면 Tx Interrupt 처리 Routine 에서
   // Buffer의 내용을 모니터에 출력 하게 된다.
   if(c == CR){
      UCSR0B |= (1 << TXCIE0); // Tx Complete Interrupt enable
      tx0_char(CR); tx0_char(LF);
   }
} 

// USART0, Tx Interrupt 처리 Routine
ISR(USART0_TX_vect)
{
   char c;

   // Tx Interrupt를 처리 하는 동안 새로운 Rx Interrupt가 발생 하지 않도록
   // Rx Interrupt를 Disable 한다.
   UCSR0B &= ~(1 << RXCIE0);   // Rx Complete Interrupt disable

   // Interrupt 처리를 하는 동안 새로운 Interrupt가 발생 하지 않도록
   // Tx Interrupt를 Disable 한다.
   UCSR0B &= ~(1 << TXCIE0);   // Tx Complete Interrupt disable

   // Buffer로 부터 하나의 문자를 가져 온다.
   c = getchar_buffer();
   // 만약 문자가 소문자이면 대문자로 변환 한다.
   if((c >= 'a') && (c <= 'z')) c &= 0b11011111;
   // Buffer로 부터 성공적으로 문자를 가져온 경우  
   if(c != 0){
      // 모니터에 문자를 출력 한다.
      tx0_char(c);
      // 새로운 문자를 출력 할 수 있도록 Tx Interrupt를 Ensable 한다.
      UCSR0B |= (1 << TXCIE0); // Tx Complete Interrupt enable
    }
   // 새로운 문자를 받을 수 있도록 Rx Interrupt를 Ensable 한다.
   else {
      tx0_char(CR); tx0_char(LF);
      UCSR0B |= (1 << RXCIE0); // Rx Complete Interrupt enable
   }
}
*/
