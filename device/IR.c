#include "IR.h"

volatile Ir_Tx_Msg ir_tx_msg;
Ir_Rcv_Msg ir_rcv_msg;
uint32_t us_num = 0;
//uint8_t ir_rx_buf[Ir_Max_Rcv*8];									//����������ݻ�����
uint8_t ir_rx_buf;
//uint32_t ir_rx_num = 0;
//uint16_t ir_rx_cnt = 0;

void Tx_Pin_Tog(void);
void Ir_Tx_Handle(void);
void Ir_Rcv_Edge_Handle(void);
void Ir_Rcv_Handle(void);

#if defined(AT32F4xx)

void EXTI0_IRQHandler(void)
{
    if(EXTI_GetIntStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearIntPendingBit(EXTI_Line0);
        Ir_Rcv_Edge_Handle();

    }
}

void TMR5_GLOBAL_IRQHandler(void)   //TIM3�ж�
{
    if (TMR_GetINTStatus(TMR5, TMR_INT_Overflow) != RESET)  //���TIM3�����жϷ������
    {
        TMR_ClearITPendingBit(TMR5, TMR_INT_Overflow); //���TIMx�����жϱ�־
        Tx_Pin_Tog();
    }
}

void TIM5_Int_Init(u16 arr, u16 psc)
{
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR5, ENABLE); //ʱ��ʹ��

    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TMR_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TMR_DIV = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM���ϼ���ģʽ
    TMR_TimeBaseInit(TMR5, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TMR_INTConfig(TMR5, TMR_INT_Overflow, ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

    //�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = TMR5_GLOBAL_IRQn;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

    TMR_Cmd(TMR5, DISABLE);  //ʹ��TIMx
}


void TMR2_GLOBAL_IRQHandler(void)
{
    if (TMR_GetINTStatus(TMR2, TMR_INT_Overflow) != RESET)
    {
        TMR_ClearITPendingBit(TMR2, TMR_INT_Overflow); //���TIMx�����жϱ�־
        Ir_Tx_Handle();
    }
}


//void TIM6_Int_Init(u16 arr,u16 psc)
//{
//	NVIC_InitType NVIC_InitStructure;
//	TMR_TimerBaseInitType  TIM_TimeBaseStructure;

//	RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR6, ENABLE);//TIM6ʱ��ʹ��
//
//	//��ʱ��TIM7��ʼ��
//	TIM_TimeBaseStructure.TMR_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
//	TIM_TimeBaseStructure.TMR_DIV =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
//	TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
//	TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM���ϼ���ģʽ
//	TMR_TimeBaseInit(TMR6, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
// 	TMR_ClearFlag(TMR6, TMR_FLAG_Update);
//	TMR_INTConfig(TMR6,TMR_INT_Overflow,ENABLE ); //ʹ��ָ����TIM6�ж�,��������ж�
//
//	/* ��ֹARRԤװ�ػ����� */
//	TMR_ARPreloadConfig(TMR6, DISABLE);
//
//	TMR_Cmd(TMR6,DISABLE);//������ʱ��6
//
//	NVIC_InitStructure.NVIC_IRQChannel = TMR6_GLOBAL_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;//��ռ���ȼ�0
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�2
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
//}

//TIM2��ʱ��
void TIM2_Int_Init(u16 arr, u16 psc)
{
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR2, ENABLE); //ʱ��ʹ��

    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TMR_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TMR_DIV = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM���ϼ���ģʽ
    TMR_TimeBaseInit(TMR2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TMR_INTConfig(TMR2, TMR_INT_Overflow, ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

    //�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = TMR2_GLOBAL_IRQn;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
    //
    #if 0
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR2, ENABLE); //ʱ��ʹ��

    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TMR_Period        = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TMR_DIV           = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode   = TMR_CounterDIR_Up;  //TIM���ϼ���ģʽ
    TMR_TimeBaseInit(TMR2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
    TMR_ClearFlag(TMR2, TMR_FLAG_Update);
    TMR_INTConfig(TMR2, TMR_INT_Overflow, ENABLE ); //ʹ��ָ����TIM6�ж�,��������ж�

    /* ��ֹARRԤװ�ػ����� */
    TMR_ARPreloadConfig(TMR2, DISABLE);

    TMR_Cmd(TMR2, DISABLE); //������ʱ��6

    //�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel                   = TMR2_GLOBAL_IRQn;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
    #endif
}



void TMR3_GLOBAL_IRQHandler(void)
{
    if (TMR_GetINTStatus(TMR3, TMR_INT_Overflow) != RESET)
    {
        TMR_ClearITPendingBit(TMR3, TMR_INT_Overflow); //���TIMx�����жϱ�־
        Ir_Rcv_Handle();
    }
}


/******************************************************************
*��������:TIM3_Int_Init
*��������:ͨ�ö�ʱ��3�жϳ�ʼ��
*�������:arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void TIM3_Int_Init(u16 arr, u16 psc)
{
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR3, ENABLE); //ʱ��ʹ��

    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TMR_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TMR_DIV = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM���ϼ���ģʽ
    TMR_TimeBaseInit(TMR3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TMR_INTConfig(TMR3, TMR_INT_Overflow, ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

    //�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = TMR3_GLOBAL_IRQn;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


    //TMR_Cmd(TMR3, ENABLE);  //ʹ��TIMx
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

void IRQ000_Handler(void)
{
    if (Set == TIMERA_GetFlag(M4_TMRA1, TimeraFlagOverflow))
    {
        TIMERA_ClearFlag(M4_TMRA1, TimeraFlagOverflow);
        Ir_Tx_Handle();
    }
}

void Timera1_Config(uint16_t arr)
{
    stc_timera_base_init_t stcTimeraInit;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcTimeraInit);

    /* Configuration peripheral clock */
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA1, Enable);

    /* Configuration timera  base structure */
    stcTimeraInit.enClkDiv = TimeraPclkDiv2;  //Pclk1 - 84M
    stcTimeraInit.enCntMode = TimeraCountModeSawtoothWave;
    stcTimeraInit.enCntDir = TimeraCountDirUp;
    stcTimeraInit.enSyncStartupEn = Disable;
    stcTimeraInit.u16PeriodVal = (uint16_t)(arr - 1ul);

    TIMERA_BaseInit(M4_TMRA1, &stcTimeraInit);

    TIMERA_IrqCmd(M4_TMRA1, TimeraIrqOverflow, Enable);

    /* Configure interrupt of timera unit 1 */

    IrqRegistration(INT_TMRA1_OVF, Int000_IRQn);

    NVIC_ClearPendingIRQ(Int000_IRQn);
    NVIC_SetPriority(Int000_IRQn, 2);
    NVIC_EnableIRQ(Int000_IRQn);

    /* Timera unit 1 startup */
    TIMERA_Cmd(M4_TMRA1, Disable);
}


void IRQ001_Handler(void)
{
    if (Set == TIMERA_GetFlag(M4_TMRA2, TimeraFlagOverflow))
    {
        TIMERA_ClearFlag(M4_TMRA2, TimeraFlagOverflow);
        Tx_Pin_Tog();
    }
}

//����
void Timera2_Config(uint16_t arr)
{
    stc_timera_base_init_t stcTimeraInit;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcTimeraInit);

    /* Configuration peripheral clock */
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA2, Enable);

    /* Configuration timera  base structure */
    stcTimeraInit.enClkDiv = TimeraPclkDiv2;  //Pclk1 - 84M
    stcTimeraInit.enCntMode = TimeraCountModeSawtoothWave;
    stcTimeraInit.enCntDir = TimeraCountDirUp;
    stcTimeraInit.enSyncStartupEn = Disable;
    stcTimeraInit.u16PeriodVal = (uint16_t)(arr - 1ul);

    TIMERA_BaseInit(M4_TMRA2, &stcTimeraInit);

    TIMERA_IrqCmd(M4_TMRA2, TimeraIrqOverflow, Enable);

    /* Configure interrupt of timera unit 1 */

    IrqRegistration(INT_TMRA2_OVF, Int001_IRQn);

    NVIC_ClearPendingIRQ(Int001_IRQn);
    NVIC_SetPriority(Int001_IRQn, 3);
    NVIC_EnableIRQ(Int001_IRQn);

    /* Timera unit 1 startup */
    TIMERA_Cmd(M4_TMRA2, Disable);
}

void IRQ002_Handler(void)
{
    if (Set == TIMERA_GetFlag(M4_TMRA3, TimeraFlagOverflow))
    {
        TIMERA_ClearFlag(M4_TMRA3, TimeraFlagOverflow);
        Ir_Rcv_Handle();
    }
}

void Timera3_Config(uint16_t arr)
{
    stc_timera_base_init_t stcTimeraInit;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcTimeraInit);

    /* Configuration peripheral clock */
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA3, Enable);

    /* Configuration timera  base structure */
    stcTimeraInit.enClkDiv = TimeraPclkDiv2;  //Pclk1 - 84M
    stcTimeraInit.enCntMode = TimeraCountModeSawtoothWave;
    stcTimeraInit.enCntDir = TimeraCountDirUp;
    stcTimeraInit.enSyncStartupEn = Disable;
    stcTimeraInit.u16PeriodVal = (uint16_t)(arr - 1ul);

    TIMERA_BaseInit(M4_TMRA3, &stcTimeraInit);

    TIMERA_IrqCmd(M4_TMRA3, TimeraIrqOverflow, Enable);

    /* Configure interrupt of timera unit 1 */

    IrqRegistration(INT_TMRA3_OVF, Int002_IRQn);

    NVIC_ClearPendingIRQ(Int002_IRQn);
    NVIC_SetPriority(Int002_IRQn, 1);
    NVIC_EnableIRQ(Int002_IRQn);

    /* Timera unit 1 startup */
    TIMERA_Cmd(M4_TMRA3, Disable);
}

void IRQ003_Handler(void)
{
    if(EXINT_IrqFlgGet(ExtiCh11) == 1)
    {
        EXINT_IrqFlgClr(ExtiCh11);
        Ir_Rcv_Edge_Handle();
    }
}

#endif

void IRRcvGpioInit(void)
{
    #if defined(AT32F4xx)
    GPIO_InitType GPIO_InitStructure;
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    GPIO_InitStructure.GPIO_Pins  = GPIO_Pins_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);          ///<Enable AFIO clock
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinsSource0);  ///<Connect EXTI0 Line to PB9 pin

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineEnable = ENABLE;
    EXTI_Init(&EXTI_InitStructure);                               ///<Configure EXTI9 line

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;
    stc_exint_config_t stcExtiConfig;

    MEM_ZERO_STRUCT(stcPortInit);
    MEM_ZERO_STRUCT(stcExtiConfig);

    stcExtiConfig.enExitCh = ExtiCh11;
    stcExtiConfig.enFilterEn = Enable;
    //stcExtiConfig.enFltClk = Pclk3Div8;
    stcExtiConfig.enExtiLvl = ExIntFallingEdge;

    EXINT_Init(&stcExtiConfig);

    stcPortInit.enPinMode = Pin_Mode_In;  //Pin_Mode_Out
    //stcPortInit.enPinOType = Pin_OType_Cmos;
    //stcPortInit.enPinDrv = Pin_Drv_H;
    stcPortInit.enExInt = Enable;
    stcPortInit.enPullUp = Enable;

    PORT_Init(PortA, Pin11, &stcPortInit);

    IrqRegistration(INT_PORT_EIRQ11, Int003_IRQn);

    NVIC_ClearPendingIRQ(Int003_IRQn);

    NVIC_SetPriority(Int003_IRQn, 0);

    NVIC_EnableIRQ(Int003_IRQn);

    #endif



}

void RX_Pin_High(void)
{
    #if defined(AT32F4xx)
    GPIO_SetBits(GPIOB, GPIO_Pins_0);
    #elif defined(HC32F46x)
    PORT_SetBits(PortA, Pin11);
    #endif
}

uint32_t RX_Pin_IsLow(void)
{
    #if defined(AT32F4xx)
    uint8_t Ret;
    Ret = GPIO_ReadInputDataBit(GPIOB, GPIO_Pins_0);

    return Ret == Bit_RESET;

    #elif defined(HC32F46x)
    en_flag_status_t Ret;
    Ret = PORT_GetBit(PortA, Pin11);

    return Ret == Reset;
    #endif
}

uint32_t RX_Pin_IsHigh(void)
{
    #if defined(AT32F4xx)
    uint8_t Ret;
    Ret = GPIO_ReadInputDataBit(GPIOB, GPIO_Pins_0);

    return Ret == Bit_SET;

    #elif defined(HC32F46x)
    en_flag_status_t Ret;
    Ret = PORT_GetBit(PortA, Pin11);

    return Ret == Set;
    #endif
}


void IR_TX_Init(void)
{
    #if defined(AT32F4xx)
    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PC15 �������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Out;
    PORT_Init(PortA, Pin12, &stcPortInit);

    #endif

}
void Tx_Pin_Low(void)
{
    #if defined(AT32F4xx)
    GPIO_ResetBits(GPIOA, GPIO_Pins_7);
    #elif defined(HC32F46x)
    PORT_ResetBits(PortA, Pin12);
    #endif

}

void Tx_Pin_Tog(void)
{
    #if defined(AT32F4xx)
    GPIOA->OPTDT ^= GPIO_Pins_7;
    #elif defined(HC32F46x)
    PORT_Toggle(PortA, Pin12);
    #endif
}

void Carwave_Timer_Init(void)
{
    //����Ϊ(2*37.9)kHz�ж�һ��
    #if defined(AT32F4xx)
    TIM5_Int_Init(12, 71); //72M/72/12
    #elif defined(HC32F46x)
    Timera2_Config(554);   //84M/2 /554
    #endif
}

void Carwave_Timer_Open(void)
{

    #if defined(AT32F4xx)
    TMR_Cmd(TMR5, ENABLE);
    #elif defined(HC32F46x)
    TIMERA_Cmd(M4_TMRA2, Enable);
    #endif

}



void Carwave_Timer_Close(void)
{
    #if defined(AT32F4xx)
    TMR_ClearITPendingBit(TMR5, TMR_INT_Overflow);
    TMR_Cmd(TMR5, DISABLE);
    #elif defined(HC32F46x)
    TIMERA_ClearFlag(M4_TMRA2, TimeraFlagOverflow);
    TIMERA_Cmd(M4_TMRA2, Disable);
    #endif

}



void Handle_Timer_Init(void)
{
    //����Ϊ833us (1s /1200)�ж�һ��
    #if defined(AT32F4xx)
    TIM2_Int_Init(820, 71);
    #elif defined(HC32F46x)
    Timera1_Config(34440);// 42*820���Ե�ƫС�㣬ָ���Ҳ��Ҫʱ��
    #endif

}

void Handle_Timer_Open(void)
{
    #if defined(AT32F4xx)
    TMR_Cmd(TMR2, ENABLE);
    #elif defined(HC32F46x)
    TIMERA_Cmd(M4_TMRA1, Enable);
    #endif

}

void Handle_Timer_Close(void)
{
    #if defined(AT32F4xx)
    TMR_ClearITPendingBit(TMR2, TMR_INT_Overflow);
    TMR_Cmd(TMR2, DISABLE);
    #elif defined(HC32F46x)
    TIMERA_ClearFlag(M4_TMRA1, TimeraFlagOverflow);
    TIMERA_Cmd(M4_TMRA1, Disable);
    #endif

}

void Handle_Timer_Zero(void)
{
    #if defined(AT32F4xx)
    TMR_SetCounter(TMR2, 0);
    #elif defined(HC32F46x)
    TIMERA_SetCurrCount(M4_TMRA1, 0);
    #endif

}

void Cap_Timer_Init(void)
{
    #if defined(AT32F4xx)
    TMR_Cmd(TMR3, DISABLE);  //ʹ��TIMx
    TIM3_Int_Init(810, 71);
    #elif defined(HC32F46x)
    Timera3_Config(34020);// 42*810���Ե�ƫС�㣬ȡ��ʱ��
    #endif

}

void Cap_Timer_Open(void)
{
    #if defined(AT32F4xx)
    TMR_Cmd(TMR3, ENABLE);
    #elif defined(HC32F46x)
    TIMERA_Cmd(M4_TMRA3, Enable);
    #endif

}

void Cap_Timer_Close(void)
{
    #if defined(AT32F4xx)
    TMR_ClearITPendingBit(TMR3, TMR_INT_Overflow);
    TMR_Cmd(TMR3, DISABLE);
    #elif defined(HC32F46x)
    TIMERA_ClearFlag(M4_TMRA3, TimeraFlagOverflow);
    TIMERA_Cmd(M4_TMRA3, Disable);
    #endif

}

void Cap_Timer_Zero(void)
{
    #if defined(AT32F4xx)
    TMR_SetCounter(TMR3, 0);
    #elif defined(HC32F46x)
    TIMERA_SetCurrCount(M4_TMRA3, 0);
    #endif

}

void Send_High(void)
{
    if(ir_tx_msg.timer_flag == 1)
    {
        Carwave_Timer_Close();
        Tx_Pin_Low();
    }

    ir_tx_msg.timer_flag = 0;
}

void Send_Low(void)
{
    if(ir_tx_msg.timer_flag == 0)
    {
        Carwave_Timer_Open();
    }

    ir_tx_msg.timer_flag = 1;
}


void Start_signal(void)
{
    Send_Low();
}

void End_signal(void)
{
    Send_High();
}

void User_Tx_Init(void)
{
    IR_TX_Init();
    Tx_Pin_Low();
    Carwave_Timer_Init();
    Handle_Timer_Init();

    ir_tx_msg.send_data = 0;
    ir_tx_msg.send_time_count = 0;
    ir_tx_msg.timer_flag = 0;
    ir_tx_msg.tx_count = 0;
    ir_tx_msg.tx_data = 0;
    ir_tx_msg.TX_Mode = 0;
    ir_tx_msg.us_count = 0;
}

void User_Rcv_Init(void)
{
    IRRcvGpioInit();
    RX_Pin_High();
    Cap_Timer_Init();
    //Cap_StartTimer_Init();

    memset(ir_rcv_msg.Ir_Rcv_Buf, 0, Ir_Max_Rcv);
    ir_rcv_msg.Ir_Rcv_Cnt = 0;
    ir_rcv_msg.Last_Rcv_Cnt = 0;
    ir_rcv_msg.LastCnt = 0;
    ir_rcv_msg.Lastrise_Cnt = 0;
    ir_rcv_msg.Rise_Cnt = 0;
    ir_rcv_msg.NowCnt = 0;
    ir_rcv_msg.Off_Cnt = 0;
    ir_rcv_msg.Rcv_data = 0;
    ir_rcv_msg.Rcv_MODE = 1;
    ir_rcv_msg.T_Cnt = 0;
    ir_rcv_msg.Rcv_Str_Flag = 0;
    ir_rcv_msg.Rcv_Bcmp_Flag = 0;
    ir_rcv_msg.Rcv_Cmp_Flag = 0;
}

void User_Ir_Init(void)
{
    User_Tx_Init();
    User_Rcv_Init();
    //Handle_Timer_Init();
}


uint8_t ir_send_byte(uint8_t data)
{
    if(ir_tx_msg.TX_Mode == 0)
    {
        ir_tx_msg.send_data = data;
        ir_tx_msg.TX_Mode = 1;
        Send_Low();
        Handle_Timer_Close();
        Handle_Timer_Zero();
        Handle_Timer_Open();
        return 0;
    }
    else
    {
        return 1;
    }
}

uint8_t User_Ir_Send(uint8_t *send_buf, uint16_t len)
{
    uint16_t j;

    ir_rcv_msg.Rcv_MODE = 0;

    for(j = 0; j < len; j++)
    {
        ir_send_byte(*send_buf);
        send_buf++;

        IWDG_Feed();
        while(ir_tx_msg.TX_Mode);
    }

    ir_rcv_msg.Rcv_MODE = 1;

    return 0;
}

void Ir_Tx_Handle(void)
{
    if(ir_tx_msg.TX_Mode == 1)
    {
        ir_tx_msg.us_count++;

        if(((ir_tx_msg.us_count - ir_tx_msg.send_time_count) == 1))
        {
            ir_tx_msg.send_time_count = ir_tx_msg.us_count;

            if(ir_tx_msg.tx_count < 8)
            {
                ir_tx_msg.tx_data = (ir_tx_msg.send_data & 0x01);

                if(ir_tx_msg.tx_data == 1)
                {
                    Send_High();
                }
                else
                {
                    Send_Low();
                }

                ir_tx_msg.send_data >>= 1;
                ir_tx_msg.tx_count++;
            }
            else if(ir_tx_msg.tx_count == 8)
            {
                Send_High();
                ir_tx_msg.tx_count++;
            }
            else if(ir_tx_msg.tx_count > 8)
            {
                ir_tx_msg.tx_count = 0;
                ir_tx_msg.us_count = 0;
                ir_tx_msg.send_time_count = 0;
                ir_tx_msg.TX_Mode = 0;
                Handle_Timer_Close();
                Handle_Timer_Zero();
                Tx_Pin_Low();
            }
        }
    }
}

void Ir_Rcv_Edge_Handle(void)
{
    if((ir_rcv_msg.Rcv_MODE == 1) && (ir_tx_msg.TX_Mode == 0))
    {
        if(ir_rcv_msg.Rcv_Str_Flag == 0)
        {
            ir_rcv_msg.Rcv_Bcmp_Flag = 0;
            ir_rcv_msg.Rcv_Str_Flag = 1;

            Cap_Timer_Close();
            Cap_Timer_Zero();
            us_num = 0;
            Cap_Timer_Open();
        }
    }
}

void Ir_Rcv_Handle(void)
{
    if((ir_rcv_msg.Rcv_MODE == 1) && (ir_tx_msg.TX_Mode == 0))
    {
        //��ʼλ���
        if((ir_rcv_msg.Rcv_Str_Flag == 1) && (ir_rcv_msg.Rcv_Bcmp_Flag == 0) && RX_Pin_IsLow())
        {
            ir_rcv_msg.Rcv_Bcmp_Flag = 1;
            ir_rcv_msg.Rcv_Cmp_Flag = 0;
            ir_rcv_msg.NowCnt = 0;
            us_num = 0;
            ir_rx_buf = 0;

        }
        //����λ
        else if(ir_rcv_msg.Rcv_Bcmp_Flag == 1)
        {
            if(RX_Pin_IsHigh())
            {
                ir_rx_buf |= (0x01 << us_num);
                //us_num++;
            }

            //ir_rx_buf>>=1;

            us_num++;

            if(us_num >= 8)
            {
                #if 0

                if(ir_rcv_msg.Ir_Rcv_Cnt < Ir_Max_Rcv)
                {
                    ir_rcv_msg.Ir_Rcv_Cnt++;
                }

                ir_rcv_msg.Rise_Cnt = 0;
                ir_rcv_msg.Lastrise_Cnt = 0;
                ir_rcv_msg.Rcv_Str_Flag = 0;
                ir_rcv_msg.Rcv_Bcmp_Flag = 0;
                #endif
                ir_rcv_msg.Rcv_data = ir_rx_buf;
                ir_rcv_msg.Ir_Rcv_Buf[ir_rcv_msg.Ir_Rcv_Cnt++] = ir_rcv_msg.Rcv_data;
                //���ݽ�����ϣ����¼����ʼλ
                ir_rcv_msg.Rcv_Str_Flag = 0;
                ir_rcv_msg.Rcv_Bcmp_Flag = 0;
                ir_rcv_msg.Rcv_Cmp_Flag = 1;
                //us_num = 0;
                Cap_Timer_Close();
                Cap_Timer_Zero();

            }
        }
    }
}


#if 0
void Ir_Data_Handle(void)
{
    uint32_t i;
    uint16_t j;

    for(j = 0; j < ir_rcv_msg.Ir_Rcv_Cnt; j++)
    {
        for(i = ir_rx_cnt * 8; i < (8 + ir_rx_cnt * 8); i++)
        {
            if(ir_rx_buf[i] == 0)
            {
                ir_rcv_msg.Rcv_data &= 0x7F;
            }
            else
            {
                ir_rcv_msg.Rcv_data |= 0x80;
            }

            if(i != (7 + ir_rx_cnt * 8))
            {
                ir_rcv_msg.Rcv_data >>= 1;
            }
        }

        ir_rx_cnt++;

        ir_rcv_msg.Ir_Rcv_Buf[j] = ir_rcv_msg.Rcv_data;
    }
}
#endif

uint8_t User_Ir_Rcv(uint8_t *rx_buf, uint16_t *rx_len)
{
    #if 0

//	if(ir_rcv_msg.Ir_Rcv_Cnt == rx_len)						//�������գ�����ִ��Ir_Rcv_Timeout����
    if(ir_rcv_msg.Rcv_Cmp_Flag == 1)						//���������գ����Ir_Rcv_Timeout������100ms��ʱ������ִ��
    {
        Ir_Data_Handle();
        memcpy(rx_buf, ir_rcv_msg.Ir_Rcv_Buf, MIN(*rx_len, ir_rcv_msg.Ir_Rcv_Cnt));
        *rx_len = MIN(*rx_len, ir_rcv_msg.Ir_Rcv_Cnt);
        ir_rx_num = 0;
        ir_rx_cnt = 0;
        ir_rcv_msg.Rcv_MODE = 0;
        ir_rcv_msg.Ir_Rcv_Cnt = 0;
        ir_rcv_msg.Rcv_Cmp_Flag = 0;
        return 0;
    }

    if(ir_rcv_msg.Rcv_MODE == 0)
    {
        memset(ir_rcv_msg.Ir_Rcv_Buf, 0, Ir_Max_Rcv);
        ir_rcv_msg.Rcv_MODE = 1;
    }

    #endif

    if(ir_rcv_msg.Rcv_TOut_Flag == 1)
    {
        memcpy(rx_buf, ir_rcv_msg.Ir_Rcv_Buf, MIN(*rx_len, ir_rcv_msg.Ir_Rcv_Cnt));
        *rx_len = MIN(*rx_len, ir_rcv_msg.Ir_Rcv_Cnt);
        ir_rcv_msg.Rcv_TOut_Flag = 0;
        ir_rcv_msg.Ir_Rcv_Cnt = 0;
        return 0;
    }

    return 1;

}

uint8_t IsIR38SendBusy(void)
{
    return (ir_tx_msg.TX_Mode == 1 ? TRUE : FALSE);
}





/****************************************************************
@FUNCTION������̾෢��ģ��ײ㶨ʱ���Ƚ���(1ms)
@INPUT��
@OUTPUT����
@RETURN: TRUE-����ִ�еײ㶨ʱ����,  FALSE-�޵ײ㶨ʱ����, ��ִ���ϲ�Ӧ�ö�ʱ����
@AUTHOR��xfw
@SPECIAL: ʵ�ֽ��ճ�ʱ�ж�
****************************************************************/
extern uint32_t Ir_LowLevelTimedProcess(void)
{
    //���յ�����
    if(ir_rcv_msg.Rcv_Cmp_Flag == 1)
    {
        ir_rcv_msg.NowCnt++;

        if(ir_rcv_msg.NowCnt >= 7)
        {
            ir_rcv_msg.Rcv_TOut_Flag = 1;
        }
    }

    return 0;
}



