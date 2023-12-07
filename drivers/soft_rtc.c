/**
  ******************************************************************************
  * @file    soft_rtc.c
  * @author  xfw
  * @brief   This file contains definitions for soft RTC
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Beijing HuaRu Tech Co.</center></h2>
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

/* Includes ------------------------------------------------------------------*/
#include "soft_rtc.h"




SOFT_RTC_MAG g_sRtcMag;


//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
uint32_t Is_Leap_Year(uint16_t year)
{
    if(year % 4 == 0) //�����ܱ�4����
    {
        if(year % 100 == 0)
        {
            if(year % 400 == 0)return 1; //�����00��β,��Ҫ�ܱ�400����
            else return 0;
        }
        else return 1;
    }
    else return 0;
}

//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�
uint8_t const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}; //���������ݱ�
//ƽ����·����ڱ�
const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//�������������������
//����ֵ�����ں�
uint32_t RTC_Get_Week(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t temp2;
    uint8_t yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;

    // ���Ϊ21����,�������100
    if (yearH > 19)yearL += 100;

    // ����������ֻ��1900��֮���
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];

    if (yearL % 4 == 0 && month < 3)temp2--;

    return(temp2 % 7);
}

uint32_t RTC_GetSecondsStartYear1970(TIMER_MAG *pDate)
{
    uint32_t seccount = 0;
    uint16_t t;

    if(pDate == NULL)
        return 0;

    if(pDate->year < 1970 || pDate->year > 2099)
        return 1;

    for(t = 1970; t < pDate->year; t++)	//��������ݵ��������
    {
        if(Is_Leap_Year(t))
            seccount += 31622400; //�����������
        else
            seccount += 31536000;			 //ƽ���������
    }

    //pDate->month -= 1;
    for(t = 0; t < pDate->month - 1; t++)	 //��ǰ���·ݵ����������
    {
        seccount += (uint32_t)mon_table[t] * 86400; //�·����������

        if(Is_Leap_Year(pDate->year) && t == 1)
            seccount += 86400; //����2�·�����һ���������
    }

    seccount += (uint32_t)(pDate->date - 1) * 86400; //��ǰ�����ڵ����������
    seccount += (uint32_t)pDate->hour * 3600; //Сʱ������
    seccount += (uint32_t)pDate->min * 60;	 //����������
    seccount += pDate->sec; //�������Ӽ���ȥ

    return seccount;
}


uint32_t RTC_GetTimeFromSecondsStartYear1970(uint32_t Sec, TIMER_MAG *pDate)
{
    static uint16_t daycnt = 0;

    uint32_t temp = 0;
    uint16_t temp1 = 0;

    if(pDate == NULL)
        return 1;

    temp = Sec / 86400; //�õ�����(��������Ӧ��)

    if(daycnt != temp) //����һ����
    {
        daycnt = temp;
        temp1 = 1970;	//��1970�꿪ʼ

        while(temp >= 365)
        {
            if(Is_Leap_Year(temp1))//������
            {
                if(temp >= 366)temp -= 366; //�����������
                else
                {
                    temp1++;
                    break;
                }
            }
            else temp -= 365;	 //ƽ��

            temp1++;
        }

        pDate->year = temp1; //�õ����
        temp1 = 0;

        while(temp >= 28) //������һ����
        {
            if(Is_Leap_Year(pDate->year) && temp1 == 1) //�����ǲ�������/2�·�
            {
                if(temp >= 29)temp -= 29; //�����������
                else break;
            }
            else
            {
                if(temp >= mon_table[temp1])temp -= mon_table[temp1]; //ƽ��
                else break;
            }

            temp1++;
        }

        pDate->month = temp1 + 1;	//�õ��·�
        pDate->date = temp + 1;  	//�õ�����
    }

    temp = Sec % 86400;     		//�õ�������
    pDate->hour = temp / 3600;     	//Сʱ
    pDate->min = (temp % 3600) / 60; 	//����
    pDate->sec = (temp % 3600) % 60; 	//����
    //pDate->week=RTC_Get_Week(pDate->year,pDate->month,pDate->date);//��ȡ����

    return 0;
}



/****************************************************************
@FUNCTION�����ʵʱʱ�ӳ�ʼ��
@INPUT����
@OUTPUT��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:����STM32F103��RTC���׳����⣬���һ��������������ϵ�RTC
		���� g_sRtcMag
****************************************************************/
extern void SoftRTC_Init(void)
{
    memset((void *)&g_sRtcMag, 0, sizeof(SOFT_RTC_MAG));

}

/****************************************************************
@FUNCTION�������ʱ��1s��ʱ����
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void SoftRTC_TimedTask_1S(void)
{
    g_sRtcMag.m_dwSysCnt += 1;
}

/****************************************************************
@FUNCTION����ȡ��ǰʱ��
@INPUT��TIMER_MAG
@OUTPUT��TIMER_MAG����
@RETURN: 0,�ɹ�;����:�������.
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern uint32_t SoftRTC_Get(TIMER_MAG *pDate)
{
    uint32_t t = GetSysRunSecond();

    t -= g_sRtcMag.m_dwSysCnt;
    t += g_sRtcMag.m_dwSecondOffset;
    return RTC_GetTimeFromSecondsStartYear1970(t, pDate);
}


/****************************************************************
@FUNCTION�����õ�ǰʱ��
@INPUT��TIMER_MAG
@OUTPUT����
@RETURN: 0,�ɹ�;����:�������.
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern uint32_t SoftRTC_Set(TIMER_MAG *pDate)
{
    g_sRtcMag.m_dwSysCnt = GetSysRunSecond();
    g_sRtcMag.m_dwSecondOffset = RTC_GetSecondsStartYear1970(pDate);

    return 0;
}

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
