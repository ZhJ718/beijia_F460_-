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


#define BAT_ADC_OFFSET 		10		//代表电压判断时偏离值在0.02V左右
#define BAT_LOW_HALT_CNT 	39		//电池电压低持续时间


typedef struct
{

    uint32_t 	m_bIsInit: 1;   //任务初始化
    uint32_t 	m_bRes: 31;

    uint32_t    m_dwScheduleTime;//最小调度时间，单位ms
    uint32_t    m_dwLastScheduleTime;//上次调度时间，单位ms

    uint16_t	m_wFactor;  //分压比例系数*1000
    uint16_t m_wBatVol;	//电池电压*1000
    uint8_t m_byBatRes; //电池剩余量0-100

} BAT_MANAGE;

extern BAT_MANAGE g_sBatMag;

/****************************************************************
@FUNCTION：电池管理任务重置分压系数（*1000）
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_SetFactor(uint16_t New);


/****************************************************************
@FUNCTION：电池管理任务初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_Init(void);

/****************************************************************
@FUNCTION：电池管理任务反初始�
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_DeInit(void);


/****************************************************************
@FUNCTION：电池管理任务处理
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_Process(void);


#endif
