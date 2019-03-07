
#define MotorPort PORTG
#define MotorPort_DDR DDRG
#define Motor_Mask 0xf0
#define MotorPort_PWM PORTE
#define MotorPort_PWMDDR DDRE
#define MotorPWM_Mask 0x18

#define MotorFoward_Mask 0x30
#define MotorBackward_Mask 0xC0
#define MotorLeft 0xb0
#define MotorRight 0x30
#define MotorL_1 3 
#define MotorL_2 4
#define MotorR_1 0
#define MotorR_2 1
#define MotorL_P 3
#define MotorR_P 4
#define Line_Sensitivity 5


void init_MOTOR();
void MotorGoFoward();
void MotorGoBackward();
void TurnRight();
void TurnLeft();
void MotorStop();
void MotorAngle(int angle);
void MotorCommand(char command);

void Motor_Sonar(unsigned int x, unsigned int y);
void Left1(int a);
void Left2(void);
void Left3(void);
void Left4(void);
void Right1(int b);
void Right2(void);
void Right3(void);
void Right4(void);
