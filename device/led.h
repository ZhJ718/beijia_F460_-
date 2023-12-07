/**
  ******************************************************************************
  * @file    led.h
  * @author  xfw
  * @brief   This file contains definitions for STM32F103 Leds
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 Beijing HuaRu Tech Co.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of HuaRu nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"


/**
 * @brief LED Types Definition
 */

//״ָ̬ʾ�ƣ�Ĭ����
typedef enum
{
    LED_COLOR_DARK = 0,
    LED_COLOR_BRIGHT,
} LED_COLOR;

//��ģʽ��Ĭ�ϵ���
typedef enum
{
    LED_MODE_BLINK_7 = 0,// 7����
    LED_MODE_BLINK_6,	// 6����
    LED_MODE_BLINK_5,	// 5����
    LED_MODE_BLINK_4,	// 4����
    LED_MODE_BLINK_3,	// 3����
    LED_MODE_BLINK_2,	// 2����
    LED_MODE_BLINK_1,	//����
    LED_MODE_BLINK_ALWAYS_ON,	//����
    LED_MODE_BLINK_ALWAYS_OFF,	//����
    LED_MODE_BLINK_MAX,
} LED_MODE;

#define LED_CYCLE_SECTION_CNT_MAX	(2*LED_MODE_BLINK_MAX)

typedef struct
{
    uint8_t						m_by100MsCnt;	//ά��ʱ��(*100MS)
    LED_COLOR 	m_enumLedColor;	//����ɫ
} LED_CYCLE_SECTION;

typedef struct
{
    uint8_t 				m_byMaxStep;			//���׶�
    LED_CYCLE_SECTION 		m_pPeriod[LED_CYCLE_SECTION_CNT_MAX];
} LED_CYCLE;

typedef struct
{
    uint8_t 				m_byNowStep;			//��ǰ�����ĸ��׶�
    uint8_t 				m_byTimePast100Ms;		//��ǰ���ڽ׶��Ѿ�����ʱ��(*100MS)
    LED_CYCLE 				m_sCycle;
} LED_CYCLE_MAG;


//״̬�ƿ���
//��������ģʽ
//����ÿ�׶�( 7s)״̬����
static const LED_CYCLE gc_sBlink_1 =
{
    2,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
    }
};

// 2����ÿ�׶�(4s)״̬����
static const LED_CYCLE gc_sBlink_2 =
{
    4,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {25, LED_COLOR_DARK},
    }
};

// 3 ����ÿ�׶�(5s)״̬����
static const LED_CYCLE gc_sBlink_3 =
{
    6,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {25, LED_COLOR_DARK},
    }
};


// 4����ÿ�׶�(6s)״̬����
static const LED_CYCLE gc_sBlink_4 =
{
    8,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {25, LED_COLOR_DARK},
    }
};

// 5����ÿ�׶�(7s)״̬����
static const LED_CYCLE gc_sBlink_5 =
{
    10,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {25, LED_COLOR_DARK},
    }
};

// 6����ÿ�׶�(8s)״̬����
static const LED_CYCLE gc_sBlink_6 =
{
    12,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {25, LED_COLOR_DARK},
    }
};

// 7����ÿ�׶�(9s)״̬����
static const LED_CYCLE gc_sBlink_7 =
{
    14,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {25, LED_COLOR_DARK},
    }
};



//����ģʽÿ�׶�(5s)״̬����
static const LED_CYCLE gc_sBlink_AlwaysOn =
{
    2,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_BRIGHT},

    }
};

//����ģʽÿ�׶�(5s)״̬����
static const LED_CYCLE gc_sBlink_AlwaysOff =
{
    2,
    {
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_DARK},

    }
};


//LEDӲ������
typedef struct
{
    //LEDӲ���ӿڳ�ʼ������
    void (*led_hw_init)(void);
    //LED������
    void (*led_on)(void);
    //LED�����
    void (*led_off)(void);
} LED_HW_OPERATIONS;


typedef struct
{
    //ʵ��LED����߼���ֻ��
    uint8_t		m_byIsStartLed: 1;
    uint8_t		m_byIsLedOn: 1;
    uint8_t		m_byRes: 6;
    uint8_t		m_byBrightness;//���� 0 - 100

    uint8_t		m_byBrightCnt;
    uint16_t	m_w1MsCnt;	//1ms����

    LED_CYCLE_MAG	 m_sCycleMag;
    LED_MODE		m_dwMode;
    //ʵ��LED���Ӳ���������ɶ�д
    LED_HW_OPERATIONS m_sHwOps;


} LED_MAG;


/****************************************************************
@FUNCTION���ƿع�����ͣ
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Led_Suspend(LED_MAG *pMag);

/****************************************************************
@FUNCTION���ƿع����Ƿ���ͣ
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: TRUE-��ͣ FALSE-δ��ͣ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
u32 Led_IsSuspend(LED_MAG *pMag);

/****************************************************************
@FUNCTION���ƿع��ָܻ�
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Led_Recovery(LED_MAG *pMag);

/****************************************************************
@FUNCTION�����õ�N����
@INPUT��N����,LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����ʱ����ͣ�ƿع��ܣ�������ɻ�ָ�
****************************************************************/
extern void Led_SetRunMode(LED_MODE BlinkMode, LED_MAG *pMag);

/****************************************************************
@FUNCTION�����õ�����
@INPUT��������,LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���Ȱٷֱ�0-100
****************************************************************/
extern void Led_SetBrightness(uint8_t SetValue, LED_MAG *pMag);


/****************************************************************
@FUNCTION��ʵ�ֵ�ɫ�ƿ����߼�
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ÿ�̶�ʱ��(Ŀǰ100Ms)����һ��

****************************************************************/
extern void Led_TimedProcess(LED_MAG *pMag) ;

/****************************************************************
@FUNCTION���ƿع��ܳ�ʼ��
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���ݵ�Ӳ��������غ�������ʼ��Ӳ���͹رյƣ���ͣ�ƿع���
****************************************************************/
extern void Led_Init(LED_MAG *pMag);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
