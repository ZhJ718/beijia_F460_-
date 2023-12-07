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


#define BAT_ADC_OFFSET 		10		//¥˙±ÌµÁ—π≈–∂œ ±∆´¿Î÷µ‘⁄0.02V◊Û”“
#define BAT_LOW_HALT_CNT 	39		//µÁ≥ÿµÁ—πµÕ≥÷–¯ ±º‰


typedef struct
{

    uint32_t 	m_bIsInit: 1;   //»ŒŒÒ≥ı ºªØ
    uint32_t 	m_bRes: 31;

    uint32_t    m_dwScheduleTime;//◊Ó–°µ˜∂» ±º‰£¨µ•Œªms
    uint32_t    m_dwLastScheduleTime;//…œ¥Œµ˜∂» ±º‰£¨µ•Œªms

    uint16_t	m_wFactor;  //∑÷—π±»¿˝œµ ˝*1000
    uint16_t m_wBatVol;	//µÁ≥ÿµÁ—π*1000
    uint8_t m_byBatRes; //µÁ≥ÿ £”‡¡ø0-100

} BAT_MANAGE;

extern BAT_MANAGE g_sBatMag;

/****************************************************************
@FUNCTION£∫µÁ≥ÿπ‹¿Ì»ŒŒÒ÷ÿ÷√∑÷—πœµ ˝£®*1000£©
@INPUT£∫ Œﬁ
@OUTPUT: Œﬁ
@RETURN: Œﬁ
@AUTHOR£∫xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_SetFactor(uint16_t New);


/****************************************************************
@FUNCTION£∫µÁ≥ÿπ‹¿Ì»ŒŒÒ≥ı ºªØ
@INPUT£∫ Œﬁ
@OUTPUT: Œﬁ
@RETURN: Œﬁ
@AUTHOR£∫xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_Init(void);

/****************************************************************
@FUNCTION£∫µÁ≥ÿπ‹¿Ì»ŒŒÒ∑¥≥ı ºª
@INPUT£∫ Œﬁ
@OUTPUT: Œﬁ
@RETURN: Œﬁ
@AUTHOR£∫xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_DeInit(void);


/****************************************************************
@FUNCTION£∫µÁ≥ÿπ‹¿Ì»ŒŒÒ¥¶¿Ì
@INPUT£∫ Œﬁ
@OUTPUT: Œﬁ
@RETURN: Œﬁ
@AUTHOR£∫xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_Process(void);


#endif
