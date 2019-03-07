
#define TRUE 0x01
#define FALSE 0x00

void init_BUFFER();
void putchar2buffer(unsigned char c);
unsigned char getchar_buffer(void);
void backspace2buffer(void);

extern volatile signed int rx0_data_counter;
extern volatile signed char buffer_full_flag;

