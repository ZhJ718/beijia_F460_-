/******************************************************************
*版权所有 (C)2018, 北京华如科技南京分公司
*
*文件名称:  wtn6.c
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者:
*完成日期:  2018/7/11
*
*修改记录1:
*    修改日期:
*    版 本 号:
*    修 改 人:
*    修改内容:
******************************************************************/

/******************************************************************
*                             头文件                              *
******************************************************************/
#define CPP_GLOBALS //C文件需要包含此宏定义

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
*函数名称:TMR6_GLOBAL_IRQHandler
*功能描述:TIM6中断服务程序,用于左侧声道发声
*输入参数:无
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
******************************************************************/

void TMR6_GLOBAL_IRQHandler(void)   //TIM6中断
{
    if (TMR_GetINTStatus(TMR6, TMR_INT_Overflow) != RESET)  //检查TIM3更新中断发生与否
    {
        TMR_ClearITPendingBit(TMR6, TMR_INT_Overflow); //清除TIMx更新中断标志

        TMR_Cmd(TMR6, DISABLE); //开启定时器6

        g_sWTN6LeftMag.m_sSoftOps.step_ops((void *)&g_sWTN6LeftMag);

    }
}

/******************************************************************
*函数名称:TMR7_GLOBAL_IRQHandler
*功能描述:TIM7中断服务程序,用于左侧声道发声
*输入参数:无
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
******************************************************************/

void TMR7_GLOBAL_IRQHandler(void)   //TIM7中断
{
    if (TMR_GetINTStatus(TMR7, TMR_INT_Overflow) != RESET)	//检查TIM3更新中断发生与否
    {
        TMR_ClearITPendingBit(TMR7, TMR_INT_Overflow);	//清除TIMx更新中断标志

        TMR_Cmd(TMR7, DISABLE); //开启定时器6

        g_sWTN6RightMag.m_sSoftOps.step_ops((void *)&g_sWTN6RightMag);

    }

}
#elif defined(HC32F46x)

//绑定中断号
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
*使用Timer4_unit2
*使用Pclk1作为时钟源 Pclk1 = 168/2 = 84M
*10ms产生一次中断
************************************************************
*/
void Timer4_Unit2_init(uint16_t arr)
{
    //定时器4结构体和中断结构体
    stc_timer4_cnt_init_t stcCntInit;
    en_int_src_t enIntSrc;
    IRQn_Type enIRQn;

    /* Clear structures */
    MEM_ZERO_STRUCT(stcCntInit);


    /* Enable peripheral clock */
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM42, Enable);

    /* Timer4-CNT : Initialize CNT configuration structure */
    stcCntInit.enBufferCmd  = Disable;    			    //不允许缓冲
    stcCntInit.enClk        = Timer4CntPclk;   			//使能定时器时钟来源
    stcCntInit.enClkDiv     = Timer4CntPclkDiv4;   		/* Timer4-CNT clock divide */
    stcCntInit.u16Cycle     = 21 * arr - 1;  					/* Timer4-CNT cycle */ //84000000/4/21 = 1MHZ,即1us周期
    stcCntInit.enCntMode    = Timer4CntSawtoothWave;    //锯齿波
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
@FUNCTION：TIM4微秒级中断
@INPUT：中断us数
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 中断时间需大于0，若为0，则中断10us
****************************************************************/
void TIM4_Unit2_Sleep_Us(uint16_t nus)	//10us的中断
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
//右侧喇叭发声
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

    TIM7_Int_Init(2, 71); //随便延时个时间进入中断
}


//左侧喇叭发声
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

    TIM6_Int_Init(2, 71); //随便延时个时间进入中断


}

//喇叭是否发声中
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
@FUNCTION：语音模块异步操作进程
@INPUT：WTN6管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 定时中断中调用
****************************************************************/
void Wtn6_StepProcess(void *p)
{
    WTN6_MAG *pMag = p;
    volatile WTN6_STEP_MAG *pSingle = &pMag->m_sSingle[pMag->m_byNowStep];

    //判断当前所处步骤
    if(pMag->m_byNowStep < MIN(pMag->m_byMaxStep, WTN6_MAG_STEP_MAX))
    {

        //引脚操作
        pSingle->m_IsHigh == TRUE ? pMag->m_sHwOps.data_high() : pMag->m_sHwOps.data_low();

        //重置延时时间
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
@FUNCTION：语音模块初始化
@INPUT：BLE底层管理结构体指针
@OUTPUT：无
@RETURN: TRUE-正在执行底层定时调度， FALSE-无底层定时调度，可执行上层应用定时调度
@AUTHOR：xfw
@SPECIAL: 实现模块复位
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

    //IO初始化
    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    //数据线拉高
    if(pMag->m_sHwOps.data_high != NULL)
    {
        pMag->m_sHwOps.data_high();
    }

}

/****************************************************************
@FUNCTION：完成左右声道语音模块初始化
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 初始化结构体g_sWTN6LeftMag、g_sWTN6RightMag，填充硬件操作函数和异步操作逻辑函数
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
@FUNCTION：语音芯片发声
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 此函数被外部调用的话，会越过当前播放队列，直接播放，慎用
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

    pMag->m_sSoftOps.sleep_us(2);//随便延时个时间进入中断

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
*函数名称:play_online
*功能描述:播放装备上线语音
*输入参数:
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
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
*函数名称:play_offline
*功能描述:播放装备下线语音
*输入参数:
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
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
*函数名称:play_Status
*功能描述:播放主体或武器当前状态
*输入参数:当前状态 参见 ENUM_UNITE_STATE //1-正常 2-阵亡锁死 3-后台锁死 4-后台复活 5-裁判锁死 6-裁判复活 7-超距锁死 8-缺弹锁死
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
******************************************************************/
void play_Status(u8 Sta)
{
    switch(Sta)
    {
        //case 1://正常
        case 4://后台复活
        case 6://裁判复活
            Wtn6_Play(WTN6_Revive);
            break;

        case 2://阵亡锁死

            Wtn6_Play(WTN6_Killed);
            break;

        case 3://后台锁死
        case 5://裁判锁死
            Wtn6_Play(WTN6_JudgeDie);
            break;

        case 7://超距锁死
            Wtn6_Play(WTN6_OutRange);
            break;

        case 8://缺弹锁死
            Wtn6_Play(WTN6_ExhaustAmmunition);
            break;

        default:
            break;
    }
}

/******************************************************************
*函数名称:play_StruckPart
*功能描述:播放中弹部位
*输入参数:
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
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
        switch(weaponid)//武器类型见<laser.h>
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

//将数据压入环形缓冲区
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

//从环形缓冲区取出下一个数据
static uint32_t wtn6_QueuePop(VOICE_RING *pRing, u8 *pVoice)
{
    assert_param(pRing != NULL);

    //当前缓冲空
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
@FUNCTION：右声道播放语音
@INPUT：语音编号
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 此函数调用后，语音可能不会立即播放
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
@FUNCTION：左声道播放语音
@INPUT：语音编号
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 此函数调用后，语音可能不会立即播放
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
@FUNCTION：根据传入的语音，分到左右声道，插入播放队列
@INPUT：语音编号
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 此函数调用后，语音可能不会立即播放
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
@FUNCTION：语音芯片定时任务，根据播放队列中内容，开启播放
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 枪声可能会打断当前播放，其他语音等待上次语音播放完毕才会播放
****************************************************************/
void  Wtn6_TimedProcess(void)
{
    //static u8 IntervalFlg = FALSE;
    //static u8 IntervalCnt = 0;

    u8 Dat = 0;
    //static u8 LastDat;

    #if defined(ENABLE_WTN6_1)

    //播放左声道声音
    if((!g_sWTN6LeftMag.m_sHwOps.is_busy()) && (TRUE == wtn6_QueuePop(&g_sVoiceMag.m_sLVoiceRing, &Dat)))
    {
        if(Dat != 0)
        {
            //LastDat = Dat;//过滤掉重复提示音
            Wtn6_StartLSound(Dat);
        }
    }

    #endif

    #if defined(ENABLE_WTN6_2)

    //播放右声道声音
    if((!g_sWTN6RightMag.m_sHwOps.is_busy()) && (TRUE == wtn6_QueuePop(&g_sVoiceMag.m_sRVoiceRing, &Dat)))
    {
        if(Dat != 0)
        {
            //LastDat = Dat;//过滤掉重复提示音
            Wtn6_StartRSound(Dat);
        }
    }

    #endif

}

//夜间音量
extern void Wtn6_NightSound(void)
{
    #if defined(ENABLE_WTN6_1)
    Wtn6_StartSound(0xE2, &g_sWTN6LeftMag);
    #endif
    #if defined(ENABLE_WTN6_2)
    Wtn6_StartSound(0xE2, &g_sWTN6RightMag);
    #endif
}

//白天音量
extern void Wtn6_DaySound(void)
{
    #if defined(ENABLE_WTN6_1)
    Wtn6_StartSound(0xEF, &g_sWTN6LeftMag);
    #endif
    #if defined(ENABLE_WTN6_2)
    Wtn6_StartSound(0xEF, &g_sWTN6RightMag);
    #endif
}
