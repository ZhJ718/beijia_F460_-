#include "battery_task.h"
#include "adc.h"
#include "main_task.h"
//#include "led.h"

BAT_MANAGE g_sBatTaskMag;


//根据电池电压(*1000)获取18650电池剩余容量
uint8_t GetBatRemainingCapacity_18650(uint16_t BatVol, uint8_t X)
{
    uint8_t ret = 100;

    if(BatVol >= (4100 * X))
    {
        ret = 100;
    }
    else if(BatVol >= (3960 * X))
    {
        ret = 90;
    }
    else if(BatVol >= (3860 * X))
    {
        ret = 80;
    }
    else if(BatVol >= (3780 * X))
    {
        ret = 70;
    }
    else if(BatVol >= (3700 * X))
    {
        ret = 60;
    }
    else if(BatVol >= (3670 * X))
    {
        ret = 50;
    }
    else if(BatVol >= (3620 * X))
    {
        ret = 40;
    }
    else if(BatVol >= (3590 * X))
    {
        ret = 30;
    }
    else if(BatVol >= (3510 * X))
    {
        ret = 20;
    }
    else if(BatVol >= (3480 * X))
    {
        ret = 15;
    }
    else if(BatVol >= (3300 * X))
    {
        ret = 10;
    }
    else if(BatVol >= (3250 * X))
    {
        ret = 5;
    }
    else if(BatVol >= (3150 * X))
    {
        ret = 4;
    }
    else if(BatVol >= (3100 * X))
    {
        ret = 3;
    }
    else if(BatVol >= (2950 * X))
    {
        ret = 2;
    }
    else if(BatVol >= (2900 * X))
    {
        ret = 1;
    }
    else
    {
        ret = 0;
    }

    return ret;
}


void BatHwInit(void)
{
    ADC1_Init();
    ADC1_DMA1_CHANNEL1_Init();
    ADC1_Start();
}

uint16_t Bat_GetADCChannelValue(ADC_CHAN Ch)
{
    uint16_t ADCValue[ADC_SAMPLE_CNT];
    uint32_t Cnt, Sum = 0;

    for(Cnt = 0; Cnt < ADC_SAMPLE_CNT; Cnt++)
    {
        ADCValue[Cnt] = ADCConvertedValue[Cnt][Ch];
    }

    Bubble_Sort_U16(ADCValue, ADC_SAMPLE_CNT);
    Sum = Protocal_GetU16Sum(&ADCValue[2], ADC_SAMPLE_CNT - 4);
    return Sum / (ADC_SAMPLE_CNT - 4);
}

uint16_t Bat_GetExBatValue(void)
{
    uint16_t ADCValRef, ADCValExtBat, ADCValIntBat;
    uint32_t Tmp;
    BAT_MANAGE *pMag = &g_sBatTaskMag;

    ADCValRef = Bat_GetADCChannelValue(ADC_CHANNEL_2_5V);
    ADCValExtBat = Bat_GetADCChannelValue(ADC_CHANNEL_EXBAT);

    Tmp =  ADCValExtBat * pMag->m_wFactor * 2.5;
    Tmp /= ADCValRef;

    return Tmp;
}

/****************************************************************
@FUNCTION：电池管理任务重置分压系数（*1000）
@INPUT：
    @param New - 新分压系数
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_SetFactor(uint16_t New)
{
    BAT_MANAGE *pMag = &g_sBatTaskMag;

    pMag->m_wFactor = New;
}

/****************************************************************
@FUNCTION：电池管理任务获取当前电压值（*1000）
@INPUT： 无
@OUTPUT: 当前电压值
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
uint16_t BatTask_GetExBatVol(void)
{
    BAT_MANAGE *pMag = &g_sBatTaskMag;

    return pMag->m_wBatVol;
}

/****************************************************************
@FUNCTION：电池管理任务获取当前电池剩余电量
@INPUT： 无
@OUTPUT: 剩余电量0-100
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_GetExBatRes(void)
{
    return GetBatRemainingCapacity_18650(BatTask_GetExBatVol(), 1);
}



/****************************************************************
@FUNCTION：电池管理任务初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_Init(void)
{
    BAT_MANAGE *pMag = &g_sBatTaskMag;

    BatHwInit();

    memset(pMag, 0, sizeof(BAT_MANAGE));

    //分压系数
    BatTask_SetFactor(2000);

    //设置调度时间
    pMag->m_dwScheduleTime = 2 * 1000;
}


/****************************************************************
@FUNCTION：电池管理任务反初始�
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_DeInit(void)
{
    BAT_MANAGE *pMag = &g_sBatTaskMag;

    memset(pMag, 0, sizeof(BAT_MANAGE));

}

/****************************************************************
@FUNCTION：电池管理任务处理
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_Process(void)
{
    BAT_MANAGE *pMag = &g_sBatTaskMag;

    uint32_t NowTime = GetSysRunMilliSecond();
    uint32_t VolTmp;

    //首次上电，获取初值
    if(FALSE == pMag->m_bIsInit)
    {
        if(NowTime > 2000 && NowTime < 5000)
        {
            pMag->m_wBatVol = Bat_GetExBatValue();
            pMag->m_bIsInit = TRUE;
        }

        return;
    }


    //定时任务
    if((NowTime >  pMag->m_dwLastScheduleTime) &&
            (NowTime -  pMag->m_dwLastScheduleTime >= pMag->m_dwScheduleTime) )
    {
        pMag->m_dwLastScheduleTime = NowTime;

        VolTmp = Bat_GetExBatValue();
        //一阶滞后滤波得到处理后的新值，前80%，后20%
        VolTmp = pMag->m_wBatVol * 8 + VolTmp * 2;
        VolTmp /= 10;
        pMag->m_wBatVol = VolTmp;
        //LOG("BatVal:%d\n\r", pMag->m_wBatVol);
    }


}

#if 0
//判断是否在充电中
u8 IsStartCharge(BAT_MANAGE *pMag)
{
    if(0 == PORT_GetBit(PPR_PORT, PPR_PIN) && (0 == PORT_GetBit(CHG_PORT, CHG_PIN)))
    {
        pMag->m_IsChargeSrc = 1;
    }
    else if(1 == PORT_GetBit(CHG_PORT, CHG_PIN))
    {
        pMag->m_IsChargeSrc = 0;
    }

    return pMag->m_IsChargeSrc;
}


//充电时的处理函数
void ChargeHandler(BAT_MANAGE *pMag)
{
    if(1 == IsStartCharge(pMag))
    {
        pMag->m_IsLastModeBre = TRUE;
        Led_SetMode(&g_sLedMag, HL1, LED_MODE_BRE, 0);
        Led_Start(&g_sLedMag, HL1);
    }
    else if((0 == IsStartCharge(pMag)) && (TRUE == pMag->m_IsLastModeBre))
    {
        pMag->m_IsLastModeBre = FALSE;
        Led_Stop(&g_sLedMag, HL1);
    }
}

#endif


void ChargeHandle(void)
{


}




#if 0
/*
 ************************************************************************************************************************************************************************
 *函数名称: ADC_GetExtBat
 *功能描述: 获取外部电池电压ADC采样值
 *输入描述: 浮点型电压值、ADC原始值
 *输出描述: 无
 *返回描述: 0成功/1未更新
 *作者日期: XFW
 *全局声明: 无
 *特殊说明: ADC_SAMPLE_CNT-指定ADC采样多少次后再处理  ADC_REFERENCE-ADC参考基准
 ***********************************************************************************************************************************************************************/

extern u32 ADC_GetExtBat(float *pVol, u16 *pADCVal)
{
    static u16 temp[ADC_SAMPLE_CNT];
    static u8 index;
    u32 adcx;

    temp[index++] = Get_Adc(ADC_Channel_1);

    if(index == ADC_SAMPLE_CNT)
    {
        index = 0;
        ChooseSort_U16(temp, ADC_SAMPLE_CNT);
        adcx = MakeU16Sum(&temp[2], ADC_SAMPLE_CNT - 4); //去掉最值
        adcx /= (ADC_SAMPLE_CNT - 4); //取平均

        if(pVol != NULL)
        {
            *pVol = (float)adcx * (ADC_REFERENCE / 4096);
            //printf("ExtBat=%fV,0x%x\r\n",*pVol,adcx);
        }

        if(pADCVal != NULL)
        {
            *pADCVal = adcx;
        }

        return 0;
    }

    return 1;
}

#endif

