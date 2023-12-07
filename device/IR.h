#ifndef __IR_H__
#define __IR_H__

#include "config.h"

/*
此程序基于HC460使用需自行适配：1、载波发生定时器
							   2、发生接收处理定时器
							   3、接收捕获定时器
							   4、发送脚配置
				               5、接收脚配置
及各定时器和发送接收脚所对应的函数：
							   Carwave_Timer_Init、Carwave_Timer_Open、Carwave_Timer_Close
							   Handle_Timer_Init、Handle_Timer_Open、Handle_Timer_Close、Handle_Timer_Zero
							   Cap_Timer_Init
							   Tx_Pin_Low、Tx_Pin_Tog、IR_TX_Init
							   接收脚是在Cap_Timer_Init中配置
*/


#define Ir_Max_Rcv	256


typedef struct
{
    uint32_t us_count;
    uint32_t send_time_count;
    uint8_t send_data;
    uint8_t tx_count;
    uint8_t timer_flag;
    uint8_t TX_Mode;
    uint8_t tx_data;
} Ir_Tx_Msg;
extern volatile Ir_Tx_Msg ir_tx_msg;


typedef struct
{

    uint8_t  Rcv_MODE: 1; //允许接收
    uint8_t  Rcv_Bcmp_Flag: 1; //接收位标记
    uint8_t  Rcv_Cmp_Flag: 1;	//接收完成标记
    uint8_t  Rcv_Str_Flag: 1; //接收起始位标记
    uint8_t  Rcv_TOut_Flag: 1;
    uint8_t  RcvFlag: 3;


    uint32_t NowCnt;
    uint32_t LastCnt;
    uint32_t Off_Cnt;
    uint16_t Rise_Cnt;
    uint16_t Lastrise_Cnt;
    uint8_t  Rcv_data;
    uint8_t  Ir_Rcv_Buf[Ir_Max_Rcv];
    uint16_t Ir_Rcv_Cnt;
    uint16_t Last_Rcv_Cnt;

    uint8_t  T_Cnt;
} Ir_Rcv_Msg;
extern Ir_Rcv_Msg ir_rcv_msg;


void User_Ir_Init(void);

void User_Tx_Init(void);

uint8_t User_Ir_Send(uint8_t *send_buf, uint16_t len);

uint8_t User_Ir_Rcv(uint8_t *rx_buf, uint16_t *rx_len);

uint8_t IsIR38SendBusy(void);


/****************************************************************
@FUNCTION：红外短距发射模块底层定时调度进程(1ms)
@INPUT：
@OUTPUT：无
@RETURN: TRUE-正在执行底层定时调度,  FALSE-无底层定时调度, 可执行上层应用定时调度
@AUTHOR：xfw
@SPECIAL: 实现接收超时判断
****************************************************************/
extern uint32_t Ir_LowLevelTimedProcess(void);


#endif

