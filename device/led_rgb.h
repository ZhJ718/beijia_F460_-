/**
  ******************************************************************************
  * @file    led_rgb.h
  * @author  xfw
  * @brief   This file contains definitions for RGB Led
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
#ifndef __LED_RGB_H
#define __LED_RGB_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "led.h"

/**
 * @brief LED Types Definition
 */

//��ɫ��
typedef enum
{
    LED_RGB_COLOR_GREEN = 0,//��  - ������С
    LED_RGB_COLOR_RED,// ��
    LED_RGB_COLOR_BLUE,// ��
    LED_RGB_COLOR_YELLOW,// ��
    LED_RGB_COLOR_PURPLE,// ��
    LED_RGB_COLOR_LIGHT_BLUE,// ǳ��
    LED_RGB_COLOR_WRITE,// ��
    LED_RGB_COLOR_MAX,//
} LED_RGB_COLOR;

//��ģʽ��Ĭ�ϵ���
typedef enum
{
    LED_RGB_MODE_BLINK_ALWAYS_OFF,	//����
    LED_RGB_MODE_BLINK_TEMPORARY,	//��ʱ��
    LED_RGB_MODE_BLINK_1,	//����
    LED_RGB_MODE_BLINK_2,	//˫��
    LED_RGB_MODE_BLINK_ALWAYS_ON,	//����
    LED_RGB_MODE_BLINK_MAX,
} LED_RGB_MODE;


//LEDӲ������
typedef struct
{
    //LEDӲ���ӿڳ�ʼ������
    void (*led_hw_init)(void);
    //��LED������
    void (*led_r_on)(void);
    //��LED������
    void (*led_g_on)(void);
    //��LED������
    void (*led_b_on)(void);
    //LED�����
    void (*led_off)(void);
} LED_RGB_HW_OPERATIONS;


typedef struct
{
    //ʵ��LED����߼���ֻ��
    uint8_t		m_byIsStartLed: 1;
    uint8_t		m_byIsLedOn: 1;
    uint8_t		m_byRes: 6;
    uint8_t		m_byBrightness;//���� 0 - 100

    uint8_t		m_byBrightCnt;
    uint8_t		m_wRunCnt;	//���д�������������LED_RGB_MODE_BLINK_TEMPORARYģʽ
    uint16_t	m_w1MsCnt;	//1ms����

    LED_CYCLE_MAG	 m_sCycleMag;
    LED_RGB_MODE		m_dwMode;
    LED_RGB_COLOR		m_eColor;
    //ʵ��LED���Ӳ���������ɶ�д
    LED_RGB_HW_OPERATIONS m_sHwOps;


} LED_RGB_MAG;


/****************************************************************
@FUNCTION���ƿع�����ͣ
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Led_RGB_Suspend(LED_RGB_MAG *pMag);

/****************************************************************
@FUNCTION���ƿع����Ƿ���ͣ
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: TRUE-��ͣ FALSE-δ��ͣ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
u32 Led_RGB_IsSuspend(LED_RGB_MAG *pMag);

/****************************************************************
@FUNCTION���ƿع��ָܻ�
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Led_RGB_Recovery(LED_RGB_MAG *pMag);

/****************************************************************
@FUNCTION�����õ���˸ģʽ
@INPUT��LED����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����ʱ����ͣ�ƿع��ܣ�������ɻ�ָ�
****************************************************************/
extern void Led_RGB_SetRunMode(LED_RGB_MODE BlinkMode, uint16_t BlinkCnt, LED_RGB_MAG *pMag);


/****************************************************************
@FUNCTION�����õ�����
@INPUT��������,LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���Ȱٷֱ�0-100
****************************************************************/
extern void Led_RGB_SetBrightness(uint8_t SetValue, LED_RGB_MAG *pMag);


/****************************************************************
@FUNCTION�����õ�ɫ��
@INPUT��ɫ��,LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: Ĭ����ɫ
****************************************************************/
extern void Led_RGB_SetColor(LED_RGB_COLOR eColor, LED_RGB_MAG *pMag);



/****************************************************************
@FUNCTION��ʵ�ֵ�ɫ�ƿ����߼�
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ÿ�̶�ʱ��(Ŀǰ100Ms)����һ��

****************************************************************/
extern void Led_RGB_TimedProcess(LED_RGB_MAG *pMag) ;

/****************************************************************
@FUNCTION���ƿع��ܳ�ʼ��
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���ݵ�Ӳ��������غ�������ʼ��Ӳ���͹رյƣ���ͣ�ƿع���
****************************************************************/
extern void Led_RGB_Init(LED_RGB_MAG *pMag);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
