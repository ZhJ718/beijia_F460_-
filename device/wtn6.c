/******************************************************************
*��Ȩ���� (C)2018, ��������Ƽ��Ͼ��ֹ�˾
*
*�ļ�����:  wtn6.c
*�ļ���ʶ:
*����ժҪ:
*����˵��:
*��ǰ�汾:
*��    ��:
*�������:  2018/7/11
*
*�޸ļ�¼1:
*    �޸�����:
*    �� �� ��:
*    �� �� ��:
*    �޸�����:
******************************************************************/

/******************************************************************
*                             ͷ�ļ�                              *
******************************************************************/
#define CPP_GLOBALS //C�ļ���Ҫ�����˺궨��

#include "wtn6.h"
#include "gpio.h"
#include "timer.h"

#if defined(ENABLE_WTN6_1)
    WTN6_MAG g_sWTN6LeftMag;
#endif

#if defined(ENABLE_WTN6_2)
    WTN6_MAG g_sWTN6RightMag;
#endif

#if defined(AT32F4xx)
/******************************************************************
*��������:TMR6_GLOBAL_IRQHandler
*��������:TIM6�жϷ������,���������������
*�������:��
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/

void TMR6_GLOBAL_IRQHandler(void)   //TIM6�ж�
{
    if (TMR_GetINTStatus(TMR6, TMR_INT_Overflow) != RESET)  //���TIM3�����жϷ������
    {
        TMR_ClearITPendingBit(TMR6, TMR_INT_Overflow); //���TIMx�����жϱ�־

        TMR_Cmd(TMR6, DISABLE); //������ʱ��6

        g_sWTN6LeftMag.m_sSoftOps.step_ops((void *)&g_sWTN6LeftMag);

    }
}

/******************************************************************
*��������:TMR7_GLOBAL_IRQHandler
*��������:TIM7�жϷ������,���������������
*�������:��
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/

void TMR7_GLOBAL_IRQHandler(void)   //TIM7�ж�
{
    if (TMR_GetINTStatus(TMR7, TMR_INT_Overflow) != RESET)	//���TIM3�����жϷ������
    {
        TMR_ClearITPendingBit(TMR7, TMR_INT_Overflow);	//���TIMx�����жϱ�־

        TMR_Cmd(TMR7, DISABLE); //������ʱ��6

        g_sWTN6RightMag.m_sSoftOps.step_ops((void *)&g_sWTN6RightMag);

    }

}
#elif defined(HC32F46x)

//���жϺ�
static void IrqRegistration(en_int_src_t enIntSrc, IRQn_Type enIRQn)
{
    stc_intc_sel_field_t *stcIntSel = NULL;

    /* IRQ032~127 whether out of range */
    if ((enIRQn >= 32) && ((((enIntSrc / 32) * 6 + 32) > enIRQn) || (((enIntSrc / 32) * 6 + 37) < enIRQn)))
    {
        return;
    }
    else
    {
        stcIntSel = (stc_intc_sel_field_t *)((uint32_t)(&M4_INTC->SEL0)  +  (4u * enIRQn));
        //if (0x1FFu == stcIntSel->INTSEL)
        {
            stcIntSel->INTSEL = enIntSrc;
        }
    }
}

/*
************************************************************
*ʹ��Timer4_unit2
*ʹ��Pclk1��Ϊʱ��Դ Pclk1 = 168/2 = 84M
*10ms����һ���ж�
************************************************************
*/
void Timer4_Unit2_init(uint16_t arr)
{
    //��ʱ��4�ṹ����жϽṹ��
    stc_timer4_cnt_init_t stcCntInit;
    en_int_src_t enIntSrc;
    IRQn_Type enIRQn;

    /* Clear structures */
    MEM_ZERO_STRUCT(stcCntInit);


    /* Enable peripheral clock */
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM42, Enable);

    /* Timer4-CNT : Initialize CNT configuration structure */
    stcCntInit.enBufferCmd  = Disable;    			    //��������
    stcCntInit.enClk        = Timer4CntPclk;   			//ʹ�ܶ�ʱ��ʱ����Դ
    stcCntInit.enClkDiv     = Timer4CntPclkDiv4;   		/* Timer4-CNT clock divide */
    stcCntInit.u16Cycle     = 21 * arr - 1;  					/* Timer4-CNT cycle */ //84000000/4/21 = 1MHZ,��1us����
    stcCntInit.enCntMode    = Timer4CntSawtoothWave;    //��ݲ�
    stcCntInit.enZeroIntCmd = Disable;    			    /* Disable zero match interrupt */
    stcCntInit.enPeakIntCmd = Enable;   			    /* Enable peak match interrupt */
    stcCntInit.enZeroIntMsk = Timer4CntIntMask0;
    stcCntInit.enPeakIntMsk = Timer4CntIntMask0;
    TIMER4_CNT_Init(M4_TMR42, &stcCntInit); 		    /* Initialize Timer4-CNT */

    /* Set Timer4-CNT IRQ */

    enIntSrc = INT_TMR42_GOVF;
    enIRQn = Int004_IRQn;

    IrqRegistration(enIntSrc, enIRQn);

    NVIC_SetPriority(enIRQn, 4);
    NVIC_ClearPendingIRQ(enIRQn);
    NVIC_EnableIRQ(enIRQn);


    /* Start Timer4-CNT */
    TIMER4_CNT_ClearCountVal(M4_TMR42);
    TIMER4_CNT_Start(M4_TMR42);
}

/****************************************************************
@FUNCTION��TIM4΢�뼶�ж�
@INPUT���ж�us��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �ж�ʱ�������0����Ϊ0�����ж�10us
****************************************************************/
void TIM4_Unit2_Sleep_Us(uint16_t nus)	//10us���ж�
{
    if(nus == 0)
    {
        Timer4_Unit2_init(1);
    }
    else
    {
        Timer4_Unit2_init(nus);
    }
}

void IRQ004_Handler(void)
{
    if(TIMER4_CNT_GetIrqFlag(M4_TMR42, Timer4CntPeakMatchInt))
    {
        /* Clear Timer4-CNT Peak interrupt flag */
        TIMER4_CNT_ClearIrqFlag(M4_TMR42, Timer4CntPeakMatchInt);
        TIMER4_CNT_Stop(M4_TMR42);
        g_sWTN6LeftMag.m_sSoftOps.step_ops((void *)&g_sWTN6LeftMag);

    }
}


#endif



#if 0
//�Ҳ����ȷ���
void wtn6_RightSound(u8 SB_DATA)
{

    volatile u8 S_DATA;
    volatile u8 B_DATA, j;
    S_DATA = SB_DATA;

    #if 0
    WNT6_DATA = 0;
    delay_ms(5);
    B_DATA = S_DATA & 0x01;

    for(j = 0; j < 8; j++)
    {
        if(B_DATA == 1)
        {
            WNT6_DATA = 1;
            delay_us(600);
            WNT6_DATA = 0;
            delay_us(200);
        }
        else
        {
            WNT6_DATA = 1;
            delay_us(200);
            WNT6_DATA = 0;
            delay_us(600);
        }

        S_DATA = S_DATA >> 1;
        B_DATA = S_DATA & 0x01;
    }

    WNT6_DATA = 1;
    #endif

    memset(&g_sWTN6RightMag, 0, sizeof(WTN6_MAG));
    j = 0;

    g_sWTN6RightMag.m_sSingle[j].m_IsHigh = FALSE;

    g_sWTN6RightMag.m_sSingle[j].m_wDelayUs = 5000;
    j += 1;

    B_DATA = S_DATA & 0x01;

    for(; j < 17; j += 2)
    {
        if(B_DATA == 1)
        {
            g_sWTN6RightMag.m_sSingle[j].m_IsHigh = TRUE;

            g_sWTN6RightMag.m_sSingle[j].m_wDelayUs = 600;
            g_sWTN6RightMag.m_sSingle[j + 1].m_IsHigh = FALSE;

            g_sWTN6RightMag.m_sSingle[j + 1].m_wDelayUs = 200;
        }
        else
        {
            g_sWTN6RightMag.m_sSingle[j].m_IsHigh = TRUE;

            g_sWTN6RightMag.m_sSingle[j].m_wDelayUs = 200;
            g_sWTN6RightMag.m_sSingle[j + 1].m_IsHigh = FALSE;

            g_sWTN6RightMag.m_sSingle[j + 1].m_wDelayUs = 600;
        }

        S_DATA = S_DATA >> 1;
        B_DATA = S_DATA & 0x01;
    }

    g_sWTN6RightMag.m_sSingle[j].m_IsHigh = TRUE;

    g_sWTN6RightMag.m_sSingle[j].m_wDelayUs = 100;
    j += 1;

    g_sWTN6RightMag.m_byMaxStep = j;

    TIM7_Int_Init(2, 71); //�����ʱ��ʱ������ж�
}


//������ȷ���
void wtn6_LeftSound(u8 SB_DATA)
{

    volatile u8 S_DATA;
    volatile u8 B_DATA, j;
    S_DATA = SB_DATA;

    #if 0

    WNT6_DATA2 = 0;
    delay_ms(5);
    B_DATA = S_DATA & 0x01;

    for(j = 0; j < 8; j++)
    {
        if(B_DATA == 1)
        {
            WNT6_DATA2 = 1;
            delay_us(600);
            WNT6_DATA2 = 0;
            delay_us(200);
        }
        else
        {
            WNT6_DATA2 = 1;
            delay_us(200);
            WNT6_DATA2 = 0;
            delay_us(600);
        }

        S_DATA = S_DATA >> 1;
        B_DATA = S_DATA & 0x01;
    }

    WNT6_DATA2 = 1;
    #endif

    memset(&g_sWTN6LeftMag, 0, sizeof(WTN6_MAG));
    j = 0;

    g_sWTN6LeftMag.m_sSingle[j].m_IsHigh = FALSE;

    g_sWTN6LeftMag.m_sSingle[j].m_wDelayUs = 5000;
    j += 1;

    B_DATA = S_DATA & 0x01;

    for(; j < 17; j += 2)
    {
        if(B_DATA == 1)
        {
            g_sWTN6LeftMag.m_sSingle[j].m_IsHigh = TRUE;

            g_sWTN6LeftMag.m_sSingle[j].m_wDelayUs = 600;
            g_sWTN6LeftMag.m_sSingle[j + 1].m_IsHigh = FALSE;

            g_sWTN6LeftMag.m_sSingle[j + 1].m_wDelayUs = 200;
        }
        else
        {
            g_sWTN6LeftMag.m_sSingle[j].m_IsHigh = TRUE;

            g_sWTN6LeftMag.m_sSingle[j].m_wDelayUs = 200;
            g_sWTN6LeftMag.m_sSingle[j + 1].m_IsHigh = FALSE;

            g_sWTN6LeftMag.m_sSingle[j + 1].m_wDelayUs = 600;
        }

        S_DATA = S_DATA >> 1;
        B_DATA = S_DATA & 0x01;
    }

    g_sWTN6LeftMag.m_sSingle[j].m_IsHigh = TRUE;

    g_sWTN6LeftMag.m_sSingle[j].m_wDelayUs = 100;
    j += 1;

    g_sWTN6LeftMag.m_byMaxStep = j;

    TIM6_Int_Init(2, 71); //�����ʱ��ʱ������ж�


}

//�����Ƿ�����
u32 wtn6_IsBusy(WTN6_MAG *pMag)
{
    return	pMag->m_sHwOps.is_busy();
}
#endif

#if 0
void wtn6_SyncSound(u8 SB_DATA)
{
    volatile u8 S_DATA;
    volatile u8 B_DATA, j;
    S_DATA = SB_DATA;

    g_sWTN6LeftMag.m_sHwOps.data_low();
    g_sWTN6RightMag.m_sHwOps.data_low();
    delay_ms(5);
    B_DATA = S_DATA & 0x01;

    for(j = 0; j < 8; j++)
    {
        if(B_DATA == 1)
        {
            g_sWTN6LeftMag.m_sHwOps.data_high();
            g_sWTN6RightMag.m_sHwOps.data_high();
            delay_us(600);
            g_sWTN6LeftMag.m_sHwOps.data_low();
            g_sWTN6RightMag.m_sHwOps.data_low();
            delay_us(200);
        }
        else
        {
            g_sWTN6LeftMag.m_sHwOps.data_high();
            g_sWTN6RightMag.m_sHwOps.data_high();
            delay_us(200);
            g_sWTN6LeftMag.m_sHwOps.data_low();
            g_sWTN6RightMag.m_sHwOps.data_low();
            delay_us(600);
        }

        S_DATA = S_DATA >> 1;
        B_DATA = S_DATA & 0x01;
    }

    g_sWTN6LeftMag.m_sHwOps.data_high();
    g_sWTN6RightMag.m_sHwOps.data_high();

}


#endif


/****************************************************************
@FUNCTION������ģ���첽��������
@INPUT��WTN6����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��ʱ�ж��е���
****************************************************************/
void Wtn6_StepProcess(void *p)
{
    WTN6_MAG *pMag = p;
    volatile WTN6_STEP_MAG *pSingle = &pMag->m_sSingle[pMag->m_byNowStep];

    //�жϵ�ǰ��������
    if(pMag->m_byNowStep < MIN(pMag->m_byMaxStep, WTN6_MAG_STEP_MAX))
    {

        //���Ų���
        pSingle->m_IsHigh == TRUE ? pMag->m_sHwOps.data_high() : pMag->m_sHwOps.data_low();

        //������ʱʱ��
        pMag->m_sSoftOps.sleep_us(pSingle->m_wDelayUs);

        pMag->m_byNowStep++;
    }
    else
    {
        pMag->m_byNowStep = 0;
    }
}

void Wtn6_1_HardWareInit(void)
{
    Wtn6_1_GpioBusyInit();
    Wtn6_1_GpioDataInit();
}

void Wtn6_2_HardWareInit(void)
{
    Wtn6_2_GpioBusyInit();
    Wtn6_2_GpioDataInit();
}


/****************************************************************
@FUNCTION������ģ���ʼ��
@INPUT��BLE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: TRUE-����ִ�еײ㶨ʱ���ȣ� FALSE-�޵ײ㶨ʱ���ȣ���ִ���ϲ�Ӧ�ö�ʱ����
@AUTHOR��xfw
@SPECIAL: ʵ��ģ�鸴λ
****************************************************************/
void  Wtn6_Init(WTN6_HW_OPERATIONS a_sHwOps, WTN6_SOFT_OPERATIONS a_sSoftOps, WTN6_MAG *pMag)
{
    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(WTN6_MAG));

    pMag->m_sHwOps.hw_init = a_sHwOps.hw_init;
    pMag->m_sHwOps.data_high = a_sHwOps.data_high;
    pMag->m_sHwOps.data_low = a_sHwOps.data_low;
    pMag->m_sHwOps.is_busy = a_sHwOps.is_busy;

    pMag->m_sSoftOps.sleep_us = a_sSoftOps.sleep_us;
    pMag->m_sSoftOps.step_ops = a_sSoftOps.step_ops;

    //IO��ʼ��
    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    //����������
    if(pMag->m_sHwOps.data_high != NULL)
    {
        pMag->m_sHwOps.data_high();
    }

}

/****************************************************************
@FUNCTION�����������������ģ���ʼ��
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��ʼ���ṹ��g_sWTN6LeftMag��g_sWTN6RightMag�����Ӳ�������������첽�����߼�����
****************************************************************/
void Wtn6_1_2_Init(void)
{
    #if defined(ENABLE_WTN6_1)
    WTN6_HW_OPERATIONS HwOps1 =
    {
        .hw_init = Wtn6_1_HardWareInit,
        .data_high = Wtn6_1_GpioDataSet,
        .data_low = Wtn6_1_GpioDataReset,
        .is_busy = Wtn6_1_IsGpioBusySet
    };

    WTN6_SOFT_OPERATIONS SoftOps1 =
    {
        .sleep_us = TIM4_Unit2_Sleep_Us,
        .step_ops = Wtn6_StepProcess
    };

    Wtn6_Init(HwOps1, SoftOps1, &g_sWTN6LeftMag);

    #endif

    #if defined(ENABLE_WTN6_2)
    WTN6_HW_OPERATIONS HwOps2 =
    {
        .hw_init = Wtn6_2_HardWareInit,
        .data_high = Wtn6_2_GpioDataSet,
        .data_low = Wtn6_2_GpioDataReset,
        .is_busy = Wtn6_2_IsGpioBusySet
    };

    WTN6_SOFT_OPERATIONS SoftOps2 =
    {
        .sleep_us = TIM7_Sleep_Us,
        .step_ops = Wtn6_StepProcess
    };

    Wtn6_Init(HwOps2, SoftOps2, &g_sWTN6RightMag);

    #endif


}


/****************************************************************
@FUNCTION������оƬ����
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �˺������ⲿ���õĻ�����Խ����ǰ���Ŷ��У�ֱ�Ӳ��ţ�����
****************************************************************/
void Wtn6_StartSound(u8 SB_DATA, WTN6_MAG *pMag)
{
    volatile u8 S_DATA;
    volatile u8 B_DATA, j;
    S_DATA = SB_DATA;

    #if 0
    WNT6_DATA = 0;
    delay_ms(5);
    B_DATA = S_DATA & 0x01;

    for(j = 0; j < 8; j++)
    {
        if(B_DATA == 1)
        {
            WNT6_DATA = 1;
            delay_us(600);
            WNT6_DATA = 0;
            delay_us(200);
        }
        else
        {
            WNT6_DATA = 1;
            delay_us(200);
            WNT6_DATA = 0;
            delay_us(600);
        }

        S_DATA = S_DATA >> 1;
        B_DATA = S_DATA & 0x01;
    }

    WNT6_DATA = 1;
    #endif

    //memset(&pMag,0,sizeof(WTN6_MAG));
    pMag->m_byNowStep = 0;

    j = 0;

    pMag->m_sSingle[j].m_IsHigh = FALSE;

    pMag->m_sSingle[j].m_wDelayUs = 5000;
    j += 1;

    B_DATA = S_DATA & 0x01;

    for(; j < 17; j += 2)
    {
        if(B_DATA == 1)
        {
            pMag->m_sSingle[j].m_IsHigh = TRUE;

            pMag->m_sSingle[j].m_wDelayUs = 600;
            pMag->m_sSingle[j + 1].m_IsHigh = FALSE;

            pMag->m_sSingle[j + 1].m_wDelayUs = 200;
        }
        else
        {
            pMag->m_sSingle[j].m_IsHigh = TRUE;

            pMag->m_sSingle[j].m_wDelayUs = 200;
            pMag->m_sSingle[j + 1].m_IsHigh = FALSE;

            pMag->m_sSingle[j + 1].m_wDelayUs = 600;
        }

        S_DATA = S_DATA >> 1;
        B_DATA = S_DATA & 0x01;
    }

    pMag->m_sSingle[j].m_IsHigh = TRUE;

    pMag->m_sSingle[j].m_wDelayUs = 100;
    j += 1;

    pMag->m_byMaxStep = j;

    pMag->m_sSoftOps.sleep_us(2);//�����ʱ��ʱ������ж�

}

#if defined(ENABLE_WTN6_1)
extern void Wtn6_StartLSound(uint8_t Dat)
{
    Wtn6_StartSound(Dat, &g_sWTN6LeftMag);
}
#endif

#if defined(ENABLE_WTN6_2)
extern void Wtn6_StartRSound(uint8_t Dat)
{
    Wtn6_StartSound(Dat, &g_sWTN6RightMag);
}
#endif

extern void Wtn6_Sound(uint8_t Dat)
{
    #if defined(ENABLE_WTN6_1)
    Wtn6_StartSound(Dat, &g_sWTN6LeftMag);
    #endif
    #if defined(ENABLE_WTN6_2)
    Wtn6_StartSound(Dat, &g_sWTN6RightMag);
    #endif
}

/******************************************************************
*��������:play_online
*��������:����װ����������
*�������:
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void play_online(u8 equipemtid)
{
    switch(equipemtid)
    {
        case 1:
            Wtn6_Play(WTN6_WeaponConnected);
            break;

        /*case 2:
        	wtn6_play(WTN6_BQConnected);
        	break;
        case 3:
        case 5:
        	wtn6_play(WTN6_JQConnected);
        	break;
        case 4:
        	wtn6_play(WTN6_JJQConnected);
        	break;
        case 6:
        	wtn6_play(WTN6_LDFSQConnected);
        	break;
        case 7:
        	wtn6_play(WTN6_DBHJTConnected);
        	break;	*/
        case 220:
            Wtn6_Play(WTN6_ClothConnected);
            break;

        case 254:
            Wtn6_Play(WTN6_WeaponConnected);
            break;

        default:

            break;
    }
}


/******************************************************************
*��������:play_offline
*��������:����װ����������
*�������:
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void play_offline(u8 equipemtid)
{
    switch(equipemtid)
    {
            /*case 1:
            	wtn6_play(WTN6_SQDisconnected);
            	break;
            case 2:
            	wtn6_play(WTN6_BQDisconnected);
            	break;
            case 3:
            case 5:
            	wtn6_play(WTN6_JQDisconnected);
            	break;
            case 4:
            	wtn6_play(WTN6_JJQDisconnected);
            	break;
            case 6:
            	wtn6_play(WTN6_LDFSQDisconnected);
            	break;
            case 7:
            	wtn6_play(WTN6_DBHJTDisconnected);
            	break;
            case 220:
            	wtn6_play(WTN6_ClothDisconnected);
            	break;
            default:
            	wtn6_play(WTN6_WeaponDisconnected);
            	break;	*/
    }
}

/******************************************************************
*��������:play_Status
*��������:���������������ǰ״̬
*�������:��ǰ״̬ �μ� ENUM_UNITE_STATE //1-���� 2-�������� 3-��̨���� 4-��̨���� 5-�������� 6-���и��� 7-�������� 8-ȱ������
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void play_Status(u8 Sta)
{
    switch(Sta)
    {
        //case 1://����
        case 4://��̨����
        case 6://���и���
            Wtn6_Play(WTN6_Revive);
            break;

        case 2://��������

            Wtn6_Play(WTN6_Killed);
            break;

        case 3://��̨����
        case 5://��������
            Wtn6_Play(WTN6_JudgeDie);
            break;

        case 7://��������
            Wtn6_Play(WTN6_OutRange);
            break;

        case 8://ȱ������
            Wtn6_Play(WTN6_ExhaustAmmunition);
            break;

        default:
            break;
    }
}

/******************************************************************
*��������:play_StruckPart
*��������:�����е���λ
*�������:
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void play_StruckPart(ENUM_PARTITION part)
{
    switch(part)
    {
        case PARTITION_HEAD:
            Wtn6_Play(WTN6_Shot_Head);
            break;

        case PARTITION_LARM:
            Wtn6_Play(WTN6_Shot_LeftArm);
            break;

        case PARTITION_RARM:
            Wtn6_Play(WTN6_Shot_RightArm);
            break;

        case PARTITION_LLEG:
            Wtn6_Play(WTN6_Shot_LeftLeg);
            break;

        case PARTITION_RLEG:
            Wtn6_Play(WTN6_Shot_RightLeg);
            break;

        case PARTITION_LCHEST:
            Wtn6_Play(WTN6_Shot_LeftChest);
            break;

        case PARTITION_RCHEST:
            Wtn6_Play(WTN6_Shot_RightChest);
            break;

        case PARTITION_BACK:
            Wtn6_Play(WTN6_Shot_Back);
            break;

        case PARTITION_ABDOMEN:
            Wtn6_Play(WTN6_Shot_Abdomen);
            break;

        default:
            break;
    }
}

void play_Shot(u8 weaponid, u8 Sta, u8 Num)
{
    if(Sta == 2 || Sta == 3 || Sta == 5)
    {
        Wtn6_Play(WTN6_Killed);
    }
    else if(Sta == 7)
    {
				Wtn6_Play(WTN6_OutRange);
    }
    else if(Sta == 8)
    {
        Wtn6_Play(WTN6_ExhaustAmmunition);
    }
    else
    {
        switch(weaponid)//�������ͼ�<laser.h>
        {
            case 1:
                Wtn6_Play(WTN6_YX_Shoot1);
                break;

            case 2:
                Wtn6_Play(WTN6_YX_Shoot2);
                break;

            case 3:
                Wtn6_Play(WTN6_YX_JQ);
                break;

            case 4:
                Wtn6_Play(WTN6_YX_JJ1);
                break;

            case 5:
                Wtn6_Play(WTN6_YX_ZJQ);
                break;

            default:
                break;
        }
    }
}



VOICE_MAG g_sVoiceMag;

//������ѹ�뻷�λ�����
static uint32_t wtn6_QueuePush(VOICE_RING *pRing, u8 Voice)
{
    assert_param(pRing != NULL);

    pRing->m_aBuf[pRing->m_byPushPos] = Voice;

    if(++pRing->m_byPushPos >= VOICE_CNT_MAX)
    {
        pRing->m_byPushPos = 0;
    }

    //LOG("Push %d in %d\n\r",Voice,pRing->m_byPushPos);
    return TRUE;
}

//�ӻ��λ�����ȡ����һ������
static uint32_t wtn6_QueuePop(VOICE_RING *pRing, u8 *pVoice)
{
    assert_param(pRing != NULL);

    //��ǰ�����
    if(pRing->m_byPopPos == pRing->m_byPushPos)
    {
        return FALSE;
    }

    if(pVoice != NULL)
    {
        *pVoice = pRing->m_aBuf[pRing->m_byPopPos];
    }

    if(++pRing->m_byPopPos >= VOICE_CNT_MAX)
    {
        pRing->m_byPopPos = 0;
    }

    //LOG("Pop %d in %d\n\r",*pVoice,pRing->m_byPopPos);
    return TRUE;
}


/****************************************************************
@FUNCTION����������������
@INPUT���������
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �˺������ú��������ܲ�����������
****************************************************************/
void Wtn6_RPlay(u8 Voice)
{
    #if defined(ENABLE_WTN6_1)
    //LOG("wtn6_play %d\n\r",Voice);
    volatile u8 Dat = Voice;

    switch(Dat)
    {
        case WTN6_Reserve:
            break;

        default:
            //LOG("VoiceRing:",Voice);
            wtn6_QueuePush(&g_sVoiceMag.m_sRVoiceRing, Dat);
            break;

    }

    #endif
}

/****************************************************************
@FUNCTION����������������
@INPUT���������
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �˺������ú��������ܲ�����������
****************************************************************/
void Wtn6_LPlay(u8 Voice)
{
    #if defined(ENABLE_WTN6_1)
    //LOG("wtn6_play %d\n\r",Voice);
    volatile u8 Dat = Voice;

    switch(Dat)
    {
        case WTN6_Reserve:
            break;

        default:
            //LOG("VoiceRing:",Voice);
            wtn6_QueuePush(&g_sVoiceMag.m_sLVoiceRing, Dat);
            break;

    }

    #endif
}

/****************************************************************
@FUNCTION�����ݴ�����������ֵ��������������벥�Ŷ���
@INPUT���������
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �˺������ú��������ܲ�����������
****************************************************************/
void Wtn6_Play(u8 Voice)
{
    #if defined(ENABLE_WTN6_1)
    Wtn6_LPlay(Voice);
    #endif

    #if defined(ENABLE_WTN6_2)
    Wtn6_RPlay(Voice);
    #endif


}

/****************************************************************
@FUNCTION������оƬ��ʱ���񣬸��ݲ��Ŷ��������ݣ���������
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ǹ�����ܻ��ϵ�ǰ���ţ����������ȴ��ϴ�����������ϲŻᲥ��
****************************************************************/
void  Wtn6_TimedProcess(void)
{
    //static u8 IntervalFlg = FALSE;
    //static u8 IntervalCnt = 0;

    u8 Dat = 0;
    //static u8 LastDat;

    #if defined(ENABLE_WTN6_1)

    //��������������
    if((!g_sWTN6LeftMag.m_sHwOps.is_busy()) && (TRUE == wtn6_QueuePop(&g_sVoiceMag.m_sLVoiceRing, &Dat)))
    {
        if(Dat != 0)
        {
            //LastDat = Dat;//���˵��ظ���ʾ��
            Wtn6_StartLSound(Dat);
        }
    }

    #endif

    #if defined(ENABLE_WTN6_2)

    //��������������
    if((!g_sWTN6RightMag.m_sHwOps.is_busy()) && (TRUE == wtn6_QueuePop(&g_sVoiceMag.m_sRVoiceRing, &Dat)))
    {
        if(Dat != 0)
        {
            //LastDat = Dat;//���˵��ظ���ʾ��
            Wtn6_StartRSound(Dat);
        }
    }

    #endif

}

//ҹ������
extern void Wtn6_NightSound(void)
{
    #if defined(ENABLE_WTN6_1)
    Wtn6_StartSound(0xE2, &g_sWTN6LeftMag);
    #endif
    #if defined(ENABLE_WTN6_2)
    Wtn6_StartSound(0xE2, &g_sWTN6RightMag);
    #endif
}

//��������
extern void Wtn6_DaySound(void)
{
    #if defined(ENABLE_WTN6_1)
    Wtn6_StartSound(0xEF, &g_sWTN6LeftMag);
    #endif
    #if defined(ENABLE_WTN6_2)
    Wtn6_StartSound(0xEF, &g_sWTN6RightMag);
    #endif
}
