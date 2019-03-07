#define CR 0x0D
#define LF 0x0A
#define BS 0x08


#ifndef SPACE
   #define SPACE   0x20
#endif

signed char CR_flag;

//void uart0_init();
void init_UART1();
void init_UART0();
unsigned char rx0_char(void);
void tx0_char(unsigned char data);
//void SCI_OutUHex(unsigned short number)
void SCI_OutString(char *pt);
void SCI_OutChar(char letter);




//-------------------------SCI_OutChar to USART------------------------


//-------------------------SCI_InChar from USART------------------------
