
/**************************** [SONAR] ******************************************
PF0,2 >> 왼쪽 초음파 [트리거 & 에코]
PF1,3 >> 오른쪽 초음파 [트리거 & 에코]
*****************************************************************************/

#define trig1_1 PORTF|=1  // PF0, srf04-1 트리거
#define trig1_0 PORTF&=~1 // PF0, srf04-1 트리거
#define echo1   (PINF&4)  // PF2, srf04-1 에코
//
#define trig2_1 PORTF|=2  // PF1, srf04-2 트리거
#define trig2_0 PORTF&=~2 // PF1, srf04-2 트리거
#define echo2   (PINF&8)  // PF3, srf04-2 에코
//
//정규화 변수 선언
unsigned int n_v1, n_v2; // 정규화 된 값
float norm1,norm2,diff1,diff2;

//Define : 모터 왼&오 최소값(W1, W2), 정규화 Min&Max 값(Norm), 범위 보정값(Range) 
#define W1_MIN 400
#define W2_MIN 400
#define RANGE_MAX 60
#define NORM_MIN 3
#define NORM_MAX 180

unsigned int  cnt1,cnt2;

void get_SONAR();
void y_SONAR(void);
void init_SONAR(void);

/**************************** [PSD] ******************************************
PORTA >> 왼
PORTB >> 오
*****************************************************************************/

char PSD_SIDE; 
char PSD_FRONT;
 
char FPA[7];
char FPC[7];

char drive;
unsigned int weightR;
unsigned int weightL;

void init_PSD();	
void get_PSD();
void avoid_PSD();
void drive_PSD();

/*********************************[ENCODER]***********************************
INT0 (PD0) >> 왼
INT1 (PD1) >> 오
*****************************************************************************/

//범위 정규화를 위함
#define ENC_NORM_MIN 0
#define ENC_NORM_MAX 1023

unsigned int enc_norm1,enc_norm2, enc_diff1, enc_diff2; // 정규화 된 값

//전역변수 선언부
volatile unsigned int count_L, count_R, encoder_T, flag, M_L,M_R, RPM_L, RPM_R;
volatile unsigned int encT_L, encT_R;
/*
volatile float motor_signal0;
volatile float error_funct;
volatile float old_error_funct;
volatile float old_error_funct2;
volatile float desired;
volatile float measured;
volatile float old_motor_signal;
volatile double KP;
volatile double KD;
volatile double KI;
volatile float delta_t;
volatile int limit;
volatile int pwmduty;*/

//함수 선언부
void init_ENCODER();
//void PID(void);
