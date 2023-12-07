#include "battery_task.h"
#include "adc.h"
#include "main_task.h"
#include "sort.h"

BAT_MANAGE g_sBatTaskMag;


//���ݵ�ص�ѹ(*1000)��ȡ18650���ʣ������
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
    AdcConfig();
}

//����һ��ADC�ɼ�
void BatSample(void)
{
    #if defined(AT32F4xx)
    ADC1_Start();
    #elif defined(HC32F46x)
    static uint8_t SampleCnt;

    ADC_PollingSa(M4_ADC1, (uint16_t *)ADCConvertedValue[SampleCnt % ADC_SAMPLE_CNT], ADC_CHANNEL_CNT, 5); //����
    SampleCnt += 1;

    #endif

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
@FUNCTION����ع����������÷�ѹϵ����*1000��
@INPUT��
    @param New - �·�ѹϵ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_SetFactor(uint16_t New)
{
    BAT_MANAGE *pMag = &g_sBatTaskMag;

    pMag->m_wFactor = New;
}

/****************************************************************
@FUNCTION����ع��������ȡ��ǰ��ѹֵ��*1000��
@INPUT�� ��
@OUTPUT: ��ǰ��ѹֵ
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
uint16_t BatTask_GetExBatVol(void)
{
    BAT_MANAGE *pMag = &g_sBatTaskMag;

    return pMag->m_wBatVol;
}

/****************************************************************
@FUNCTION����ع��������ȡ��ǰ���ʣ�����
@INPUT�� ��
@OUTPUT: ʣ�����0-100
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
uint16_t BatTask_GetExBatRes(void)
{
    return GetBatRemainingCapacity_18650(BatTask_GetExBatVol(), 1);
}



/****************************************************************
@FUNCTION����ع��������ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_Init(void)
{
    BAT_MANAGE *pMag = &g_sBatTaskMag;

    BatHwInit();

    memset(pMag, 0, sizeof(BAT_MANAGE));

    //��ѹϵ��
    BatTask_SetFactor(2000);

    //���õ���ʱ��
    pMag->m_dwScheduleTime = 30 * 1000;
}


/****************************************************************
@FUNCTION����ع������񷴳�ʼ�
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_DeInit(void)
{
    BAT_MANAGE *pMag = &g_sBatTaskMag;

    memset(pMag, 0, sizeof(BAT_MANAGE));

}

/****************************************************************
@FUNCTION����ع���������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sBatTaskMag
****************************************************************/
void BatTask_Process(void)
{
    BAT_MANAGE *pMag = &g_sBatTaskMag;

    uint32_t NowTime = GetSysRunMilliSecond();
    uint32_t VolTmp;

    //ÿ100ms����ʱ����һ��
    if(NowTime % 100 == 0)
    {
        BatSample();
    }

    //�״��ϵ磬��ȡ��ֵ
    if(FALSE == pMag->m_bIsInit)
    {
        if(NowTime > 2000 && NowTime < 5000)
        {
            pMag->m_wBatVol = Bat_GetExBatValue();
            pMag->m_byBatRes = BatTask_GetExBatRes();
            #ifdef SEND_TO_UPCOM
            ResComTask_SendToUpcom((uint8_t *)&pMag->m_byBatRes, 1, 0x18, CTRL_DAT);
            #endif
            pMag->m_bIsInit = TRUE;
        }

        return;
    }


    //��ʱ����
    if((NowTime >  pMag->m_dwLastScheduleTime) &&
            (NowTime -  pMag->m_dwLastScheduleTime >= pMag->m_dwScheduleTime) )
    {
        pMag->m_dwLastScheduleTime = NowTime;

        VolTmp = Bat_GetExBatValue();
        //һ���ͺ��˲��õ���������ֵ��ǰ80%����20%
        VolTmp = pMag->m_wBatVol * 8 + VolTmp * 2;
        VolTmp /= 10;
        pMag->m_wBatVol = VolTmp;
        pMag->m_byBatRes = BatTask_GetExBatRes();
				
				BatteryReplacementTask(pMag->m_byBatRes, &g_sUserParam.m_byEnableChangeBat);
        #ifdef SEND_TO_UPCOM
        ResComTask_SendToUpcom((uint8_t *)&pMag->m_byBatRes, 1, 0x18, CTRL_DAT);
        #endif
        //LOG("BatVal:%d\n\r", pMag->m_wBatVol);
    }


}

#if 0
//�ж��Ƿ��ڳ����
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


//���ʱ�Ĵ�����
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
 *��������: ADC_GetExtBat
 *��������: ��ȡ�ⲿ��ص�ѹADC����ֵ
 *��������: �����͵�ѹֵ��ADCԭʼֵ
 *�������: ��
 *��������: 0�ɹ�/1δ����
 *��������: XFW
 *ȫ������: ��
 *����˵��: ADC_SAMPLE_CNT-ָ��ADC�������ٴκ��ٴ���  ADC_REFERENCE-ADC�ο���׼
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
        adcx = MakeU16Sum(&temp[2], ADC_SAMPLE_CNT - 4); //ȥ����ֵ
        adcx /= (ADC_SAMPLE_CNT - 4); //ȡƽ��

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

