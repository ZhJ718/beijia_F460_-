#ifndef IR_SEND_H
#define IR_SEND_H


#include "Config.h"

#define TICK_PRESCALE   (38000 * 3)

#define SEND_CNT_ONE_CLICK        2;  //һ�ΰ����ķ��ʹ���

//���ⷢ���ʼ��-IO�ڲ������壬ʹ��Systick�ж�
void IR_Init(void);

#define Laser_A_On() do{LASERA = 1;}while(0)
#define Laser_A_Off() do{LASERA = 0;}while(0)

#define Laser_B_On() do{;}while(0)
#define Laser_B_Off() do{;}while(0)

//��ʱ��⼤���Ƿ���Ҫ��������
void IR_TimedProcess(void);

//�������ⷢ��	SEND_CNT_ONE_CLICK ��
//id-ʶ����  0-��� 2-���� 3-����ָ��
//RedOrBlue 1-��� 0-����
void IR_StartSend(uint8_t id, uint8_t rid, uint16_t pid, uint8_t AmmoUniID, uint8_t Key, uint8_t RedOrBlue);

uint8_t IR_is_idle(void);

extern u8 g_byIRSwitch;
extern u8 g_byBase;

#endif
