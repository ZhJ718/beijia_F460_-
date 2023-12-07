#ifndef BATTERY_TASK_H
#define BATTERY_TASK_H

#include "config.h"

typedef enum
{
    ADC_CHANNEL_INNER_VREF = 0,
    ADC_CHANNEL_2_5V = 1,
    ADC_CHANNEL_EXBAT = 2,
} ADC_CHAN;

#define BAT_LOW 		3.5	// 20200703 

//#define BAT_LOW 		2.9	// 20190813


#define BAT_VERY_LOW 	3.4 //6.0	// 20190724
#define BAT_OFFSET 	0.04	// 20190724


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


#endif
