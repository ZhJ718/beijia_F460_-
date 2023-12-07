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


//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
uint32_t Is_Leap_Year(uint16_t year)
{
    if(year % 4 == 0) //必须能被4整除
    {
        if(year % 100 == 0)
        {
            if(year % 400 == 0)return 1; //如果以00结尾,还要能被400整除
            else return 0;
        }
        else return 1;
    }
    else return 0;
}

//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.
//月份数据表
uint8_t const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}; //月修正数据表
//平年的月份日期表
const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//输入参数：公历年月日
//返回值：星期号
uint32_t RTC_Get_Week(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t temp2;
    uint8_t yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;

    // 如果为21世纪,年份数加100
    if (yearH > 19)yearL += 100;

    // 所过闰年数只算1900年之后的
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

    for(t = 1970; t < pDate->year; t++)	//把所有年份的秒钟相加
    {
        if(Is_Leap_Year(t))
            seccount += 31622400; //闰年的秒钟数
        else
            seccount += 31536000;			 //平年的秒钟数
    }

    //pDate->month -= 1;
    for(t = 0; t < pDate->month - 1; t++)	 //把前面月份的秒钟数相加
    {
        seccount += (uint32_t)mon_table[t] * 86400; //月份秒钟数相加

        if(Is_Leap_Year(pDate->year) && t == 1)
            seccount += 86400; //闰年2月份增加一天的秒钟数
    }

    seccount += (uint32_t)(pDate->date - 1) * 86400; //把前面日期的秒钟数相加
    seccount += (uint32_t)pDate->hour * 3600; //小时秒钟数
    seccount += (uint32_t)pDate->min * 60;	 //分钟秒钟数
    seccount += pDate->sec; //最后的秒钟加上去

    return seccount;
}


uint32_t RTC_GetTimeFromSecondsStartYear1970(uint32_t Sec, TIMER_MAG *pDate)
{
    static uint16_t daycnt = 0;

    uint32_t temp = 0;
    uint16_t temp1 = 0;

    if(pDate == NULL)
        return 1;

    temp = Sec / 86400; //得到天数(秒钟数对应的)

    if(daycnt != temp) //超过一天了
    {
        daycnt = temp;
        temp1 = 1970;	//从1970年开始

        while(temp >= 365)
        {
            if(Is_Leap_Year(temp1))//是闰年
            {
                if(temp >= 366)temp -= 366; //闰年的秒钟数
                else
                {
                    temp1++;
                    break;
                }
            }
            else temp -= 365;	 //平年

            temp1++;
        }

        pDate->year = temp1; //得到年份
        temp1 = 0;

        while(temp >= 28) //超过了一个月
        {
            if(Is_Leap_Year(pDate->year) && temp1 == 1) //当年是不是闰年/2月份
            {
                if(temp >= 29)temp -= 29; //闰年的秒钟数
                else break;
            }
            else
            {
                if(temp >= mon_table[temp1])temp -= mon_table[temp1]; //平年
                else break;
            }

            temp1++;
        }

        pDate->month = temp1 + 1;	//得到月份
        pDate->date = temp + 1;  	//得到日期
    }

    temp = Sec % 86400;     		//得到秒钟数
    pDate->hour = temp / 3600;     	//小时
    pDate->min = (temp % 3600) / 60; 	//分钟
    pDate->sec = (temp % 3600) % 60; 	//秒钟
    //pDate->week=RTC_Get_Week(pDate->year,pDate->month,pDate->date);//获取星期

    return 0;
}



/****************************************************************
@FUNCTION：软件实时时钟初始化
@INPUT：无
@OUTPUT：
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:由于STM32F103的RTC容易出问题，而且还不是真正意义上的RTC
		清零 g_sRtcMag
****************************************************************/
extern void SoftRTC_Init(void)
{
    memset((void *)&g_sRtcMag, 0, sizeof(SOFT_RTC_MAG));

}

/****************************************************************
@FUNCTION：软件定时器1s定时任务
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern void SoftRTC_TimedTask_1S(void)
{
    g_sRtcMag.m_dwSysCnt += 1;
}

/****************************************************************
@FUNCTION：获取当前时间
@INPUT：TIMER_MAG
@OUTPUT：TIMER_MAG内容
@RETURN: 0,成功;其他:错误代码.
@AUTHOR：xfw
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
@FUNCTION：设置当前时间
@INPUT：TIMER_MAG
@OUTPUT：无
@RETURN: 0,成功;其他:错误代码.
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern uint32_t SoftRTC_Set(TIMER_MAG *pDate)
{
    g_sRtcMag.m_dwSysCnt = GetSysRunSecond();
    g_sRtcMag.m_dwSecondOffset = RTC_GetSecondsStartYear1970(pDate);

    return 0;
}

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
