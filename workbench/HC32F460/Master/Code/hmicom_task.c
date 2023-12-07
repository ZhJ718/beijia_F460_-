/********************************************************************************
* @File name: hmicom_task.c
* @Author: xfw
* @Version: 1.0
* @Date: 2023-4-4
* @Description:
********************************************************************************/

#include "hmicom_task.h"
#include "battery_task.h"
#include "rescom_task.h"
#include "zigcom_task.h"

/*************底层实现************************************************/
//串口初始化
void HmiCom_ComInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[HMI_COM];

    memcpy((void *)&Config, (void *)&g_scUartDefConfig[HMI_COM], sizeof(UART_CONFIG));
    Config.m_dwBaudRate = HMI_COM_BAUDRATE;
    //Config.TxMode = MODE_INTERRUPT;
    //Config.RxMode = MODE_INTERRUPT;
    //对应串口配置
    Uart_ComConfiguration(Config, pUartMag);
    Uart_ComInit(pUartMag);

}

//串口写数据
uint32_t HmiCom_ComWrite(uint8_t *pBuf, uint16_t Len)
{
    #ifdef HMI_COM_LOG_OUTPUT

    LOG("HMICOM(%d)->", Len);
    printf_with_data(pBuf, Len);

    #endif

    return Uart_SendBuf(&g_sUartSetMag.Com[HMI_COM], pBuf, Len);
}

//串口读数据
uint32_t HmiCom_ComRead(uint8_t *pBuf, uint16_t *pLen)
{
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[HMI_COM];

    /**/
    if(TRUE == pUartMag->m_bIsComRecv)
    {
        if(pUartMag->m_wRecvLen == 0)
        {
            pUartMag->m_bIsComRecv = FALSE;
            return FALSE;
        }

        #ifdef HMI_COM_LOG_OUTPUT

        LOG("HMICOM(%d)<-", pUartMag->m_wRecvLen);
        printf_with_data(pUartMag->m_byRecvBuf, pUartMag->m_wRecvLen);

        #endif

        if(*pLen >= pUartMag->m_wRecvLen)
        {
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, pUartMag->m_wRecvLen);
            *pLen = pUartMag->m_wRecvLen;
        }
        else
        {
            //缓存区不够,不拷贝多余数据
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, *pLen);
        }

        pUartMag->m_wRecvLen = 0;
        pUartMag->m_bIsComRecv = FALSE;

        return TRUE;
    }

    return FALSE;
}

/*************底层实现结束************************************************/

HMICOMTASK_MAG g_sHmiComTask_Mag;


/****************************************************************
@FUNCTION：获取HMI端口管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
HMICOMTASK_MAG *HmiComTask_HandleGet(void)
{
    return &g_sHmiComTask_Mag;
}

/****************************************************************
@FUNCTION：获取HMI端口任务管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
SCOMTASK_MAG *HmiComTask_TaskHandleGet(void)
{
    return &g_sHmiComTask_Mag.m_sTaskMag;
}


/****************************************************************
@FUNCTION：HMI 串口功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void HmiComTask_Init(void)
{
    HMICOMTASK_MAG *pMag = HmiComTask_HandleGet();

    memset((void *)pMag, 0, sizeof(HMICOMTASK_MAG));

    COM_HW_OPERATIONS HwOps =
    {
        .hw_init = HmiCom_ComInit,
        .read = HmiCom_ComRead,
        .write = HmiCom_ComWrite,
    };

    Hmi268b_LowLevelInit(HwOps);

    pMag->m_pLLMag = Hmi268b_GetLLMagHandle();

    pMag->m_bIsInit = SComTask_Init(&pMag->m_sTaskMag, HMI_COM_FRAME_LEN_MAX, 100);

    SComTask_SetListNodeLen(&pMag->m_sTaskMag, 40);
		
		pMag->m_bIsOnline = FALSE;
		pMag->m_byDispalyOnFlg = TRUE;
		pMag->m_byLastMode = 0xFF;
		pMag->m_byLastBlood = 0xFF;
		pMag->m_byLastIsNight = 0xFF;
		pMag->m_wLastPID = 0xFFFF;
		pMag->m_byLastBelong = 0xFF;
		pMag->m_HurtKind = NULL;
		pMag->m_byHelmetIsOnline = FALSE;
    HmiCom_Prot_CheckOnline(pMag, 1, 1000);
}


/****************************************************************
@FUNCTION：HMI 串口功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void HmiComTask_DeInit(void)
{
    HMICOMTASK_MAG *pMag = HmiComTask_HandleGet();

    SComTask_DeInit(&pMag->m_sTaskMag);

    memset((void *)pMag, 0, sizeof(HMICOMTASK_MAG));
}


/****************************************************************
@FUNCTION：HMI 串口功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void HmiComTask_Process(void)
{
    HMICOMTASK_MAG *pMag = HmiComTask_HandleGet();
    SCOMTASK_MAG *pComTaskMag = &pMag->m_sTaskMag;
    uint16_t i, Len = pComTaskMag->m_wProtBufMaxLen - pComTaskMag->m_wProtBufLen;
    uint32_t IsCutted = FALSE;//, IsLong55AA = FALSE;
    uint32_t NowTime = GetSysRunMilliSecond();
    FRAME_BUF *pFrame = NULL;

    if(FALSE == pMag->m_bIsInit)
    {
        return;
    }

    //定时任务
    if((NowTime >  pComTaskMag->m_dwLastScheduleTime) &&
            (NowTime -  pComTaskMag->m_dwLastScheduleTime >= pComTaskMag->m_dwScheduleTime) )
    {
        pComTaskMag->m_dwLastScheduleTime = NowTime;

        //优先保证底层操作
        if(FALSE == Hmi268b_LowLevelTimedProcess())
        {

            pFrame = (FRAME_BUF *)SComTask_TimedProcess(pComTaskMag);

            if(pFrame != NULL)
            {
								pMag->m_pLLMag->m_sHwOps.write(pFrame->m_byBuf, pFrame->m_wBufLen);
            }

            /***********应用逻辑***************************/
            if(TRUE == pMag->m_bIsOnline)
            {
                HmiCom_TimedTask(pMag);
            }

        }


    }

    //串口接收处理
    if(TRUE == pMag->m_pLLMag->m_sHwOps.read(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], &Len))
    {
			  #ifdef SEND_TO_UPCOM
        //转发数据到上行口
        ResComTask_SendToUpcom(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], Len, 0x13, CTRL_RECV);
        pComTaskMag->m_wProtBufLen = 0;
        #endif

//        #if 0
        pComTaskMag->m_wProtBufLen += Len;
		
        for(i = 0; i < pComTaskMag->m_wProtBufLen;)
        {
            Len = pComTaskMag->m_wProtBufLen - i;

            if(TRUE == Protocal_FindHmi268bFrame(&pComTaskMag->m_pProtBuf[i], &Len, &IsCutted))
            {
                pComTaskMag->m_dwLastCommTime = NowTime;

                if(TRUE == IsCutted)
                {
                    memcpy(pComTaskMag->m_pProtBuf, &pComTaskMag->m_pProtBuf[i], pComTaskMag->m_wProtBufLen - i);
                    pComTaskMag->m_wProtBufLen -= i ;
                    break;
                }
                else
                {
                    HmiCom_Analysis(&pComTaskMag->m_pProtBuf[i], Len, (void *)pMag);

                    i = Len + i;
                }
            }
            else
            {
                i++;
            }

            if(i == pComTaskMag->m_wProtBufLen)
            {
                pComTaskMag->m_wProtBufLen = 0;
            }
        }

//        #endif

    }

}



/****************************************************************
@FUNCTION：获取联机状态
@INPUT：
    @param Cnt - 重发次数
    @param ScheTim - 重发间隔
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void HmiCom_Prot_CheckOnline(HMICOMTASK_MAG *pMag, uint16_t Cnt, uint32_t ScheTim)
{
    FRAME_BUF *pFrame = NULL;
    uint16_t chkSum;

    assert_param(pMag != NULL);

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    memset((void *)pFrame, 0, sizeof(FRAME_BUF));

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_START;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_CMD_CHKON;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    chkSum = Rx_CRC_CCITT(&pFrame->m_byBuf[1], 0 + 2);
    pFrame->m_byBuf[pFrame->m_wBufLen++] = (chkSum >> 8) & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = chkSum & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xE4;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x1B;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x11;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xEE;

    pFrame->m_dwID = (pFrame->m_byBuf[1] << 8) | pFrame->m_byBuf[2];
    pFrame->m_dwCnt = Cnt;
    pFrame->m_dwScheTime = ScheTim;
	
    SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void *)pFrame);
    //LOG("LinkQueue_Append : %p , len=%d\n\r",pFrame,LinkQueue_Length(pMag->m_sTaskMag.m_pSendQueue));
}


/****************************************************************
@FUNCTION：显示单张或多张图片
@INPUT：
    @param eIdx - 图片编号
    @param Cnt - 重发次数
    @param ScheTim - 重发间隔
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void HmiCom_Prot_PicShow(HMICOMTASK_MAG *pMag, HMI268B_PICIDX eIdx, uint16_t Cnt, uint32_t ScheTim)
{
    FRAME_BUF *pFrame = NULL;
    uint16_t chkSum;

    assert_param(pMag != NULL);

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    memset((void *)pFrame, 0, sizeof(FRAME_BUF));

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_START;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_CMD_PICSHOW;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = (uint8_t)eIdx;
    chkSum = Rx_CRC_CCITT(&pFrame->m_byBuf[1], 0 + 2);
    pFrame->m_byBuf[pFrame->m_wBufLen++] = (chkSum >> 8) & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = chkSum & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xE4;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x1B;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x11;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xEE;

    pFrame->m_dwID = (pFrame->m_byBuf[1] << 8) | pFrame->m_byBuf[2];
    pFrame->m_dwCnt = Cnt;
    pFrame->m_dwScheTime = ScheTim;

    SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void *)pFrame);
    //LOG("LinkQueue_Append : %p , len=%d\n\r",pFrame,LinkQueue_Length(pMag->m_sTaskMag.m_pSendQueue));
}

/****************************************************************
@FUNCTION：显示单张或多张图标
@INPUT：
    @param eIdx - 图标编号
    @param Cnt - 重发次数
    @param ScheTim - 重发间隔
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void HmiCom_Prot_BUTTONShow(HMICOMTASK_MAG *pMag, HMI268B_ICONIDX eIdx, uint16_t Cnt, uint32_t ScheTim)
{
    FRAME_BUF *pFrame = NULL;
    uint16_t chkSum;

    assert_param(pMag != NULL);

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    memset((void *)pFrame, 0, sizeof(FRAME_BUF));

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_START;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI_BUTTONCMD_SHOW;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = (uint8_t)eIdx;
    chkSum = Rx_CRC_CCITT(&pFrame->m_byBuf[1], 0 + 2);
    pFrame->m_byBuf[pFrame->m_wBufLen++] = (chkSum >> 8) & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = chkSum & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xE4;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x1B;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x11;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xEE;

    pFrame->m_dwID = (pFrame->m_byBuf[1] << 8) | pFrame->m_byBuf[2];
    pFrame->m_dwCnt = Cnt;
    pFrame->m_dwScheTime = ScheTim;

    SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void *)pFrame);
    //LOG("LinkQueue_Append : %p , len=%d\n\r",pFrame,LinkQueue_Length(pMag->m_sTaskMag.m_pSendQueue));
}

void HmiCom_Prot_BUTTONHide(HMICOMTASK_MAG *pMag, HMI268B_ICONIDX eIdx, uint16_t Cnt, uint32_t ScheTim)
{
    FRAME_BUF *pFrame = NULL;
    uint16_t chkSum;

    assert_param(pMag != NULL);

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    memset((void *)pFrame, 0, sizeof(FRAME_BUF));

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_START;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI_BUTTONCMD_HIDE;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = (uint8_t)eIdx;
    chkSum = Rx_CRC_CCITT(&pFrame->m_byBuf[1], 0 + 2);
    pFrame->m_byBuf[pFrame->m_wBufLen++] = (chkSum >> 8) & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = chkSum & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xE4;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x1B;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x11;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xEE;

    pFrame->m_dwID = (pFrame->m_byBuf[1] << 8) | pFrame->m_byBuf[2];
    pFrame->m_dwCnt = Cnt;
    pFrame->m_dwScheTime = ScheTim;

    SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void *)pFrame);
    //LOG("LinkQueue_Append : %p , len=%d\n\r",pFrame,LinkQueue_Length(pMag->m_sTaskMag.m_pSendQueue));
}

void HmiCom_Prot_BarShow(HMICOMTASK_MAG *pMag, HMI268B_BARIDX eIdx, uint8_t Buf, uint16_t Cnt, uint32_t ScheTim)
{
    FRAME_BUF *pFrame = NULL;
    uint16_t chkSum;

    assert_param(pMag != NULL);

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    memset((void *)pFrame, 0, sizeof(FRAME_BUF));

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_START;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI_BARCMD;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = (uint8_t)eIdx;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x00;	//AA B0 00 00 32 E7 C0 E4 1B 11 EE 
		pFrame->m_byBuf[pFrame->m_wBufLen++] = Buf;
    chkSum = Rx_CRC_CCITT(&pFrame->m_byBuf[1], 4);
    pFrame->m_byBuf[pFrame->m_wBufLen++] = (chkSum >> 8) & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = chkSum & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xE4;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x1B;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x11;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xEE;

    pFrame->m_dwID = (pFrame->m_byBuf[1] << 8) | pFrame->m_byBuf[2];
    pFrame->m_dwCnt = Cnt;
    pFrame->m_dwScheTime = ScheTim;

    SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void *)pFrame);
    //LOG("LinkQueue_Append : %p , len=%d\n\r",pFrame,LinkQueue_Length(pMag->m_sTaskMag.m_pSendQueue));
}

/****************************************************************
@FUNCTION：文本控件
@INPUT：
    @param Idx - 序号
    @param pTxtBuf - 文本
    @param TxtLen - 文本长度
    @param Cnt - 重发次数
    @param ScheTim - 重发间隔
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void HmiCom_Prot_Text0Show(HMICOMTASK_MAG *pMag, uint8_t Idx, uint8_t *pTxtBuf, uint16_t TxtLen, uint16_t Cnt, uint32_t ScheTim)
{
    FRAME_BUF *pFrame = NULL;
    uint16_t chkSum;

    assert_param(pMag != NULL);

    if(TxtLen > 100)
    {
        return;
    }

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    memset((void *)pFrame, 0, sizeof(FRAME_BUF));

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_START;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_CMD_TXT0SHOW;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = Idx;

    if(pTxtBuf != NULL)
    {
        memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], pTxtBuf, TxtLen);
        chkSum = Rx_CRC_CCITT(&pFrame->m_byBuf[1], TxtLen + 2);
        pFrame->m_wBufLen += TxtLen;
    }
    else
    {
        chkSum = Rx_CRC_CCITT(&pFrame->m_byBuf[1], 0 + 2);
    }

    pFrame->m_byBuf[pFrame->m_wBufLen++] = (chkSum >> 8) & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = chkSum & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xE4;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x1B;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x11;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xEE;

    pFrame->m_dwID = (pFrame->m_byBuf[1] << 8) | pFrame->m_byBuf[2];
    pFrame->m_dwCnt = Cnt;
    pFrame->m_dwScheTime = ScheTim;

    SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void *)pFrame);
    //LOG("LinkQueue_Append : %p , len=%d\n\r",pFrame,LinkQueue_Length(pMag->m_sTaskMag.m_pSendQueue));
}

/****************************************************************
@FUNCTION：文本控件
@INPUT：
    @param Idx - 序号
    @param pTxtBuf - 文本
    @param TxtLen - 文本长度
    @param Cnt - 重发次数
    @param ScheTim - 重发间隔
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void HmiCom_Prot_Text1Show(HMICOMTASK_MAG *pMag, uint8_t Idx, uint8_t *pTxtBuf, uint16_t TxtLen, uint16_t Cnt, uint32_t ScheTim)
{
    FRAME_BUF *pFrame = NULL;
    uint16_t chkSum;

    assert_param(pMag != NULL);

    if(TxtLen > 100)
    {
        return;
    }

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    memset((void *)pFrame, 0, sizeof(FRAME_BUF));

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_START;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_CMD_TXT1SHOW;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = Idx;

    if(pTxtBuf != NULL)
    {
        memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], pTxtBuf, TxtLen);
        chkSum = Rx_CRC_CCITT(&pFrame->m_byBuf[1], TxtLen + 2);
        pFrame->m_wBufLen += TxtLen;
    }
    else
    {
        chkSum = Rx_CRC_CCITT(&pFrame->m_byBuf[1], 0 + 2);
    }

    pFrame->m_byBuf[pFrame->m_wBufLen++] = (chkSum >> 8) & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = chkSum & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xE4;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x1B;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x11;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xEE;

    pFrame->m_dwID = (pFrame->m_byBuf[1] << 8) | pFrame->m_byBuf[2];
    pFrame->m_dwCnt = Cnt;
    pFrame->m_dwScheTime = ScheTim;

    SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void *)pFrame);
    //LOG("LinkQueue_Append : %p , len=%d\n\r",pFrame,LinkQueue_Length(pMag->m_sTaskMag.m_pSendQueue));
}

u8 g_sHmiWeaponPic[][8] =
{
    //步枪0            //冲锋枪1        //手枪2             //班用机枪3      //榴弹发射机4        //狙击枪5       //重机枪6        //迫击炮7
    {HMI_LINE1RIFLE_PIC, HMI_LINE1SMG_PIC, HMI_LINE1PISTOL_PIC, HMI_LINE1SAW_PIC, HMI_LINE1GRENADE_PIC, HMI_LINE1SR_PIC, HMI_LINE1HMG_PIC, HMI_LINE1MORTAR_PIC}, //第一行
    {HMI_LINE2RIFLE_PIC, HMI_LINE2SMG_PIC, HMI_LINE2PISTOL_PIC, HMI_LINE2SAW_PIC, HMI_LINE2GRENADE_PIC, HMI_LINE2SR_PIC, HMI_LINE2HMG_PIC, HMI_LINE2MORTAR_PIC}, //第二行                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ,HMI_LINE2GRENADE_PIC,HMI_LINE2SR_PIC,HMI_LINE2HMG_PIC,HMI_LINE2MORTAR_PIC},//第二行
    {HMI_LINE3RIFLE_PIC, HMI_LINE3SMG_PIC, HMI_LINE3PISTOL_PIC, HMI_LINE3SAW_PIC, HMI_LINE3GRENADE_PIC, HMI_LINE3SR_PIC, HMI_LINE3HMG_PIC, HMI_LINE3MORTAR_PIC}, //第三行
};

u8 g_saHmiWeapon[][7] =
{
    //武器图片         //弹药图片          //武器弹药进度条       //武器弹药数                 //
    {HMI_LINE1RIFLE_PIC, HMI_LINE1BULLET_PIC, HMI_LINE1WEAPONNUM_BAR, HMI_LINE1WEAPONNUM_TXT, HMI_LINE1WEAPONBACK_PIC, HMI_LINE1WEAPONNUMRED_BAR, HMI_WPLINE1BG_PIC}, //第一行
    {HMI_LINE2RIFLE_PIC, HMI_LINE2BULLET_PIC, HMI_LINE2WEAPONNUM_BAR, HMI_LINE2WEAPONNUM_TXT, HMI_LINE2WEAPONBACK_PIC, HMI_LINE2WEAPONNUMRED_BAR, HMI_WPLINE2BG_PIC}, //第二行
    {HMI_LINE3RIFLE_PIC, HMI_LINE3BULLET_PIC, HMI_LINE3WEAPONNUM_BAR, HMI_LINE3WEAPONNUM_TXT, HMI_LINE3WEAPONBACK_PIC, HMI_LINE3WEAPONNUMRED_BAR, HMI_WPLINE3BG_PIC}, //第三行
};

u8 g_aHmiWeaponBattery[][5] =
{
    {HMI_WEAPON1BATTERY100_PIC, HMI_WEAPON1BATTERY75_PIC, HMI_WEAPON1BATTERY50_PIC, HMI_WEAPON1BATTERY25_PIC, HMI_WEAPON1BATTERYLOW_PIC},
    {HMI_WEAPON2BATTERY100_PIC, HMI_WEAPON2BATTERY75_PIC, HMI_WEAPON2BATTERY50_PIC, HMI_WEAPON2BATTERY25_PIC, HMI_WEAPON2BATTERYLOW_PIC},
    {HMI_WEAPON3BATTERY100_PIC, HMI_WEAPON3BATTERY75_PIC, HMI_WEAPON3BATTERY50_PIC, HMI_WEAPON3BATTERY25_PIC, HMI_WEAPON3BATTERYLOW_PIC},
};

uint8_t g_sHurtPic[] =
{
    NULL,		//无=0
    HMI_HEADHURT_PIC,       //头=1
    HMI_LEFTCHESTHURT_PIC,  //左胸=2
    HMI_RIGHTCHESTHURT_PIC, //右胸=3
    HMI_BELLYHURT_PIC,      //腹部=4
    HMI_BACKHURT_PIC,       //背部=5
    HMI_LEFTARMHURT_PIC,    //左臂=6
    HMI_RIGHTARMHURT_PIC,   //右臂=7
    HMI_LEFTLEGHURT_PIC,    //左腿=8
    HMI_RIGHTLEGHURT_PIC,   //右腿=9
    NULL,		//无=10
};

char* Subtype_PTZD[6] = {"NULL","5.8mmPTZD", "7.62mmPTZD", "12.7mmPTZD", "5.56mmPTZD","9mmPTZD"};
char* Subtype_CJD[3] = {"NULL","5.8mmCJD", "12.7mmCJD"};
char* Subtype_CJRSD[3] = {"NULL","5.8mmCJRSD", "12.7mmCJRSD"};
char* Subtype_RSD[3] = {"NULL","5.8mm", "12.7mm"};
char* Subtype_XD[2] = {"NULL","XianDan"};
char* Subtype_SSLD[9] = {"NULL","85mmLD", "125mmLD", "120mmLD", "105mmLD", "100mmLD", "73mmLD", "35mmLD", "40mmLD"};
char* Subtype_YBD[3] = {"NULL","93mmYBD", "80mmYBD"};
char* Subtype_SSBPRSD[2] = {"NULL","80mmSSBPRSD"};
char* Subtype_SSBPLD[2] = {"NULL","73mmSSBPLD"};
char* Subtype_TKCJD[7] = {"NULL","85mmTKCJD", "125TKCJD", "120TKCJD", "105TKCJD", "100TKCJD", "30TKCJD"};

//电池电量图标
void HMI_Battery(HMICOMTASK_MAG *pMag)
{
		SINGLE_SLAVER_MAG *pSlaverMag = NULL;
    uint8_t ret, i, pos = 0;

    ret = BatTask_GetExBatRes();

    if(ret >= 80)
    {
        HmiCom_Prot_PicShow(pMag, PIC_CLOTHBATTERY100, 1, 100);
    }
    else if(ret >= 40 && ret < 80)
    {
        HmiCom_Prot_PicShow(pMag, PIC_CLOTHBATTERY75, 1, 100);
    }
    else if(ret >= 10 && ret < 40)
    {
        HmiCom_Prot_PicShow(pMag, PIC_CLOTHBATTERY50, 1, 100);
    }
    else if(ret > 3 && ret < 10)
    {
        HmiCom_Prot_PicShow(pMag, PIC_CLOTHBATTERY25, 1, 100);
    }
    else 
    {
        HmiCom_Prot_PicShow(pMag, PIC_CLOTHBATTERYLOW, 1, 100);
    }

		for(i = 0; i < UserParam_GetSlaverCnt(); i++)
		{
				pSlaverMag = UserParam_GetSlaverMag(i);

				//pSlaverMag = SlaverMag_GetHandleByType(0xFE,0x08);//找到头盔
				if(pSlaverMag->m_byTType == 0xFE && pSlaverMag->m_byTSubType == 0x08) 
				{
					if(pSlaverMag->m_byIsSleepFlg == TRUE || pSlaverMag->m_byIsOnline == FALSE)
					{
							HmiCom_Prot_PicShow(pMag, HMI_HELMETBATTERY_BACK, 1, 100);
					}
					else
					{
							if(pSlaverMag->m_wBatVol > 3860)
							{
									HmiCom_Prot_PicShow(pMag, HMI_HELMETBATTERY100_PIC, 1, 100);
							}
							else if(pSlaverMag->m_wBatVol > 3620 && pSlaverMag->m_wBatVol <= 3860)
							{
									HmiCom_Prot_PicShow(pMag, HMI_HELMETBATTERY75_PIC, 1, 100);
							}
							else if(pSlaverMag->m_wBatVol > 3300 && pSlaverMag->m_wBatVol <= 3620)
							{
									HmiCom_Prot_PicShow(pMag, HMI_HELMETBATTERY50_PIC, 1, 100);
							}
							else if(pSlaverMag->m_wBatVol > 3100 && pSlaverMag->m_wBatVol <= 3300)
							{
									HmiCom_Prot_PicShow(pMag, HMI_HELMETBATTERY25_PIC, 1, 100);
							}
							else
							{
									HmiCom_Prot_PicShow(pMag, HMI_HELMETBATTERYLOW_PIC, 1, 100);
							}
					 }
					 continue;
				}
				
				if(pSlaverMag->m_byIsOnline == FALSE)
					continue;

				if(pos >= 3)
					return;

				//显示武器电池电量图片
				if(pSlaverMag->m_byIsSleepFlg == FALSE)
				{
					if(pSlaverMag->m_wBatVol > 1850)
					{
							HmiCom_Prot_PicShow(pMag, g_aHmiWeaponBattery[pos][0], 1, 100);
					}
					else if(pSlaverMag->m_wBatVol > 1750 && pSlaverMag->m_wBatVol <= 1850)
					{
							HmiCom_Prot_PicShow(pMag, g_aHmiWeaponBattery[pos][1], 1, 100);
					}
					else if(pSlaverMag->m_wBatVol > 1650 && pSlaverMag->m_wBatVol <= 1750)
					{
							HmiCom_Prot_PicShow(pMag, g_aHmiWeaponBattery[pos][2], 1, 100);
					}
					else if(pSlaverMag->m_wBatVol > 1550 && pSlaverMag->m_wBatVol <= 1650)
					{
							HmiCom_Prot_PicShow(pMag, g_aHmiWeaponBattery[pos][3], 1, 100);
					}
					else
					{
							HmiCom_Prot_PicShow(pMag, g_aHmiWeaponBattery[pos][4], 1, 100);
					}
				}
				pos++;
		}
    #if 0

    for(u8 i = 0; i < SYS_SLAVER_CNT_MAX; i++)
    {
        if(g_sUserPara.m_saSlaverMag[i].m_byTtype == 0xFE &&
                g_sUserPara.m_saSlaverMag[i].m_byTSubtype == 0x08 &&
                g_sUserPara.m_saSlaverMag[i].m_byIsOnline == TRUE &&
                g_sSysPara.m_saBleSlaver[i].m_byOnlineState == TRUE)
        {
            if(g_sUserPara.m_saSlaverMag[i].DataUnion.m_sCloth.m_byBatRes > 75)
            {
                SendToHmi(HMI_PICCMD, HMI_HELMETBATTERY100_PIC, NULL, 0, pMag);
            }
            else if(g_sUserPara.m_saSlaverMag[i].DataUnion.m_sCloth.m_byBatRes > 50 && g_sUserPara.m_saSlaverMag[i].DataUnion.m_sCloth.m_byBatRes <= 75)
            {
                SendToHmi(HMI_PICCMD, HMI_HELMETBATTERY75_PIC, NULL, 0, pMag);
            }
            else if(g_sUserPara.m_saSlaverMag[i].DataUnion.m_sCloth.m_byBatRes > 25 && g_sUserPara.m_saSlaverMag[i].DataUnion.m_sCloth.m_byBatRes <= 50)
            {
                SendToHmi(HMI_PICCMD, HMI_HELMETBATTERY50_PIC, NULL, 0, pMag);
            }
            else if(g_sUserPara.m_saSlaverMag[i].DataUnion.m_sCloth.m_byBatRes > 15 && g_sUserPara.m_saSlaverMag[i].DataUnion.m_sCloth.m_byBatRes <= 25)
            {
                SendToHmi(HMI_PICCMD, HMI_HELMETBATTERY25_PIC, NULL, 0, pMag);
            }
            else
            {
                SendToHmi(HMI_PICCMD, HMI_HELMETBATTERYLOW_PIC, NULL, 0, pMag);
            }
        }

        if(g_sSysPara.m_saBleSlaver[i].m_byID == 0xFE &&
                g_sSysPara.m_saBleSlaver[i].m_bySubType == 0x08 &&
                g_sSysPara.m_saBleSlaver[i].m_byOnlineState == FALSE)
        {
            SendToHmi(HMI_PICCMD, HMI_HELMETBATTERY_BACK, NULL, 0, pMag);
        }
    }

    #endif

}

#if 0
//滚动字符串
void HMI_Time(HMICOMTASK_MAG *pMag)
{
    uint8_t Str[] = "Huaru";

    uint8_t Buf[50] = {0};
    uint32_t NowTime = GetSysRunSecond();
    uint8_t l, H, M, S;
    static uint8_t i = 0;

    H = NowTime / 3600;
    NowTime %= 3600;
    M = NowTime / 60;
    NowTime %= 60;
    S = NowTime;

    //sprintf((char *)Buf,"%02d:%02d:%02d",H,M,S);

    l = strlen(Str);

    memcpy(&Buf[0], &Str[i], l - i);
    memcpy(&Buf[l - i], &Str[0], i);
    i++;
    i %= l;

    HmiCom_Prot_Text0Show(pMag, 1, Buf, strlen(Buf), 1, 1000);
}
#endif


//时间图标
void HMI_Time(HMICOMTASK_MAG *pMag)
{
    uint8_t Buf[50] = {0};
    uint32_t NowTime = GetSysRunSecond();
    uint8_t H, M, S;

    H = NowTime / 3600;
    NowTime %= 3600;
    M = NowTime / 60;
    NowTime %= 60;
    S = NowTime;

    sprintf((char *)Buf, "%02d:%02d:%02d", H, M, S);

    HmiCom_Prot_Text0Show(pMag, 1, Buf, strlen((char *)Buf), 1, 1000);
}

uint8_t GetInjuryPic(int8_t Blood)
{
    if(Blood > 0 && Blood < 41)
    {
				return HMI_INJURYWZS_PIC;//危重伤
    }
    else if(Blood >= 41 && Blood < 74)
    {
        return HMI_INJURYHDS_PIC;//重度伤
    }
    else if(Blood >= 74 && Blood < 91)
    {
        return HMI_INJURYZDS_PIC;//中度伤
    }
    else if(Blood >= 91 && Blood < 100)
    {
        return HMI_INJURYQS_PIC;//轻伤
    }
    else if(Blood >= 100)
    {
        return HMI_INJURYZC_PIC;//正常
    }
    else
    {
        return HMI_INJURYSW_PIC;//死亡
    }
}

void HMI_Damage(HMICOMTASK_MAG *pMag)
{
		uint8_t Blood[HMIBUFLEN] = {0};
    uint8_t HurtDegree = 0;

		HmiCom_Prot_PicShow(pMag, HMI_INJURYBACKGROUND_PIC, 1, 100);
		//伤势
		HurtDegree = GetInjuryPic(g_sUserParam.m_byBlood);
		HmiCom_Prot_PicShow(pMag, HurtDegree, 1, 200);

		//血量
		sprintf((char*)Blood, "%d", g_sUserParam.m_byBlood);
		HmiCom_Prot_Text0Show(pMag, HMI_BLOOD_TXT, Blood, strlen((char *)Blood), 1, 1000);
}

void HMI_Show_Hurt_Kind(uint8_t Kind)
{
		HMICOMTASK_MAG *pMag = HmiComTask_HandleGet();
		static uint8_t LastHurtKind = 0xFF;

    if(Kind != LastHurtKind)
    {
				HmiCom_Prot_PicShow(pMag, Kind, 1, 100);
        LastHurtKind = Kind;
    }
}

void Hmi_Version_Send(HMICOMTASK_MAG *pMag)
{
    uint8_t EvtNum[50] = {0};
    sprintf((char*)EvtNum, "V%d.%d.%d", MAINVERSION, SUBVERSION, FIXVERSION);
        
    HmiCom_Prot_Text0Show(pMag, 6, EvtNum, strlen((char *)EvtNum), 1, 1000);
}

void LCDScreenSleepHandle(HMICOMTASK_MAG *pMag)
{
		FRAME_BUF *pFrame = NULL;
    uint16_t chkSum;
		uint32_t NowTime = GetSysRunMilliSecond();

		assert_param(pMag != NULL);

		if(Is_PwrKeyOn())
		{
			pMag->m_byDispalyOnFlg = TRUE;
			pMag->m_dwLastDisplayOnTime = GetSysRunMilliSecond();
			
			pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

			if(pFrame == NULL)
			{
					return;
			}
		
			memset((void *)pFrame, 0, sizeof(FRAME_BUF));

			pFrame->m_wBufLen = 0;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_START;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI_DISPLAY_CTRL;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byDispalyOnFlg;
			chkSum = Rx_CRC_CCITT(&pFrame->m_byBuf[1], 0 + 2);
			
			pFrame->m_byBuf[pFrame->m_wBufLen++] = (chkSum >> 8) & 0xFF;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = chkSum & 0xFF;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xE4;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x1B;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x11;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xEE;

			pFrame->m_dwID = (pFrame->m_byBuf[1] << 8) | pFrame->m_byBuf[2];
			pFrame->m_dwCnt = 1;
			pFrame->m_dwScheTime = 1000;

			SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void *)pFrame);

		}
		else
		{
			if((NowTime - pMag->m_dwLastDisplayOnTime) >= 30000) 
			{
				pMag->m_dwLastDisplayOnTime = GetSysRunMilliSecond();
				pMag->m_byDispalyOnFlg = FALSE;

				pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

				if(pFrame == NULL)
				{
						return;
				}
				
				memset((void *)pFrame, 0, sizeof(FRAME_BUF));

				pFrame->m_wBufLen = 0;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_START;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI_DISPLAY_CTRL;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byDispalyOnFlg;
				chkSum = Rx_CRC_CCITT(&pFrame->m_byBuf[1], 0 + 2);
				
				pFrame->m_byBuf[pFrame->m_wBufLen++] = (chkSum >> 8) & 0xFF;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = chkSum & 0xFF;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xE4;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x1B;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x11;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xEE;

				pFrame->m_dwID = (pFrame->m_byBuf[1] << 8) | pFrame->m_byBuf[2];
				pFrame->m_dwCnt = 1;
				pFrame->m_dwScheTime = 1000;

				SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void *)pFrame);
			}
			else
				return;
		}
}

uint8_t GetHurtPic(ENUM_PARTITION Part)
{
    return g_sHurtPic[Part];
}

void HMI_HelmetConnect(HMICOMTASK_MAG *pMag)
{
		uint8_t i, Pic;
	
		if(pMag->m_byHelmetIsOnline == TRUE)// && pMag->m_byFirstDispFlg == TRUE)
		{
			HmiCom_Prot_PicShow(pMag, HMI_GREENSOLDIER_PIC, 1, 0);
			
			for(i = 0; i <= HMI_INJURYLEN; i++)
			{
					Pic = GetHurtPic(pMag->m_sInjury[i].m_byPart);

					if(Pic != NULL)
					{
							HmiCom_Prot_PicShow(pMag, Pic, 1, 200);
					}
			}
			if(pMag->m_HurtKind != 0)	
				HMI_Show_Hurt_Kind(pMag->m_HurtKind);
		}
		else// if(pMag->m_byHelmetIsOnline == FALSE && pMag->m_byFirstDispFlg == FALSE)
		{
			//上电显示黑色单兵底图，默认没有连接到头盔
			HmiCom_Prot_PicShow(pMag, HMI_BLACKSOLDIER_PIC, 1, 0);
			//pMag->m_byFirstDispFlg = TRUE;
		}
}

//根据武器ID获取武器名字图片
//步枪0x31 //冲锋枪0x34  //手枪0x36 //班用机枪0x32 0x39//榴弹发射机0x35 0x37 //狙击枪0x33 //重机枪0x38 //迫击炮0x14 0x40 0x41 0x42
uint8_t GetWeaponPic(uint8_t Weapon, uint8_t* WeaponPic, uint8_t LineNum)
{
    if(LineNum > 3) //LineNum可以是0,1,2
        return 0;

    switch(Weapon)
    {
        case 0x31:   //步枪
            *WeaponPic = g_sHmiWeaponPic[LineNum][0];
            break;

        case 0x34 :  //冲锋枪
            *WeaponPic = g_sHmiWeaponPic[LineNum][1];
            break;

        case 0x36:   //手枪
            *WeaponPic = g_sHmiWeaponPic[LineNum][2];
            break;

        case 0x39:
        case 0x32:   //班用机枪
            *WeaponPic = g_sHmiWeaponPic[LineNum][3];
            break;

        case 0x37:
        case 0x35:   //榴弹发射器
            *WeaponPic = g_sHmiWeaponPic[LineNum][4];
            break;

        case 0x33:   //狙击枪
            *WeaponPic = g_sHmiWeaponPic[LineNum][5];
            break;

        case 0x38:   //重机枪
            *WeaponPic = g_sHmiWeaponPic[LineNum][6];
            break;

        case 0x13:   //迫击炮
            *WeaponPic = g_sHmiWeaponPic[LineNum][7];
            break;

        case 0x42:
        case 0x41:
        case 0x40:
        case 0x14:		//迫击炮
				case 0x25:
            *WeaponPic = g_sHmiWeaponPic[LineNum][7];
            break;

        default:
            //*WeaponPic = g_sHmiWeaponPic[LineNum][0];//默认返回步枪
            break;
    }
}

//发送给HMI
void SendToHmi(uint8_t Cmd, uint8_t Widget, uint8_t *Buf, uint8_t BufLen/*文本控件为本身长度，图片控件写0，按钮控件写0，进度条控件写1,*/, HMICOMTASK_MAG *pMag)
{
    /*****@【文本控件】
        AA C0 01 31 33 B4 E4 1B 11 EE
        AA	帧头
        C0	指令，显示字库1文字
        01	文本控件序号
        31	显示字符 ’1’
        33 B4  CRC校验，C0 01 31的CRC为33 B4
        E4 1B 11 EE  帧尾
    *****/

    /*****@【图片控件】
        报文例：AA 80 01 0B B9 E4 1B 11 EE 显示1号图片控件
        AA 80 04 5B 1C E4 1B 11 EE  显示4号图片控件
    *****/

    /*****@【进度条控件】
        报文例： AA B0 00 00 3E 26 4C E4 1B 11 EE
    *****/
    unsigned int chkSum = 0;

    uint8_t SendBuf[HMIBUFLEN] = {0xAA, Cmd, Widget, 0x00};

    if(Cmd == HMI268B_FRAME_CMD_PICSHOW && BufLen == 0) //图片控件
    {
        BufLen += 2;
        chkSum = Rx_CRC_CCITT(&SendBuf[1], BufLen);
        BufLen = 3;

        SendBuf[BufLen++] = (chkSum >> 8) & 0xFF;
        SendBuf[BufLen++] = chkSum & 0xFF;
        SendBuf[BufLen++] = 0xE4;
        SendBuf[BufLen++] = 0x1B;
        SendBuf[BufLen++] = 0x11;
        SendBuf[BufLen++] = 0xEE;
        pMag->m_pLLMag->m_sHwOps.write((uint8_t*)SendBuf, BufLen);
    }
    else if((Cmd == HMI_BUTTONCMD_SHOW || Cmd == HMI_BUTTONCMD_HIDE) && BufLen == 0)
    {
        BufLen += 2;
        chkSum = Rx_CRC_CCITT(&SendBuf[1], BufLen);
        BufLen = 3;

        SendBuf[BufLen++] = (chkSum >> 8) & 0xFF;
        SendBuf[BufLen++] = chkSum & 0xFF;
        SendBuf[BufLen++] = 0xE4;
        SendBuf[BufLen++] = 0x1B;
        SendBuf[BufLen++] = 0x11;
        SendBuf[BufLen++] = 0xEE;
        pMag->m_pLLMag->m_sHwOps.write((uint8_t*)SendBuf, BufLen);
    }
    else if(Cmd == HMI_BARCMD && BufLen == 1) //进度条控件
    {
        BufLen += 3;
        SendBuf[BufLen++] = Buf[0];     //进度条范围00 00 ~~ 00 3E，
        chkSum = Rx_CRC_CCITT(&SendBuf[1], 4);

        SendBuf[BufLen++] = (chkSum >> 8) & 0xFF;
        SendBuf[BufLen++] = chkSum & 0xFF;
        SendBuf[BufLen++] = 0xE4;
        SendBuf[BufLen++] = 0x1B;
        SendBuf[BufLen++] = 0x11;
        SendBuf[BufLen++] = 0xEE;
        pMag->m_pLLMag->m_sHwOps.write((uint8_t*)SendBuf, BufLen);
    }
    else
    {
        memcpy(&SendBuf[3], Buf, strlen((const char*)Buf));

        BufLen = BufLen + 3;
        chkSum = Rx_CRC_CCITT(&SendBuf[1], BufLen - 1);

        SendBuf[BufLen++] = (chkSum >> 8) & 0xFF;
        SendBuf[BufLen++] = chkSum & 0xFF;
        SendBuf[BufLen++] = 0xE4;
        SendBuf[BufLen++] = 0x1B;
        SendBuf[BufLen++] = 0x11;
        SendBuf[BufLen++] = 0xEE;

        pMag->m_pLLMag->m_sHwOps.write((uint8_t*)SendBuf, BufLen);
    }

    if(0x39 == Widget)
    {
        //pMag->m_GreenSolderReplyFlg = TRUE;
    }

}

void HMI_WeaponInfo(HMICOMTASK_MAG *pMag)
{
		SINGLE_SLAVER_MAG *pSlaverMag = NULL;
		uint32_t Bullet_RemainNum, Bullet_CfgNum;
		uint8_t n, b, i, pos = 0, bulletBar = 0, bulletNum[HMIBUFLEN] = {0};
		char* str = "PDA";

		for(i = 0; i < UserParam_GetSlaverCnt(); i++)
		{
				pSlaverMag = UserParam_GetSlaverMag(i);

				if(pSlaverMag->m_byTType == 0xFE && pSlaverMag->m_byTSubType == 0x08) 
					continue;

				if(pSlaverMag->m_byIsOnline == FALSE)
					continue;

				if(pos >= 3)
					return;

				if(pSlaverMag->m_byTType != 0xFE)
				{
						n = pSlaverMag->m_byWeaponIdx;

						if(g_sUserParam.m_saAmmuInfo[n].m_byWeapon_Type == 0)
							continue;
						
						if(pSlaverMag->m_byIsSleepFlg == TRUE)
						{
								HmiCom_Prot_PicShow(pMag, g_saHmiWeapon[pos][6], 1, 100);
						}
						else
						{
								//获取对应武器的图片控件
								GetWeaponPic(g_sUserParam.m_saAmmuInfo[n].m_byWeapon_Type, &g_saHmiWeapon[pos][0], pos);
								//武器图片
								HmiCom_Prot_PicShow(pMag, g_saHmiWeapon[pos][0], 1, 100);
								//弹药图片
								HmiCom_Prot_PicShow(pMag, g_saHmiWeapon[pos][1], 1, 100);
								
								Bullet_RemainNum = g_sUserParam.m_saAmmuInfo[n].m_sBulletInfo[0].m_aBulletRemainNum[0] << 16 |
																	 g_sUserParam.m_saAmmuInfo[n].m_sBulletInfo[0].m_aBulletRemainNum[1] << 8 |
																	 g_sUserParam.m_saAmmuInfo[n].m_sBulletInfo[0].m_aBulletRemainNum[2];

								Bullet_CfgNum = g_sUserParam.m_saAmmuInfo[n].m_sBulletInfo[0].m_aBulletTotalNum[0] << 16 |
																g_sUserParam.m_saAmmuInfo[n].m_sBulletInfo[0].m_aBulletTotalNum[1] << 8 |
																g_sUserParam.m_saAmmuInfo[n].m_sBulletInfo[0].m_aBulletTotalNum[2];

								//剩余弹药量及总单量
								sprintf((char*)bulletNum, "%d|%d", Bullet_RemainNum, Bullet_CfgNum);
								HmiCom_Prot_Text0Show(pMag, g_saHmiWeapon[pos][3], bulletNum, strlen((char *)bulletNum), 1, 100);	

								bulletBar = (Bullet_RemainNum / 1.0) / (Bullet_CfgNum / 1.0) * 0x20; //换算成进度条的占比，进度条最大范围0x20

								if(bulletBar < 0x10) //如果剩余弹药比例小于总数一半，调用红色的进度条
								{
										HmiCom_Prot_BarShow(pMag, g_saHmiWeapon[pos][5], (uint8_t)bulletBar, 1, 100);
								}
								else
								{
										HmiCom_Prot_BarShow(pMag, g_saHmiWeapon[pos][2], (uint8_t)bulletBar, 1, 100);
								}
						}
				}
				else
				{
						if(pSlaverMag->m_byIsSleepFlg == TRUE)
								HmiCom_Prot_PicShow(pMag, g_saHmiWeapon[pos][6], 1, 100);
						else
								HmiCom_Prot_Text0Show(pMag, g_saHmiWeapon[pos][3], (uint8_t *)str, strlen(str), 1, 200);
				}
				pos++;
		}
}

//定位图标
void HMI_PosPic(HMICOMTASK_MAG *pMag)
{

    uint8_t TimeBuf[HMIBUFLEN] = {0}, WD[HMIBUFLEN] = {0}, JD[HMIBUFLEN] = {0}; 
			
    if(g_sUserParam.m_sLPosition.m_byLposState)
    {
				HmiCom_Prot_PicShow(pMag, HMI_GPS_PIC, 1, 100);	
				sprintf((char*)TimeBuf, "%d:%d", g_sUserParam.m_sUtcTime.m_byHour, g_sUserParam.m_sUtcTime.m_byMinute);			
    }
    else
    {
				//UTC时间
				sprintf((char*)TimeBuf, "00:00");
				HmiCom_Prot_Text1Show(pMag, HMI_TIME_TXT, TimeBuf, strlen((char *)TimeBuf), 1, 1000);
				
				HmiCom_Prot_PicShow(pMag, HMI_NOGPS_PIC, 1, 100);
    }
}

//获取时间
void HMI_UctTime(HMICOMTASK_MAG *pMag)
{
	uint8_t TimeBuf[HMIBUFLEN] = {0};

	if(g_sMpu6050Mag.Pose == POSE_ZHAN)
	{
			sprintf((char*)TimeBuf, "立|%d", g_sUserParam.m_byKey);
	}
	else if(g_sMpu6050Mag.Pose == POSE_ZOU)
	{
			sprintf((char*)TimeBuf, "走|%d", g_sUserParam.m_byKey);
	}
	else if(g_sMpu6050Mag.Pose == POSE_PAO)
	{
			sprintf((char*)TimeBuf, "跑|%d", g_sUserParam.m_byKey);
	}
	else if(g_sMpu6050Mag.Pose == POSE_WO)
	{
			sprintf((char*)TimeBuf, "卧|%d", g_sUserParam.m_byKey);
	}
	else if(g_sMpu6050Mag.Pose == POSE_PUFU)
	{
			sprintf((char*)TimeBuf, "匍匐");
	}
	HmiCom_Prot_Text1Show(pMag, HMI_POSTURE_TXT, TimeBuf, strlen((char *)TimeBuf), 1, 1000);
}

// 经纬度转高斯坐标
void GaussProjCal(const double longitude/*经度*/, const double latitude/*纬度*/, double *X, double *Y, int ZoneWide/*选6度带宽方法*/)
{
    int ProjNo = 0;
    double longitude1, latitude1, longitude0, latitude0, X0, Y0, xval, yval;
    double a, f, e2, ee, NN, T, C, A, M, iPI;

    iPI = 0.0174532925199433;   // Pi/180.0;
		//	// 北京54坐标体系
		//	a = 6378245.0;
		//	f = 1.0/298.3;

    // 80西安坐标系参数
    // a = 6378140.0;
    // f = 1.0 / 298.257;

    // 2000中国大地坐标系
    a = 6378137.0;
    f = 0.003352811;

    if (6 == ZoneWide)  // 6度带宽方法
    {
        ProjNo = (int)(longitude / ZoneWide);
        longitude0 = ProjNo * ZoneWide + ZoneWide / 2.0;
        ProjNo += 1;
    }
    else if (3 == ZoneWide) // 3度带宽方法
    {
        ProjNo = (int)((longitude - ZoneWide / 2.0) / ZoneWide) + 1;
        longitude0 = ProjNo + ZoneWide;
    }
    else
    {
        return;
    }

    longitude0 = longitude0 * iPI;
    latitude0 = 0;
    longitude1 = longitude * iPI;
    latitude1 = latitude * iPI;
    e2 = 2 * f - f * f;
    ee = e2 * (1.0 - e2);
    NN = a / sqrt(1.0 - e2 * sin(latitude1) * sin(latitude1));
    T = tan(latitude1) * tan(latitude1);
    C = ee * cos(latitude1) * cos(latitude1);
    A = (longitude1 - longitude0) * cos(latitude1);
    M = a * ((1 - e2 / 4 - 3 * e2 * e2 / 64 - 5 * e2 * e2 * e2 / 256) * latitude1 - (3 * e2 / 8 + 3 * e2 * e2 / 32 + 45 * e2 * e2 * e2 / 1024) * sin(2 * latitude1) + (15 * e2 * e2 / 256 + 45 * e2 * e2 * e2 / 1024) * sin(4 * latitude1) - (35 * e2 * e2 * e2 / 3072) * sin(6 * latitude1));
    xval = NN * (A + (1 - T + C) * A * A * A / 6 + (5 - 18 * T + T * T + 72 * C - 58 * ee) * A * A * A * A * A / 120);
    yval = M + NN * tan(latitude1) * (A * A / 2 + (5 - T + 9 * C + 4 * C * C) * A * A * A * A / 24 + (61 - 58 * T + T * T + 600 * C - 330 * ee) * A * A * A * A * A * A / 720);
    X0 = 1000000L * ProjNo + 500000L;
    Y0 = 0;
    xval = xval + X0;
    yval = yval + Y0;
    *X = xval;
    *Y = yval;
}

//位置
void HMI_Location(HMICOMTASK_MAG *pMag)
{
		uint8_t i, JD[50] = {0};
		uint8_t WD[50] = {0};
		double X = 0, Y = 0;
		
		pMag->m_Jingdu = g_sResComTask_Mag.m_sNmeaMag.m_byJD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byJD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwJD_Fen_XiaoShu / 100000.0) / 60.0; 
		
		pMag->m_Weidu = g_sResComTask_Mag.m_sNmeaMag.m_byWD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byWD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwWD_Fen_XiaoShu / 100000.0) / 60.0;
										
		//经度相关
		if(g_sResComTask_Mag.DrillParam.LocationShowType == FALSE)
		{
				GaussProjCal(pMag->m_Jingdu, pMag->m_Weidu, &X, &Y, 6);
				if('E' == g_sResComTask_Mag.m_sNmeaMag.m_byEorW)
				{
						if(g_sResComTask_Mag.m_sNmeaMag.RTKMode == TRUE)
						{
								//sprintf((char*)JD, "%.8lf°E", pMag->m_Jingdu);
								sprintf((char*)JD, "%.0f", Y);
						}
						else
						{
								sprintf((char*)JD, "%.0f", Y);
						}

						HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONX_TXT, JD, strlen((char *)JD), 1, 1000);
				}
				else if('W' == g_sResComTask_Mag.m_sNmeaMag.m_byEorW)
				{
						if(g_sResComTask_Mag.m_sNmeaMag.RTKMode == TRUE)
						{
								//sprintf((char*)JD, "%.8lf°W", pMag->m_Jingdu);
								sprintf((char*)JD, "%.0f", Y);
						}
						else
						{
								sprintf((char*)JD, "%.0f", Y);
						}

						HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONX_TXT, JD, strlen((char *)JD), 1, 1000);
				}
				else
				{
						sprintf((char*)JD, "00000000");
						HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONX_TXT, JD, strlen((char *)JD), 1, 1000);
				}

				//纬度相关
				if('S' == g_sResComTask_Mag.m_sNmeaMag.m_byNorS)
				{
						if(g_sResComTask_Mag.m_sNmeaMag.RTKMode == TRUE)
						{
								//sprintf((char*)WD, "%.8lf°S", pMag->m_Weidu);
								sprintf((char*)WD, "%.0f", X);
						}
						else
						{
								sprintf((char*)WD, "%.0f", X);
						}
						HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONY_TXT, WD, strlen((char *)WD), 1, 1000);
				}
				else if('N' == g_sResComTask_Mag.m_sNmeaMag.m_byNorS)
				{
						if(g_sResComTask_Mag.m_sNmeaMag.RTKMode == TRUE)
						{
								//sprintf((char*)WD, "%.8lf°N", pMag->m_Weidu);
								sprintf((char*)WD, "%.0f", X);
						}
						else
						{
								sprintf((char*)WD, "%.0f", X);
						}

						HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONY_TXT, WD, strlen((char *)WD), 1, 1000);
				}
				else
				{
						sprintf((char*)WD, "00000000");
						HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONY_TXT, WD, strlen((char *)WD), 1, 1000);
				}	
		}
		else
		{
			if('E' == g_sResComTask_Mag.m_sNmeaMag.m_byEorW)
			{
					if(g_sResComTask_Mag.m_sNmeaMag.RTKMode == TRUE)
					{
							sprintf((char*)JD, "%.8lf°E", pMag->m_Jingdu);
					}
					else
					{
							sprintf((char*)JD, "%.5lf°E", pMag->m_Jingdu);
					}

					for(i = 0; i < 12; i++)
					{
						if(JD[i] == 0)
						{
							JD[i] = 0x2E;
							break;
						}
					}
					HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONX_TXT, JD, strlen((char *)JD), 1, 1000);
			}
			else if('W' == g_sResComTask_Mag.m_sNmeaMag.m_byEorW)
			{
					if(g_sResComTask_Mag.m_sNmeaMag.RTKMode == TRUE)
					{
							sprintf((char*)JD, "%.8lf°W", pMag->m_Jingdu);
					}
					else
					{
							sprintf((char*)JD, "%.5lf°W", pMag->m_Jingdu);
					}

					HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONX_TXT, JD, strlen((char *)JD), 1, 1000);
			}
			else
			{
					sprintf((char*)JD, "00.00000");
					HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONX_TXT, JD, strlen((char *)JD), 1, 1000);
			}

			//纬度相关
			if('S' == g_sResComTask_Mag.m_sNmeaMag.m_byNorS)
			{
					if(g_sResComTask_Mag.m_sNmeaMag.RTKMode == TRUE)
					{
							sprintf((char*)WD, "%.8lf°S", pMag->m_Weidu);
					}
					else
					{
							sprintf((char*)WD, "%.5lf°S", pMag->m_Weidu);
					}
					HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONY_TXT, WD, strlen((char *)WD), 1, 1000);
			}
			else if('N' == g_sResComTask_Mag.m_sNmeaMag.m_byNorS)
			{
					if(g_sResComTask_Mag.m_sNmeaMag.RTKMode == TRUE)
					{
							sprintf((char*)WD, "%.8lf°N", pMag->m_Weidu);
					}
					else
					{
							sprintf((char*)WD, "%.5lf°N", pMag->m_Weidu);
					}

					for(i = 0; i < 12; i++)
					{
						if(WD[i] == 0)
						{
							WD[i] = 0x2E;
							break;
						}
					}
					HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONY_TXT, WD, strlen((char *)WD), 1, 1000);
			}
			else
			{
					sprintf((char*)WD, "00.00000");
					HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONY_TXT, WD, strlen((char *)WD), 1, 1000);
			}			
		}		
		//sprintf((char*)JD, "%.5lf°", pMag->m_Jingdu);
		//sprintf((char*)WD, "%.5lf°", pMag->m_Weidu);
		//HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONX_TXT, JD, strlen((char *)JD), 1, 1000);
		//HmiCom_Prot_Text0Show(pMag, HMI_LOCATIONY_TXT, WD, strlen((char *)WD), 1, 1000);
		/*
		pMag->m_sLastPos.m_byJD_DuHundred = g_sUserParam.m_sLPosition.m_byJD_DuHundred;
		pMag->m_sLastPos.m_byJD_DuTen = g_sUserParam.m_sLPosition.m_byJD_DuTen;
		pMag->m_sLastPos.m_byJD_DuUnit = g_sUserParam.m_sLPosition.m_byJD_DuUnit;
		pMag->m_sLastPos.m_byJD_FenTen = g_sUserParam.m_sLPosition.m_byJD_FenTen;
		pMag->m_sLastPos.m_byJD_FenUnit = g_sUserParam.m_sLPosition.m_byJD_FenUnit;
		pMag->m_sLastPos.m_byJD_FenTenth = g_sUserParam.m_sLPosition.m_byJD_FenTenth;
		pMag->m_sLastPos.m_byJD_FenPercent = g_sUserParam.m_sLPosition.m_byJD_FenPercent;
		pMag->m_sLastPos.m_byJD_FenThousandth = g_sUserParam.m_sLPosition.m_byJD_FenThousandth;
		pMag->m_sLastPos.m_byJD_FenTenThousandth = g_sUserParam.m_sLPosition.m_byJD_FenTenThousandth;
		pMag->m_sLastPos.m_byWD_DuTen = g_sUserParam.m_sLPosition.m_byWD_DuTen;
		pMag->m_sLastPos.m_byWD_DuUnit = g_sUserParam.m_sLPosition.m_byWD_DuUnit;
		pMag->m_sLastPos.m_byJD_FenTen = g_sUserParam.m_sLPosition.m_byJD_FenTen;
		pMag->m_sLastPos.m_byJD_FenUnit = g_sUserParam.m_sLPosition.m_byJD_FenUnit;
		pMag->m_sLastPos.m_byJD_FenTenth = g_sUserParam.m_sLPosition.m_byJD_FenTenth;
		pMag->m_sLastPos.m_byJD_FenPercent = g_sUserParam.m_sLPosition.m_byJD_FenPercent;
		pMag->m_sLastPos.m_byJD_FenThousandth = g_sUserParam.m_sLPosition.m_byJD_FenThousandth;
		pMag->m_sLastPos.m_byJD_FenTenThousandth = g_sUserParam.m_sLPosition.m_byJD_FenTenThousandth;
		*/
}

//信号图标
void HMI_Signal(HMICOMTASK_MAG *pMag)
{
		if(g_sResComTask_Mag.m_IsAccess == FALSE)		
			HmiCom_Prot_PicShow(pMag, HMI_NOSIGNAL_PIC, 1, 100);	
		else
			HmiCom_Prot_PicShow(pMag, HMI_SIGNAL_PIC, 1, 100);	
}

void HMI_UnUpLoadEvtNum(HMICOMTASK_MAG *pMag)
{
    uint8_t EvtNum[50] = {0};
    uint32_t EpNum = g_sUserParam.EP_Num[0] << 16 | g_sUserParam.EP_Num[1] << 8 | g_sUserParam.EP_Num[2];
    static uint8_t Flg = 0;

    //三种状态切换
		switch(Flg)
		{
			case 0:
				sprintf((char*)EvtNum, "%d  V%d.%d.%d", g_sResComTask_Mag.UnUpLoadEvtCnt, MAINVERSION, SUBVERSION, FIXVERSION);
        HmiCom_Prot_Text0Show(pMag, HMI_UNUPLOADEVTNO_TXT, EvtNum, strlen((const char*)EvtNum), 1, 1000);
				Flg = 1;
				break;
			
			case 1:
				sprintf((char*)EvtNum, "NO:%d", EpNum);
        HmiCom_Prot_Text0Show(pMag, HMI_UNUPLOADEVTNO_TXT, EvtNum, strlen((const char*)EvtNum), 1, 1000);
				Flg = 2;
				break;
			
			case 2:
				sprintf((char*)EvtNum, "TM:%02d_%02d_%02d", g_sUserParam.m_sManoStart.byDay,
                g_sUserParam.m_sManoStart.byHour,
                g_sUserParam.m_sManoStart.byMin);
        HmiCom_Prot_Text0Show(pMag, HMI_UNUPLOADEVTNO_TXT, EvtNum, strlen((const char*)EvtNum), 1, 1000);
				Flg = 0;
				break;
				
			default:
				break;
		}
}

void HMI_Mode(HMICOMTASK_MAG *pMag)
{
		if(g_sUserParam.m_byMode == TRAIN)		//训练模式
		{
				HmiCom_Prot_PicShow(pMag, HMI_TRAIN_PIC, 1, 100);
		}
		else		//演习模式
		{
				HmiCom_Prot_PicShow(pMag, HMI_MANO_PIC, 1, 100);
		}
}

void LCDScreenDayOrNightHandle(HMICOMTASK_MAG *pMag, u8 IsNight)
{
		FRAME_BUF *pFrame = NULL;
    uint16_t chkSum;

		assert_param(pMag != NULL);

		if(!IsNight)
		{
			pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

			if(pFrame == NULL)
			{
					return;
			}
		
			memset((void *)pFrame, 0, sizeof(FRAME_BUF));

			pFrame->m_wBufLen = 0;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_START;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI_BACKLIGHTBRIGHTNESS_CTRL;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x0F;
			chkSum = Rx_CRC_CCITT(&pFrame->m_byBuf[1], 0 + 2);
			
			pFrame->m_byBuf[pFrame->m_wBufLen++] = (chkSum >> 8) & 0xFF;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = chkSum & 0xFF;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xE4;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x1B;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x11;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xEE;

			pFrame->m_dwID = (pFrame->m_byBuf[1] << 8) | pFrame->m_byBuf[2];
			pFrame->m_dwCnt = 1;
			pFrame->m_dwScheTime = 1000;

			SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void *)pFrame);
		}
		else
		{
			pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

			if(pFrame == NULL)
			{
					return;
			}
		
			memset((void *)pFrame, 0, sizeof(FRAME_BUF));

			pFrame->m_wBufLen = 0;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI268B_FRAME_START;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = HMI_BACKLIGHTBRIGHTNESS_CTRL;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
			chkSum = Rx_CRC_CCITT(&pFrame->m_byBuf[1], 0 + 2);
			
			pFrame->m_byBuf[pFrame->m_wBufLen++] = (chkSum >> 8) & 0xFF;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = chkSum & 0xFF;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xE4;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x1B;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x11;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xEE;

			pFrame->m_dwID = (pFrame->m_byBuf[1] << 8) | pFrame->m_byBuf[2];
			pFrame->m_dwCnt = 1;
			pFrame->m_dwScheTime = 1000;

			SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void *)pFrame);
		
		}
}

void HMI_DayOrNight(HMICOMTASK_MAG *pMag)
{
		if(g_sUserParam.m_IsNight != pMag->m_byLastIsNight )
    {
        if(0 == g_sUserParam.m_IsNight)
        {
						HmiCom_Prot_PicShow(pMag, HMI_SUN_PIC, 1, 100);
            LCDScreenDayOrNightHandle(pMag, FALSE);
        }
        else
        {
						HmiCom_Prot_PicShow(pMag, HMI_MOON_PIC, 1, 100);
            LCDScreenDayOrNightHandle(pMag, TRUE);
        }

        //更新最后一次的数据
        pMag->m_byLastIsNight = g_sUserParam.m_IsNight;
    }
}

void HMI_ID(HMICOMTASK_MAG *pMag)
{
    uint8_t personID[HMIBUFLEN] = {0};

    if(g_sIDParam.m_wPid != pMag->m_wLastPID)
    {
        if(g_sIDParam.m_wPid > 61000)
        {
            sprintf((char*)personID, " %04d", g_sIDParam.m_wPid % 1000);
        }
        else
        {
            sprintf((char*)personID, " %04d", g_sIDParam.m_wPid % 10000);
        }

        HmiCom_Prot_Text1Show(pMag, HMI_PID_TXT, personID, strlen((char *)personID), 1, 1000);
				
        pMag->m_wLastPID = g_sIDParam.m_wPid;
    }
}

void HMI_Belong(HMICOMTASK_MAG *pMag)
{
    if(UserParam_GetBRChar() != pMag->m_byLastBelong)
    {
        if(UserParam_GetBRChar() == 0)
        {
						HmiCom_Prot_PicShow(pMag, HMI_BELONGBACKGROUND_PIC, 1, 100);
						HmiCom_Prot_PicShow(pMag, HMI_BLUEBELONG_PIC, 1, 100);
        }
        else if(UserParam_GetBRChar() == 1)
        {
						HmiCom_Prot_PicShow(pMag, HMI_BELONGBACKGROUND_PIC, 1, 100);
						HmiCom_Prot_PicShow(pMag, HMI_REDBELONG_PIC, 1, 100);
        }
        else if(UserParam_GetBRChar() == 2)
        {
						HmiCom_Prot_PicShow(pMag, HMI_BELONGBACKGROUND_PIC, 1, 100);
						HmiCom_Prot_PicShow(pMag, HMI_YELLOWBELONG_PIC, 1, 100);
        }
        else if(UserParam_GetBRChar() == 3)
        {
						HmiCom_Prot_PicShow(pMag, HMI_BELONGBACKGROUND_PIC, 1, 100);
						HmiCom_Prot_PicShow(pMag, HMI_WHITEBELONG_PIC, 1, 100);
        }

        pMag->m_byLastBelong = UserParam_GetBRChar();
    }
}

WpStr GetOwnBulletTypeString(HMICOMTASK_MAG *pMag)
{
    static char* ret[3] = {NULL};

    for(uint8_t i = 0; i < 3; i++)
    {
        switch(g_sUserParam.m_saAmmuInfo[i].m_sBulletInfo[0].m_byBullet_Type)
        {
            case 0x01:
                ret[i] = Subtype_PTZD[g_sUserParam.m_saAmmuInfo[i].m_sBulletInfo[0].m_byBullet_SubType];
                break;

            case 0x02:
                ret[i] = Subtype_CJD[g_sUserParam.m_saAmmuInfo[i].m_sBulletInfo[0].m_byBullet_SubType];
                break;

            case 0x03:
                ret[i] = Subtype_CJRSD[g_sUserParam.m_saAmmuInfo[i].m_sBulletInfo[0].m_byBullet_SubType];
                break;

            case 0x04:
                ret[i] = Subtype_RSD[g_sUserParam.m_saAmmuInfo[i].m_sBulletInfo[0].m_byBullet_SubType];
                break;

            case 0x07:
                ret[i] = Subtype_XD[g_sUserParam.m_saAmmuInfo[i].m_sBulletInfo[0].m_byBullet_SubType];
                break;

            case 0x10:
                ret[i] = Subtype_SSLD[g_sUserParam.m_saAmmuInfo[i].m_sBulletInfo[0].m_byBullet_SubType];
                break;

            case 0x16:
                ret[i] = Subtype_YBD[g_sUserParam.m_saAmmuInfo[i].m_sBulletInfo[0].m_byBullet_SubType];
                break;

            case 0x17:
                ret[i] = Subtype_SSBPRSD[g_sUserParam.m_saAmmuInfo[i].m_sBulletInfo[0].m_byBullet_SubType];
                break;

            case 0x18:
                ret[i] = Subtype_SSBPLD[g_sUserParam.m_saAmmuInfo[i].m_sBulletInfo[0].m_byBullet_SubType];
                break;

            case 0x1A:
                ret[i] = Subtype_TKCJD[g_sUserParam.m_saAmmuInfo[i].m_sBulletInfo[0].m_byBullet_SubType];
                break;

            default:
                ret[i] = "NULL";
                break;
        }
    }

    return &ret;
}

char* GetEnemyWeaponTypeStrig(uint8_t TType)
{
    char* string = NULL;

    switch(TType)
    {
        case 0x01:
        case 0x02:
            string = "TK";
            break;

        case 0x03:
            string = "BZC";
            break;

        case 0x04:
            string = "ZJSSH";
            break;

        case 0x05:
            string = "TJC";
            break;

        case 0x06:
            string = "TJP";
            break;

        case 0x07:
            string = "FTKDD";
            break;

        case 0x10:
            string = "QYHP";
            break;

        case 0x11:
        case 0x12:
            string = "ZXHP";
            break;

        case 0x13:
            string = "HJP";
            break;

        case 0x14:
            string = "PJP";
            break;

        case 0x20:
        case 0x21:
            string = "GP";
            break;

        case 0x22:
            string = "FKDD";
            break;

        case 0x25:
            string = "FKDD";
            break;

        case 0x30:
            string = "RY";
            break;

        case 0x31:
            string = "ZDBQ";
            break;

        case 0x32:
            string = "BYJQ";
            break;

        case 0x33:
            string = "JJBQ";
            break;

        case 0x34:
            string = "WSCFQ";
            break;

        case 0x35:
            string = "XDQ";
            break;

        case 0x36:
            string = "SQ";
            break;

        case 0x37:
            string = "LDFSQ";
            break;

        case 0x38:
            string = "ZJQ";
            break;

        case 0x39:
            string = "GSJQ";
            break;

        case 0x40:
            string = "DBHJ";
            break;

        case 0x41:
            string = "FTKHJ";
            break;

        case 0x42:
            string = "FTKDD";
            break;

        case 0x43:
            string = "DL";
            break;

        case 0x45:
            string = "SL";
            break;

        case 0x46:
            string = "JYBZW";
            break;

        case 0x48:
            string = "TLSB";
            break;

        case 0x50:
            string = "TYCL";
            break;

        case 0x51:
            string = "ZHCL";
            break;

        case 0x52:
            string = "TXCL";
            break;

        case 0x53:
            string = "DYSSC";
            break;

        case 0x54:
            string = "YLYSC";
            break;

        case 0x55:
            string = "HJBLC";
            break;

        case 0x56:
            string = "GCZCC";
            break;

        case 0x57:
            string = "QXPZC";
            break;

        case 0x58:
            string = "ZHSLC";
            break;

        case 0x5A:
            string = "QXCL";
            break;

        case 0x5B:
            string = "GCZYCL";
            break;

        case 0x60:
            string = "ZSJ";
            break;

        case 0x61:
            string = "WRJ";
            break;

        case 0x62:
            string = "ZDJ";
            break;

        default:
            string = "NULL";
            break;
    }

    return string;

}

void HMI_ButtonMsgHandle(HMICOMTASK_MAG *pMag)
{
    WpStr str = NULL;
    uint8_t RES1[HMIBUFLEN] = "NULL";
    uint8_t RES2[HMIBUFLEN] = "NULL";
    uint8_t i, Pic;
	
    char *HurtInfo = NULL;

    static uint8_t Cnt = 0;
    static uint8_t Background_DispFlg = 0;

    if(pMag->m_byButtonMsgFlg == TRUE)
    {
				if(Background_DispFlg == 0)
				{
					//底图显示
					HmiCom_Prot_PicShow(pMag, HMI_BUTTONMSG_PIC, 3, 100);
					Background_DispFlg = 1;
				}
        //显示文本控件
        str = GetOwnBulletTypeString(pMag);

				HmiCom_Prot_Text0Show(pMag, HMI_BUTTONMSG1_TXT, (uint8_t *)(*str)[0], strlen((char *)(*str)[0]), 2, 250);
        HmiCom_Prot_Text0Show(pMag, HMI_BUTTONMSG2_TXT, (uint8_t *)(*str)[1], strlen((char *)(*str)[1]), 2, 250);
        HmiCom_Prot_Text0Show(pMag, HMI_BUTTONMSG3_TXT, (uint8_t *)(*str)[2], strlen((char *)(*str)[2]), 2, 250);

				if(g_sHmiComTask_Mag.m_byHurtCountFlg == 0)
					HurtInfo = GetEnemyWeaponTypeStrig(pMag->m_sInjury[g_sHmiComTask_Mag.m_byHurtCountFlg].m_Ttype);
				else
					HurtInfo = GetEnemyWeaponTypeStrig(pMag->m_sInjury[g_sHmiComTask_Mag.m_byHurtCountFlg - 1].m_Ttype);
				HmiCom_Prot_Text0Show(pMag, HMI_BUTTONMSG4_TXT, (uint8_t *)HurtInfo, strlen((const char*)HurtInfo), 2, 250);
            
        //后两个控件分时复用,显示蓝牙和探头个数
        if(TRUE == pMag->m_byShowInfoPolling)
        {
            sprintf((char*)RES1, "Z:%02x%02x%02x%02x", g_sUserParam.m_aWMac[0], g_sUserParam.m_aWMac[1], g_sUserParam.m_aWMac[2],g_sUserParam.m_aWMac[3]);
            HmiCom_Prot_Text0Show(pMag, HMI_RES1_TXT, RES1, strlen((char *)RES1), 2, 250);
            sprintf((char*)RES2, "Z:%02x%02x%02x%02x", g_sUserParam.m_aWMac[4], g_sUserParam.m_aWMac[5], g_sUserParam.m_aWMac[6],g_sUserParam.m_aWMac[7]);
            HmiCom_Prot_Text0Show(pMag, HMI_RES2_TXT, RES2, strlen((char *)RES2), 2, 250);
				}
        else
        {
						sprintf((char*)RES1, "ZPID:%d", IDParam_GetPid());
            HmiCom_Prot_Text0Show(pMag, HMI_RES1_TXT, RES1, strlen((char *)RES1), 1, 250);
            
            sprintf((char*)RES2, "C:%d|%d", UserParam_GetActLaserCnt(), 10);		//UserParam_GetActLaserCnt1()
            HmiCom_Prot_Text0Show(pMag, HMI_RES2_TXT, RES2, strlen((char *)RES2), 1, 250);
				}
				
				if(Cnt++ >= 15)
        {
						Cnt = 0;
            pMag->m_byButtonMsgFlg = FALSE;
        }

    }
    else// if(pMag->m_byButtonMsgFlg == FALSE && LastButtonMsgFlg == TRUE)
    {
				HMI_HelmetConnect(pMag);
				Background_DispFlg = 0;
				Cnt = 0;
    }
}

void HMI_GetInjury(HMICOMTASK_MAG* pMag, u8 Part, u8 TType, u8 TSubtype)
{
		pMag->m_sInjury[g_sHmiComTask_Mag.m_byHurtCountFlg].m_byPart = Part;
    pMag->m_sInjury[g_sHmiComTask_Mag.m_byHurtCountFlg].m_Ttype = TType;
    pMag->m_sInjury[g_sHmiComTask_Mag.m_byHurtCountFlg].m_Tsubtype = TSubtype;
    g_sHmiComTask_Mag.m_byHurtCountFlg++;

    if(g_sHmiComTask_Mag.m_byHurtCountFlg >= HMI_INJURYLEN)
    {
        g_sHmiComTask_Mag.m_byHurtCountFlg = 0;
    }

    pMag->m_byHurtfinishFlg = TRUE;
}
/****************************************************************
@FUNCTION：HMI 串口功能定时任务
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void HmiCom_TimedTask(HMICOMTASK_MAG *pMag)
{
    uint32_t NowTime = GetSysRunMilliSecond();
	
		LCDScreenSleepHandle(pMag);
	
		if(pMag->m_byDispalyOnFlg == FALSE)
			return;
		
		if(pMag->m_byFirstPowerOn == FALSE)
		{
				HmiCom_Prot_CheckOnline(pMag, 1, 100);
		}
		
		if((NowTime - pMag->m_dwLastInfoShowTime) >= 1000)
    {
				HMI_ButtonMsgHandle(pMag);
				HMI_WeaponInfo(pMag);
        HMI_Location(pMag);

				pMag->m_dwLastInfoShowTime = NowTime;
    }
		
		if((NowTime - pMag->m_dwLastBaseInfoFlushTime) >= 5000)
		{
				HMI_ID(pMag);		//ID
				HMI_Belong(pMag);		//属方
				HMI_Mode(pMag);
				HMI_DayOrNight(pMag);		//昼夜图标
				HMI_PosPic(pMag);

				HMI_Damage(pMag);		//血量
				
				HMI_UctTime(pMag);
				HMI_Signal(pMag);
        HMI_UnUpLoadEvtNum(pMag);		//未上传事件数、方案编号
			
				pMag->m_dwLastBaseInfoFlushTime = NowTime;
		}

    //初次上电显示下电量，后面间隔1分钟更新一下
    if((NowTime - pMag->m_dwLastBatteryShowTime) >= 10000)
    {
        HMI_Battery(pMag);
				pMag->m_dwLastBatteryShowTime = NowTime;
    }
						
    if((NowTime - pMag->m_dwLastCheckTime) >=  60000)
    {
        HmiCom_Prot_CheckOnline(pMag, 1, 1000);
        pMag->m_dwLastCheckTime = NowTime;
    }

}

void HMI_Show_Init(void)
{
	HMICOMTASK_MAG *pMag = HmiComTask_HandleGet();	
	uint8_t Str[HMIBUFLEN] = {0};

	HMI_HelmetConnect(pMag);
	
	HMI_PosPic(pMag);
	HMI_Location(pMag);
	
	HMI_Signal(pMag);
	HMI_UnUpLoadEvtNum(pMag);		//未上传事件数、方案编号

	HMI_Mode(pMag);

	HMI_DayOrNight(pMag);		//昼夜图标
	HMI_Damage(pMag);		//血量
	
	//PID
	if(g_sIDParam.m_wPid > 61000)
	{
			sprintf((char*)Str, " %04d", g_sIDParam.m_wPid % 1000);
	}
	else
	{
			sprintf((char*)Str, " %04d", g_sIDParam.m_wPid % 10000);
	}

  HmiCom_Prot_Text1Show(pMag, HMI_PID_TXT, Str, strlen((char *)Str), 1, 1000);
	pMag->m_wLastPID = g_sIDParam.m_wPid;
	
	HMI_Belong(pMag);		//属方
	
	HmiCom_Prot_BUTTONShow(pMag, HMI_BUTTON, 1, 100);
}
/****************************************************************
@FUNCTION：HMI 串口串口协议处理
@INPUT：
    @param pBuf - 数据内容
    @param BufLen - 数据内容长度
    @param pHandle - 处理句柄
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void HmiCom_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    HMICOMTASK_MAG *pMag = (HMICOMTASK_MAG *)pHandle;

    switch(pBuf[1])
    {
        case HMI268B_FRAME_CMD_CHKON:
            pMag->m_bIsOnline = TRUE;
						pMag->m_byFirstPowerOn = TRUE;
						HMI_Show_Init();
            SComTask_SendListDeleteByFrameId(pMag->m_sTaskMag.m_pSendList, (pBuf[1] << 8) | pBuf[2]);
            break;

				case HMI_BUTTONCMD_SHOW:		//AA A0 00 30 8B CF E4 1B 11 EE
					  if(0x00 == pBuf[2] && 0x30 == pBuf[3])
						{
							pMag->m_byButtonMsgFlg = (pMag->m_byButtonMsgFlg == 0 ? 1 : 0);		//每次交替取1/0
							pMag->m_byShowInfoPolling = (pMag->m_byShowInfoPolling == 0 ? 1 : 0);	
						}
						break;

        default:
            break;
    }


}


