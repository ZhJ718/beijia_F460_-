#ifndef BATTERY_TASK_H
#define BATTERY_TASK_H

#include "config.h"

#define BAT_LOW 		3

#define BAT_VERY_LOW 	1		
#define BAT_OFFSET 	4

#define BAT_ADC_OFFSET 		10		//�����ѹ�ж�ʱƫ��ֵ��0.02V����
#define BAT_LOW_HALT_CNT 	39		//��ص�ѹ�ͳ���ʱ��

typedef struct
{

    uint32_t 	m_bIsInit: 1;   //�����ʼ��
    uint32_t 	m_bRes: 31;

    uint32_t    m_dwScheduleTime;//��С����ʱ�䣬��λms
    uint32_t    m_dwLastScheduleTime;//�ϴε���ʱ�䣬��λms

    uint16_t	m_wFactor;  //��ѹ����ϵ��*1000
    uint16_t m_wBatVol;	//��ص�ѹ*1000
    uint8_t m_byBatRes; //���ʣ����0-100

} BAT_MANAGE;

extern BAT_MANAGE g_sBatMag;

/****************************************************************
@FUNCTION����ع����������÷�ѹϵ����*1000��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_SetFactor(uint16_t New);


/****************************************************************
@FUNCTION����ع��������ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_Init(void);

/****************************************************************
@FUNCTION����ع������񷴳�ʼ�
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_DeInit(void);


/****************************************************************
@FUNCTION����ع���������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_Process(void);

/****************************************************************
@FUNCTION����ع��������ȡ��ǰ��ѹֵ��*1000��
@INPUT�� ��
@OUTPUT: ��ǰ��ѹֵ
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
uint16_t BatTask_GetExBatVol(void);


/****************************************************************
@FUNCTION����ع��������ȡ��ǰ���ʣ�����
@INPUT�� ��
@OUTPUT: ʣ�����0-100
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
uint16_t BatTask_GetExBatRes(void);

uint8_t GetBatRemainingCapacity_18650(uint16_t BatVol, uint8_t X);

#endif
