#ifndef IR_SEND_H
#define IR_SEND_H


#include "Config.h"

#define TICK_PRESCALE   (38000 * 3)

#define SEND_CNT_ONE_CLICK        2;  //一次按键的发送次数

//激光发射初始化-IO口产生脉冲，使能Systick中断
void IR_Init(void);

#define Laser_A_On() do{LASERA = 1;}while(0)
#define Laser_A_Off() do{LASERA = 0;}while(0)

#define Laser_B_On() do{;}while(0)
#define Laser_B_Off() do{;}while(0)

//定时检测激光是否需要发送数据
void IR_TimedProcess(void);

//开启红外发送	SEND_CNT_ONE_CLICK 次
//id-识别码  0-射击 2-调理 3-弹量指令
//RedOrBlue 1-红军 0-蓝军
void IR_StartSend(uint8_t id, uint8_t rid, uint16_t pid, uint8_t AmmoUniID, uint8_t Key, uint8_t RedOrBlue);

uint8_t IR_is_idle(void);

extern u8 g_byIRSwitch;
extern u8 g_byBase;

#endif
