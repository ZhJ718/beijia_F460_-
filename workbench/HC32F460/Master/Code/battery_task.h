#ifndef BATTERY_TASK_H
#define BATTERY_TASK_H

#include "config.h"

#define BAT_LOW 		3

#define BAT_VERY_LOW 	1		
#define BAT_OFFSET 	4

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

/****************************************************************
@FUNCTION£∫µÁ≥ÿπ‹¿Ì»ŒŒÒªÒ»°µ±«∞µÁ—π÷µ£®*1000£©
@INPUT£∫ Œﬁ
@OUTPUT: µ±«∞µÁ—π÷µ
@RETURN: Œﬁ
@AUTHOR£∫xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
uint16_t BatTask_GetExBatVol(void);


/****************************************************************
@FUNCTION£∫µÁ≥ÿπ‹¿Ì»ŒŒÒªÒ»°µ±«∞µÁ≥ÿ £”‡µÁ¡ø
@INPUT£∫ Œﬁ
@OUTPUT:  £”‡µÁ¡ø0-100
@RETURN: Œﬁ
@AUTHOR£∫xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
uint16_t BatTask_GetExBatRes(void);

uint8_t GetBatRemainingCapacity_18650(uint16_t BatVol, uint8_t X);

#endif
