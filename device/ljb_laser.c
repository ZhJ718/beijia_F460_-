/******************************************************************
*版权所有 (C)2020, 江苏华如防务科技有限公司
*
*文件名称:  ljb_laser.c
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者:
*完成日期:  2020/3/28
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

#include "ljb_laser.h"
#include "protocal.h"


LJB_LASER_RECV_MAG g_sLaserRecvMag[IR_MAX];

/**
 *******************************************************************************
 ** \brief GPIO PCA 中断处理函数
 **
 ** \retval
 *****************************************************************************
void PCA_IRQHandler(void)
{
	//if (TRUE == Pca_GetIntFlag(Module0))
    if(M0P_PCA->CCON_f.CCF0==1)
    {
       	g_sLaserRecvMag.m_IsRecvStart = TRUE;
		g_sLaserRecvMag.m_byRecvTimeoutMs = 0;
		//获取捕获值
		g_sLaserRecvMag.m_wNowCnt =  M0P_PCA->CCAP0_f.CCAP0;//Pca_CapData16Get(Module0);
		g_sLaserRecvMag.m_IsRecvStep = 1;


		M0P_PCA->ICLR_f.CCF0 = FALSE;
		//Pca_ClearIntFlag(Module0);
    }
}

*/


#if 0
void PcaInt(void)
{

    #if 0

    if (TRUE == Pca_GetCntIntFlag())
    {
        Pca_ClearCntIntFlag();
        g_IsRecvTimeout = 1;
    }

    #endif


    if (TRUE == Pca_GetIntFlag(Module0))
        //if(M0P_PCA->CCON_f.CCF0==1)
    {
        g_sLaserRecvMag.m_IsRecvStart = TRUE;
        g_sLaserRecvMag.m_byRecvTimeoutMs = 0;
        //获取捕获值
        g_sLaserRecvMag.m_wNowCnt =  M0P_PCA->CCAP0_f.CCAP0;//Pca_CapData16Get(Module0);
        g_sLaserRecvMag.m_IsRecvStep = 1;


        //M0P_PCA->ICLR_f.CCF0 = FALSE;
        Pca_ClearIntFlag(Module0);
    }

}

#endif




#if 0

void Adt4CaptureBCalllback(void)
{
    stc_adt_cntstate_cfg_t stcAdt4CntState = {0};
    uint16_t u16Capture;

    Adt_GetCntState(AdTIM4, &stcAdt4CntState);

    if(stcAdt4CntState.bCMBF == 1)
    {
        Adt_GetCaptureValue(AdTIM4, AdtCHxB, &u16Capture);
        g_sLaserRecvMag.m_IsRecvStart = TRUE;
        g_sLaserRecvMag.m_byRecvTimeoutMs = 0;
        //获取捕获值
        g_sLaserRecvMag.m_wNowCnt =  u16Capture;
        g_sLaserRecvMag.m_IsRecvStep = 1;
    }


}

/**
 *******************************************************************************
 ** \brief GPIO 高级时钟4 中断处理函数
 **
 ** \retval
 ******************************************************************************/
void TIM4_IRQHandler(void)
{

    if (TRUE == M0P_ADT0->IFR_f.CMBF)
    {
        g_sLaserRecvMag.m_IsRecvStart = TRUE;
        g_sLaserRecvMag.m_byRecvTimeoutMs = 0;
        //获取捕获值
        g_sLaserRecvMag.m_wNowCnt =  M0P_ADT0->GCMBR_f.GCMB;
        g_sLaserRecvMag.m_IsRecvStep = 1;

        M0P_ADT0->ICLR = ~(1 << 1);
    }
}

void AdtTim4CapInit(void)
{
    en_adt_unit_t enAdt;
    stc_adt_basecnt_cfg_t stcAdtBaseCntCfg;
    en_adt_compare_t enAdtCompareA;
    stc_adt_CHxX_port_cfg_t stcAdtTIM4BCfg;

    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);


    Clk_SetPeripheralGate(ClkPeripheralAdt, TRUE);

    Gpio_SetFunc_TIM4_CHB_P24();

    enAdt = AdTIM4;
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div8;
    Adt_Init(enAdt, &stcAdtBaseCntCfg);





    stcAdtTIM4BCfg.enCap = AdtCHxCompareInput;
    stcAdtTIM4BCfg.bOutEn = FALSE;
    stcAdtTIM4BCfg.enPerc = AdtCHxPeriodLow;
    stcAdtTIM4BCfg.enCmpc = AdtCHxCompareHigh;
    stcAdtTIM4BCfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4BCfg.enStaOut = AdtCHxPortOutLow;

    Adt_CHxXPortConfig(enAdt, AdtCHxB, &stcAdtTIM4BCfg);     //端口CHB配置，捕获输入功能

    Adt_ConfigHwCaptureB(enAdt, AdtHwTrigTimxBFall);         //硬件捕获B条件配置

    Adt_ConfigIrq(enAdt, AdtCMBIrq, TRUE, Adt4CaptureBCalllback); //捕获中断B配置

    Adt_StartCount(enAdt);
}
#endif


void GetEncode(LJB_LASER_RECV_MAG *pMag)
{
    uint8_t TSeqTemp[6];
    uint8_t i, j;

    //从环形缓冲中取出未处理的6个时序
    TSeqTemp[0] = pMag->m_sTSeqMag.m_aSeq[pMag->m_sTSeqMag.m_wPopOff % TIMING_SEQUENCE_MAX];
    TSeqTemp[1] = pMag->m_sTSeqMag.m_aSeq[(pMag->m_sTSeqMag.m_wPopOff + 1) % TIMING_SEQUENCE_MAX];
    TSeqTemp[2] = pMag->m_sTSeqMag.m_aSeq[(pMag->m_sTSeqMag.m_wPopOff + 2) % TIMING_SEQUENCE_MAX];
    TSeqTemp[3] = pMag->m_sTSeqMag.m_aSeq[(pMag->m_sTSeqMag.m_wPopOff + 3) % TIMING_SEQUENCE_MAX];
    TSeqTemp[4] = pMag->m_sTSeqMag.m_aSeq[(pMag->m_sTSeqMag.m_wPopOff + 4) % TIMING_SEQUENCE_MAX];
    TSeqTemp[5] = pMag->m_sTSeqMag.m_aSeq[(pMag->m_sTSeqMag.m_wPopOff + 5) % TIMING_SEQUENCE_MAX];
    //比对该序列

    for(i = 0, j = 0; i < ENCODE_MAX; i++)
    {
        if(TSeqTemp[0] == gs_LaserEncode[i].m_aTSeq[0])
        {
            for(j = 1; j < 6;)
            {
                if(TSeqTemp[j] == gs_LaserEncode[i].m_aTSeq[j])
                {
                    j++;
                }
                else
                {
                    break;
                }
            }
        }

        if(j >= 6)
        {
            break;
        }

    }

    //找到序列
    if(j == 6)
    {
        RING_OFFSET_ADD(pMag->m_sTSeqMag.m_wPopOff, 6, TIMING_SEQUENCE_MAX);
        //取出编码
        pMag->m_byRecvEncode[pMag->m_wRecvEncodeCnt] = gs_LaserEncodeIdx[i];
        RING_OFFSET_ADD(pMag->m_wRecvEncodeCnt, 1, RECV_ENCODE_MAX);

    }
    else
    {
        RING_OFFSET_ADD(pMag->m_sTSeqMag.m_wPopOff, 1, TIMING_SEQUENCE_MAX);
    }
}


typedef struct
{

    uint8_t m_byStartPos;
    uint8_t m_byEndPos;

    uint8_t m_byChkSum;
} LASER_CHILD_SEQ_MAG;

#define CHILD_SEQ_REPEAT_MAX 3 //最多重复三遍


int32_t Decode_ABCABCABC(LJB_LASER_RECV_MAG *pMag)
{
    uint16_t i, j;
    uint8_t ChildSeqCnt = 0;
    uint8_t MaxRptCnt = 0, MaxRptIdx = 0, RptCntTemp = 0;
    LASER_CHILD_SEQ_MAG sCSeqMag[CHILD_SEQ_REPEAT_MAX];

    //遍历数据序列，根据帧头帧尾分解成各个子串，并计算子串累加和，以累加和作为key，子串为value，创建Hash表
    for(i = 0, ChildSeqCnt = 0; (i < pMag->m_wRecvEncodeCnt) && (ChildSeqCnt < CHILD_SEQ_REPEAT_MAX); i++)
    {
        if(pMag->m_byRecvEncode[i] == ENCODE_HEAD)
        {
            sCSeqMag[ChildSeqCnt].m_byStartPos = i;
            sCSeqMag[ChildSeqCnt].m_byChkSum = 0;
        }
        else if(pMag->m_byRecvEncode[i] == ENCODE_TAIL)
        {
            sCSeqMag[ChildSeqCnt].m_byEndPos = i;
            ChildSeqCnt++;
        }
        else
        {
            sCSeqMag[ChildSeqCnt].m_byChkSum += pMag->m_byRecvEncode[i];
        }
    }

    //printf("Find %d Child Seq\n\r",ChildSeqCnt);
    //遍历Hash表，查找重复子串并返回重复次数(不考虑多种重复字串)
    if(ChildSeqCnt >= 2)
    {
        MaxRptCnt = 1;

        MaxRptIdx = 0;

        for(i = 0; i < ChildSeqCnt; i++)
        {
            RptCntTemp = 1;

            for(j = i + 1; j < ChildSeqCnt; j++)
            {
                if(sCSeqMag[i].m_byChkSum == sCSeqMag[j].m_byChkSum)
                {

                    RptCntTemp += 1;

                    if(RptCntTemp > MaxRptCnt)
                    {
                        MaxRptCnt = RptCntTemp;
                        MaxRptIdx = i;
                        //printf("MaxRptCnt Update,i=%d,j=%d Seq=0x%02X,MaxRptCnt=%d\n\r",i,j,sCSeqMag[i].m_byChkSum,MaxRptCnt);
                    }
                }
            }
        }

    }

    //若重复次数大于1，则解码
    if(MaxRptCnt >= 2)
    {
        //首先校验下接收编码长度是否为偶数(不包括0)
        i = sCSeqMag[MaxRptIdx].m_byEndPos - sCSeqMag[MaxRptIdx].m_byStartPos - 1;

        if(i == 0 || i % 2 == 1)
        {
            return -1;
        }


        //解码
        pMag->m_wRecvBufLen = 0;

        for(i = sCSeqMag[MaxRptIdx].m_byStartPos + 1; i < sCSeqMag[MaxRptIdx].m_byEndPos; i += 2)
        {
            if(pMag->m_byRecvEncode[i] <= ENCODE_HIGH_1111 && pMag->m_byRecvEncode[i + 1] >= ENCODE_LOW_0000)
            {
                //高4位在前，低4位在后
                pMag->m_byRecvBuf[pMag->m_wRecvBufLen] = pMag->m_byRecvEncode[i] << 4;
                pMag->m_byRecvBuf[pMag->m_wRecvBufLen] |= pMag->m_byRecvEncode[i + 1] - ENCODE_LOW_0000;
                pMag->m_wRecvBufLen += 1;
            }
            else
            {

                pMag->m_IsRecvNew = FALSE;
                return -1;
            }
        }

        pMag->m_IsRecvNew = TRUE;
        pMag->m_byRepeatCnt = MaxRptCnt;

    }

    //printf("MaxRptCnt=%d,MaxRptIdx=%d\n\r",MaxRptCnt,MaxRptIdx);
    //清除接收编码，等待新的接收
    pMag->m_wRecvEncodeCnt = 0;

    return 0;
}

int32_t Decode_AAABBBCCC(LJB_LASER_RECV_MAG *pMag)
{
    uint16_t i, j;
    uint8_t HeadOff, TailOff;

    //过滤掉重复两次或三次的编码
    for(i = 0, j = 0; i < pMag->m_wRecvEncodeCnt;)
    {
        if(pMag->m_byRecvEncode[i] == pMag->m_byRecvEncode[i + 1])
        {
            pMag->m_byRecvEncode[j++] = pMag->m_byRecvEncode[i];

            i += ((pMag->m_byRecvEncode[i + 1] == pMag->m_byRecvEncode[i + 2]) ? 2 : 1);
        }

        i += 1;
    }

    pMag->m_wRecvEncodeCnt = j;

    //找到帧头帧尾
    HeadOff = TailOff = 0;

    for(i = 0; i < pMag->m_wRecvEncodeCnt; i++)
    {
        if(pMag->m_byRecvEncode[i] == ENCODE_HEAD)
        {
            HeadOff = i;
        }

        if(pMag->m_byRecvEncode[i] == ENCODE_TAIL)
        {
            TailOff = i;
        }
    }

    //若帧头帧尾都存在
    if(HeadOff != TailOff)
    {
        //解码
        pMag->m_wRecvBufLen = 0;

        for(i = HeadOff + 1; i < TailOff; i += 2)
        {
            //如果数据完整
            if(pMag->m_byRecvEncode[i] <= ENCODE_HIGH_1111 && pMag->m_byRecvEncode[i + 1] >= ENCODE_LOW_0000)
            {
                //高4位在前，低4位在后
                pMag->m_byRecvBuf[pMag->m_wRecvBufLen] = pMag->m_byRecvEncode[i] << 4;
                pMag->m_byRecvBuf[pMag->m_wRecvBufLen] |= pMag->m_byRecvEncode[i + 1] - ENCODE_LOW_0000;
                pMag->m_wRecvBufLen += 1;
            }
            //如果数据不完整，统一填充成0xFF
            else if(pMag->m_byRecvEncode[i] <= ENCODE_HIGH_1111 || pMag->m_byRecvEncode[i + 1] >= ENCODE_LOW_0000)
            {
                pMag->m_byRecvBuf[pMag->m_wRecvBufLen] = 0xFF;
                pMag->m_wRecvBufLen += 1;
            }
        }

        if(pMag->m_wRecvBufLen == 0)
        {
            pMag->m_IsRecvNew = FALSE;
        }
        //满足协议校验，编码重复次数填充3，上传接收数据去掉校验字节
        else if(pMag->m_byRecvBuf[pMag->m_wRecvBufLen - 1] == Crc8_Calc(0x07, pMag->m_byRecvBuf, pMag->m_wRecvBufLen - 1))
        {
            pMag->m_IsRecvNew = TRUE;
            pMag->m_byRepeatCnt = 3;
            //去掉校验码
            pMag->m_wRecvBufLen -= 1;
        }
        //不满足协议校验，编码重复次数填充1，上传接收数据
        else
        {
            pMag->m_IsRecvNew = TRUE;
            pMag->m_byRepeatCnt = 1;
            //不去掉校验码
        }

    }

    //清除接收编码，等待新的接收
    pMag->m_wRecvEncodeCnt = 0;

    return 0;
}



void LaserRecvTimedProcess(LJB_LASER_RECV_MAG *pLjbMag)
{
    volatile LJB_LASER_RECV_MAG *pMag = pLjbMag;

    //接收超时判断
    if(pMag->m_IsRecvStart == TRUE)
    {
        pMag->m_byRecvTimeoutMs++;

        if(pMag->m_byRecvTimeoutMs > 2)
        {
            pMag->m_IsRecv = TRUE;

        }
    }
}

void LaserRecvStepProcess(LJB_LASER_RECV_MAG *pLjbMag)
{
    uint16_t Off;
    volatile LJB_LASER_RECV_MAG *pMag = pLjbMag;

    if(pMag->m_IsRecvStep == 1)
    {
        pMag->m_IsRecvStep = 0;

        if(pMag->m_wNowCnt < pMag->m_wLastCnt)
        {
            Off = pMag->m_wNowCnt + 0x10000 - pMag->m_wLastCnt;
        }
        else
        {
            Off = pMag->m_wNowCnt - pMag->m_wLastCnt;
        }

        //整形
        if(Off > TSEQ_1T_MIN && Off < TSEQ_1T_MAX)
        {
            //存入时序环形缓冲区
            pMag->m_sTSeqMag.m_aSeq[pMag->m_sTSeqMag.m_wPushOff] = 1;
        }
        else if(Off > TSEQ_2T_MIN && Off < TSEQ_2T_MAX)
        {
            //存入时序环形缓冲区
            pMag->m_sTSeqMag.m_aSeq[pMag->m_sTSeqMag.m_wPushOff] = 2;
        }
        else if(Off > TSEQ_3T_MIN && Off < TSEQ_3T_MAX)
        {
            //存入时序环形缓冲区
            pMag->m_sTSeqMag.m_aSeq[pMag->m_sTSeqMag.m_wPushOff] = 3;
        }
        else if(Off > TSEQ_4T_MIN && Off < TSEQ_4T_MAX)
        {
            //存入时序环形缓冲区
            pMag->m_sTSeqMag.m_aSeq[pMag->m_sTSeqMag.m_wPushOff] = 4;
        }
        else if(Off > TSEQ_5T_MIN && Off < TSEQ_5T_MAX)
        {
            //存入时序环形缓冲区
            pMag->m_sTSeqMag.m_aSeq[pMag->m_sTSeqMag.m_wPushOff] = 5;
        }
        else
        {
            pMag->m_sTSeqMag.m_aSeq[pMag->m_sTSeqMag.m_wPushOff] = 0xff;
        }

        RING_OFFSET_ADD(pMag->m_sTSeqMag.m_wPushOff, 1, TIMING_SEQUENCE_MAX);
        //当未处理时序大于等于6时
        //if(RING_OFF(g_sLaserRecvMag.m_sTSeqMag.m_wPushOff,g_sLaserRecvMag.m_sTSeqMag.m_wPopOff,TIMING_SEQUENCE_MAX)>=(TIMING_SEQUENCE_MAX-1))
        //{
        //	g_sLaserRecvMag.m_IsRecvNew = 1;
        //}
        pMag->m_wLastCnt = pMag->m_wNowCnt;
    }
}

void LaserRecvProcess(LJB_LASER_RECV_MAG *pLjbMag)
{
    volatile LJB_LASER_RECV_MAG *pMag = pLjbMag;

    if(pMag->m_IsRecv == 1)
    {
        //接收时序超时了，若缓冲中未处理时序等于5（最后一个编码的最后一个时序收不到）
        //if(RING_OFF(g_sLaserRecvMag.m_sTSeqMag.m_byPushOff,g_sLaserRecvMag.m_sTSeqMag.m_byPopOff,TIMING_SEQUENCE_MAX)==5)
        {
            //手动加入最后一个时序1T
            pMag->m_sTSeqMag.m_aSeq[pMag->m_sTSeqMag.m_wPushOff] = 1;
            RING_OFFSET_ADD(pMag->m_sTSeqMag.m_wPushOff, 1, TIMING_SEQUENCE_MAX);

            //获得发送编码
            for(; RING_OFF(pMag->m_sTSeqMag.m_wPushOff, pMag->m_sTSeqMag.m_wPopOff, TIMING_SEQUENCE_MAX) >= 6;)
            {
                GetEncode((LJB_LASER_RECV_MAG *)pMag);
            }

            (1 == pMag->m_IsABCABCABC) ? Decode_ABCABCABC((LJB_LASER_RECV_MAG *)pMag) : Decode_AAABBBCCC((LJB_LASER_RECV_MAG *)pMag);
        }
        pMag->m_IsRecvStart = 0;
        pMag->m_IsRecv = 0;
        pMag->m_byRecvTimeoutMs = 0;


    }

    #if 0

    if(pMag->m_IsRecvNew == 1)
    {
        //LED_2_fast_blink_once();
        //LED_2_slow_blink_once();
        //send_laser_msg(pMag->m_byRecvBuf,pMag->m_wRecvBufLen,pMag->m_byRepeatCnt);


        pMag->m_IsRecvNew = 0;
        pMag->m_wRecvBufLen = 0;
        pMag->m_byRepeatCnt = 0;

    }

    #endif

}




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

static void IR_NvicConfig(en_int_src_t enIntSrc, IRQn_Type enIRQn, int8_t IrqPrity)
{
    IrqRegistration(enIntSrc, enIRQn);
    NVIC_SetPriority(enIRQn, IrqPrity);
    NVIC_ClearPendingIRQ(enIRQn);
    NVIC_EnableIRQ(enIRQn);
}


//捕获中断函数
void IRQ001_Handler(void)
{
    volatile uint16_t Dat;
    volatile IR_CFG *pCfg = (IR_CFG *)&gs_IRCfg[IR_F];
    volatile LJB_LASER_RECV_MAG *pLsrRsvMag = &g_sLaserRecvMag[IR_F];

    if (Set == TIMERA_GetFlag(pCfg->m_pTimTypeDef, pCfg->m_eTimFlg))
    {
        TIMERA_ClearFlag(pCfg->m_pTimTypeDef, pCfg->m_eTimFlg);

        Dat = pCfg->m_pTimTypeDef->CNTER_f.CNT; //计数器数值

        pLsrRsvMag->m_IsRecvStart = TRUE;
        pLsrRsvMag->m_byRecvTimeoutMs = 0;
        //获取捕获值
        pLsrRsvMag->m_wNowCnt =  Dat;
        pLsrRsvMag->m_IsRecvStep = 1;
    }
}

void IRQ002_Handler(void)
{
    volatile uint16_t Dat;
    volatile IR_CFG *pCfg = (IR_CFG *)&gs_IRCfg[IR_L];
    volatile LJB_LASER_RECV_MAG *pLsrRsvMag = &g_sLaserRecvMag[IR_L];

    if (Set == TIMERA_GetFlag(pCfg->m_pTimTypeDef, pCfg->m_eTimFlg))
    {
        TIMERA_ClearFlag(pCfg->m_pTimTypeDef, pCfg->m_eTimFlg);

        Dat = pCfg->m_pTimTypeDef->CNTER_f.CNT; //计数器数值

        pLsrRsvMag->m_IsRecvStart = TRUE;
        pLsrRsvMag->m_byRecvTimeoutMs = 0;
        //获取捕获值
        pLsrRsvMag->m_wNowCnt =  Dat;
        pLsrRsvMag->m_IsRecvStep = 1;
    }
}

void IRQ003_Handler(void)
{
    volatile uint16_t Dat;
    volatile IR_CFG *pCfg = (IR_CFG *)&gs_IRCfg[IR_R];
    volatile LJB_LASER_RECV_MAG *pLsrRsvMag = &g_sLaserRecvMag[IR_R];

    if (Set == TIMERA_GetFlag(pCfg->m_pTimTypeDef, pCfg->m_eTimFlg))
    {
        TIMERA_ClearFlag(pCfg->m_pTimTypeDef, pCfg->m_eTimFlg);

        Dat = pCfg->m_pTimTypeDef->CNTER_f.CNT; //计数器数值

        pLsrRsvMag->m_IsRecvStart = TRUE;
        pLsrRsvMag->m_byRecvTimeoutMs = 0;
        //获取捕获值
        pLsrRsvMag->m_wNowCnt =  Dat;
        pLsrRsvMag->m_IsRecvStep = 1;
    }
}

void IRQ004_Handler(void)
{
    volatile uint16_t Dat;
    volatile IR_CFG *pCfg = (IR_CFG *)&gs_IRCfg[IR_B];
    volatile LJB_LASER_RECV_MAG *pLsrRsvMag = &g_sLaserRecvMag[IR_B];

    if (Set == TIMERA_GetFlag(pCfg->m_pTimTypeDef, pCfg->m_eTimFlg))
    {
        TIMERA_ClearFlag(pCfg->m_pTimTypeDef, pCfg->m_eTimFlg);

        Dat = pCfg->m_pTimTypeDef->CNTER_f.CNT; //计数器数值

        pLsrRsvMag->m_IsRecvStart = TRUE;
        pLsrRsvMag->m_byRecvTimeoutMs = 0;
        //获取捕获值
        pLsrRsvMag->m_wNowCnt =  Dat;
        pLsrRsvMag->m_IsRecvStep = 1;
    }
}

#if 0
void IRQ004_Handler(void)
{
    volatile uint16_t Dat;
    volatile IR_CFG *pCfg = (IR_CFG *)&gs_IRCfg[IR_D];
    volatile LJB_LASER_RECV_MAG *pLsrRsvMag = &g_sLaserRecvMag[IR_D];

    if (Set == TIMERA_GetFlag(pCfg->m_pTimTypeDef, pCfg->m_eTimFlg))
    {
        TIMERA_ClearFlag(pCfg->m_pTimTypeDef, pCfg->m_eTimFlg);

        Dat = pCfg->m_pTimTypeDef->CNTER_f.CNT; //计数器数值

        pLsrRsvMag->m_IsRecvStart = TRUE;
        pLsrRsvMag->m_byRecvTimeoutMs = 0;
        //获取捕获值
        pLsrRsvMag->m_wNowCnt =  Dat;
        pLsrRsvMag->m_IsRecvStep = 1;
    }
}
#endif

/**************捕获通道配置*****************/
/*************
IR_U		IRA1		PB5		TIMA_3_PWM2(Func4) 	HL1
IR_L		IRA2		PC14	TIMA_4_PWM5(Func4) 	HL3
IR_M		IRA3		PA11		TIMA_1_PWM4(Func4) 	HL1\HL2\HL3\HL4
IR_R		IRA4		PA1		TIMA_2_PWM2(Func4)	HL2
IR_D		IRA5		PA12		TIMA_6_PWM1(Func5)	HL4
*************/
void IR_Cap_Init(void)
{
    uint8_t i;
    stc_timera_base_init_t stcTimeraInit;
    stc_timera_capture_init_t stcTimeraCaptureInit;
    stc_port_init_t stcPortInit;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcTimeraInit);
    MEM_ZERO_STRUCT(stcTimeraCaptureInit);
    MEM_ZERO_STRUCT(stcPortInit);

    /* Configuration peripheral clock */
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA1, Enable);
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA2, Enable);
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA3, Enable);
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA4, Enable);
    //PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA5, Enable);
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS, Enable);

    /*initiallize port*/
    stcPortInit.enPinMode = Pin_Mode_In;
    stcPortInit.enExInt = Disable;
    stcPortInit.enPullUp = Disable;



    /* Configuration TIMERA capture pin */
    for(i = 0; i < IR_MAX; i++)
    {
        /* Port/Pin initialization */
        PORT_Init(gs_IRCfg[i].m_ePort, gs_IRCfg[i].m_ePin, &stcPortInit);
        PORT_SetFunc(gs_IRCfg[i].m_ePort, gs_IRCfg[i].m_ePin, gs_IRCfg[i].m_ePortFunc, Disable);
    }

    /* Configuration timera  base structure */
    stcTimeraInit.enClkDiv = TimeraPclkDiv32;  //Pclk1 - 84M
    stcTimeraInit.enCntMode = TimeraCountModeSawtoothWave;
    stcTimeraInit.enCntDir = TimeraCountDirUp;
    stcTimeraInit.enSyncStartupEn = Disable;
    stcTimeraInit.u16PeriodVal = 0xFFFFu;

    for(i = 0; i < IR_MAX; i++)
    {
        TIMERA_BaseInit(gs_IRCfg[i].m_pTimTypeDef, &stcTimeraInit);
    }


    /* Configuration timera  capture structure */
    stcTimeraCaptureInit.enCapturePwmRisingEn = Disable; 	//禁止上升沿触发捕获
    stcTimeraCaptureInit.enCapturePwmFallingEn = Enable;  //下降沿触发捕获
    stcTimeraCaptureInit.enCaptureSpecifyEventEn = Disable;

    /* Enable  capture and interrupt */
    //i = 0;
    for(i = 0; i < IR_MAX; i++)
    {
        TIMERA_CaptureInit(gs_IRCfg[i].m_pTimTypeDef, gs_IRCfg[i].m_eTimCh, &stcTimeraCaptureInit); //配置TIMA单元 通道
        TIMERA_IrqCmd(gs_IRCfg[i].m_pTimTypeDef, gs_IRCfg[i].m_eTimIrq, Enable);
        IR_NvicConfig(gs_IRCfg[i].m_enIntSrc, gs_IRCfg[i].m_enIRQn, gs_IRCfg[i].m_byIrqPrity);
    }

    /* Timera  startup */
    for(i = 0; i < IR_MAX; i++)
    {
        TIMERA_Cmd(gs_IRCfg[i].m_pTimTypeDef, Enable);
    }
}



