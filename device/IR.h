#ifndef __IR_H__
#define __IR_H__

#include "config.h"

/*
�˳������HC460ʹ�����������䣺1���ز�������ʱ��
							   2���������մ���ʱ��
							   3�����ղ���ʱ��
							   4�����ͽ�����
				               5�����ս�����
������ʱ���ͷ��ͽ��ս�����Ӧ�ĺ�����
							   Carwave_Timer_Init��Carwave_Timer_Open��Carwave_Timer_Close
							   Handle_Timer_Init��Handle_Timer_Open��Handle_Timer_Close��Handle_Timer_Zero
							   Cap_Timer_Init
							   Tx_Pin_Low��Tx_Pin_Tog��IR_TX_Init
							   ���ս�����Cap_Timer_Init������
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

    uint8_t  Rcv_MODE: 1; //�������
    uint8_t  Rcv_Bcmp_Flag: 1; //����λ���
    uint8_t  Rcv_Cmp_Flag: 1;	//������ɱ��
    uint8_t  Rcv_Str_Flag: 1; //������ʼλ���
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
@FUNCTION������̾෢��ģ��ײ㶨ʱ���Ƚ���(1ms)
@INPUT��
@OUTPUT����
@RETURN: TRUE-����ִ�еײ㶨ʱ����,  FALSE-�޵ײ㶨ʱ����, ��ִ���ϲ�Ӧ�ö�ʱ����
@AUTHOR��xfw
@SPECIAL: ʵ�ֽ��ճ�ʱ�ж�
****************************************************************/
extern uint32_t Ir_LowLevelTimedProcess(void);


#endif

