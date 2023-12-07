/**
  ******************************************************************************
  * @file    ljb_laser_send.h
  * @author  xfw
  * @brief   This file contains definitions for laser sending
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2021 Beijing HuaRu Tech Co.</center></h2>
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
#ifndef __LJB_LASER_SEND_H
#define __LJB_LASER_SEND_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "ljb_laser.h"

#define SELF_CHECK_BYTE 0x55

/*
	LJB�±�׼:�ߵ�ƽ����ʱ��㶨2-3us��ֻ�ǵ͵�ƽ����ʱ�䲻һ��
	�ڲ����� 22.1184MHz,1us�������Ƚ�ֵΪ22.1184
	ÿ�������Ϊ6�����֣��ߵ�ƽ+�͵�ƽ�����Ҹߵ�ƽ����ʱ��һ��������ÿ������ֻ��Ҫ��¼�µ͵�ƽ����ʱ��
*/

#define LJB_LASER_TIMER_CLOCK_MHZ	42 //��ʱ��ʱ��(��λ��MHZ)

#define LJB_LASER_SEND_HIGH_DURATION	3	//�ߵ�ƽĬ������
#define LJB_LASER_SEND_BASED_FREQ	48000	//�������Ƶ��
#define LJB_LASER_SEND_LOW_CNT_STEP	6	//����ʱ����ÿ��������ĵ͵�ƽ����
#define LJB_LASER_SEND_STEP_CNT		12	//����ʱ���а����Ľ׶���

#if 0
#define LJB_LASER_SEND_CMP_CALIBRATION	40	//����ֵ(���ڳ���ִ�е����ʱ��)
#define HIGH_CMP	(66-LJB_LASER_SEND_CMP_CALIBRATION)
#define LOW_1T_CMP	(394-LJB_LASER_SEND_CMP_CALIBRATION)
#define LOW_2T_CMP	(854-LJB_LASER_SEND_CMP_CALIBRATION)
#define LOW_3T_CMP	(1314-LJB_LASER_SEND_CMP_CALIBRATION)
#define LOW_4T_CMP	(1774-LJB_LASER_SEND_CMP_CALIBRATION)
#define LOW_5T_CMP	(2234-LJB_LASER_SEND_CMP_CALIBRATION)
#endif

//����ʱ��42MHz
#define LJB_LASER_SEND_CMP_CALIBRATION	108	//����ֵ(���ڳ���ִ�е����ʱ��)
#define HIGH_CMP	(126 - LJB_LASER_SEND_CMP_CALIBRATION)	//�ߵ�ƽȡ3us  (3*48 - LJB_LASER_SEND_CMP_CALIBRATION)
#define LOW_1T_CMP	(748-LJB_LASER_SEND_CMP_CALIBRATION+15)	//(1*20.8-3) * LJB_LASER_TIMER_CLOCK_MHZ - LJB_LASER_SEND_CMP_CALIBRATION
#define LOW_2T_CMP	(1621-LJB_LASER_SEND_CMP_CALIBRATION+30)	//(2*20.8-3) * LJB_LASER_TIMER_CLOCK_MHZ - LJB_LASER_SEND_CMP_CALIBRATION
#define LOW_3T_CMP	(2495-LJB_LASER_SEND_CMP_CALIBRATION+45)	//(3*20.8-3) * LJB_LASER_TIMER_CLOCK_MHZ - LJB_LASER_SEND_CMP_CALIBRATION
#define LOW_4T_CMP	(3368-LJB_LASER_SEND_CMP_CALIBRATION+60)	//(4*20.8-3) * LJB_LASER_TIMER_CLOCK_MHZ - LJB_LASER_SEND_CMP_CALIBRATION
#define LOW_5T_CMP	(4242-LJB_LASER_SEND_CMP_CALIBRATION+75)	//(5*20.8-3) * LJB_LASER_TIMER_CLOCK_MHZ - LJB_LASER_SEND_CMP_CALIBRATION



//Ӳ������
typedef struct
{
    //Ӳ����ʼ������������IO�ڳ�ʼ�������ڵ�
    void (*hw_init)(void);
    //������øߣ�����Ϊ��
    void (*send_gpio_high)(void);
    //������õͣ�����Ϊ��
    void (*send_gpio_low)(void);
    //�򿪶�ʱ������Ϊ��
    void (*open_timing)(uint16_t);
    //�رն�ʱ������Ϊ��
    void (*close_timing)(void);
} LJB_LASER_SEND_HW_OPERATIONS;


typedef struct
{
    uint8_t m_IsHigh;	//������״̬ 0-�͵�ƽ 1-�ߵ�ƽ
    uint8_t m_byRes;		//����
    uint16_t m_wDelayCmpCnt;//��ʱ������ֵ
} LJB_LASER_SEND_STEP_MAG;

typedef struct
{
    LJB_LASER_SEND_STEP_MAG m_saStep[LJB_LASER_SEND_STEP_CNT];
} LJB_LASER_SEND_ENCODE_MAG;


typedef struct
{
    LJB_LASER_SEND_ENCODE_MAG *m_psEncode;
    uint8_t m_byNowStep;	//��ǰ���͵��Ĳ�
    uint8_t m_byMaxStep;	//�����
} LJB_LASER_SEND_SINGLE_ENCODE;

#define LJB_LASER_SEND_REPEAT_CNT 3

typedef struct
{
    LJB_LASER_SEND_HW_OPERATIONS m_sHwOps;


    uint8_t m_IsRun: 1;	//TRUE-���ڷ�����
    uint8_t m_IsEnableIRT1: 1; ////TRUE-ʹ��IRT1
    uint8_t m_IsEnableIRT2: 1; ////TRUE-ʹ��IRT2
    uint8_t m_IsEnableIRT3: 1; ////TRUE-ʹ��IRT3
    uint8_t m_IsEnableIRT4: 1; ////TRUE-ʹ��IRT4
    uint8_t m_IsRes: 2;

    LJB_LASER_SEND_SINGLE_ENCODE m_sSingle;//����ֲ����ݸ�ʽ

    uint8_t m_bySendLen;	//��Ҫ���ͱ�������1�ֽ�->2���룩
    uint8_t m_bySentLen;	//�ѷ��ͱ�����
    uint8_t m_bySendBuf[LJB_LASER_SEND_LEN_MAX];
    uint8_t m_byRepeatCnt;	//�ظ�����
    uint16_t m_wLaserIntensity;	//���ⷢ��ǿ��0-500  ����λ0.01
} LJB_LASER_SEND_MAG;



static const LJB_LASER_SEND_ENCODE_MAG gs_LaserSendEncode[ENCODE_MAX] =
{
    //ENCODE_HIGH_0000 	2 3 3 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
//ENCODE_HIGH_0001	4 1 2 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_0010	4 2 2 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_0011	3 2 3 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_0100	2 2 2 3 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_0101	3 1 3 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_0110	2 1 2 1 4 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_0111	2 2 1 2 3 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1000	2 1 3 3 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1001 	3 1 2 3 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1010	3 3 2 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1011	2 2 1 3 2 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1100 	2 1 3 2 2 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1101	3 2 1 3 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1110	2 2 2 1 3 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1111	3 2 2 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},

    //ENCODE_LOW_0000	2 3 2 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0001	4 1 1 1 3 1
    {1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0010	2 2 1 4 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0011	2 1 4 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0100	2 1 2 2 3 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0101	4 1 3 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0110	2 4 2 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0111	5 2 1 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_5T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1000	2 4 1 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1001	3 3 1 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1010	3 4 1 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1011	2 1 1 5 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_5T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1100	4 3 1 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1101	2 5 1 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_5T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1110	2 2 4 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1111	2 3 1 3 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},

    //ENCODE_HEAD	2 1 5 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_5T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_TAIL	2 2 3 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
};

extern LJB_LASER_SEND_MAG g_sLaserSendMag;

//���ⷢ����̣���ʱ���жϵ���
void LaserSend_Process(LJB_LASER_SEND_MAG *pMag);

void LaserSend_Start(LJB_LASER_SEND_MAG *pMag, uint8_t *pBuf, uint16_t Len);

void LaserSend_Init(LJB_LASER_SEND_MAG *pMag, LJB_LASER_SEND_HW_OPERATIONS HwOps);

uint8_t IsLaserSendIdle(LJB_LASER_SEND_MAG Mag);

//�ײ���������
//void LaserSend_Gpio_Toggle(void);

void LaserSend_LowlevelInit(void);

void LaserSend_Gpio_On(void);

void LaserSend_Gpio_Off(void);

void LaserSendTimer_Open(uint16_t Val);

void LaserSendTimer_Close(void);



#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
