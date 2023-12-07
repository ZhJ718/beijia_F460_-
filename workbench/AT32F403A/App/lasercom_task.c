/********************************************************************************
* @File name: lasercom_task.c
* @Author: xfw
* @Version: 1.0
* @Date: 2023-4-4
* @Description:
********************************************************************************/

#include "lasercom_task.h"

/*************底层实现************************************************/
//串口初始化
void LaserCom_ComInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[LASER_COM];

    memcpy((void *)&Config, (void *)&g_scUartDefConfig[LASER_COM], sizeof(UART_CONFIG));
    Config.m_dwBaudRate = LASER_COM_BAUDRATE;
    //对应串口配置
    Uart_ComConfiguration(Config, pUartMag);
    Uart_ComInit(pUartMag);

    LaserCom_GpioPowerOnInit();
}

//串口写数据
uint32_t LaserCom_ComWrite(uint8_t *pBuf, uint16_t Len)
{
    #ifdef LASER_COM_LOG_OUTPUT

    LOG("LASERCOM(%d)->", Len);
    printf_with_data(pBuf, Len);

    #endif

    return Uart_SendBuf(&g_sUartSetMag.Com[LASER_COM], pBuf, Len);
}

//串口读数据
uint32_t LaserCom_ComRead(uint8_t *pBuf, uint16_t *pLen)
{
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[LASER_COM];

    /**/
    if(TRUE == pUartMag->m_bIsComRecv)
    {
        if(pUartMag->m_wRecvLen == 0)
        {
            //pUartMag->m_bIsComRecv = FALSE;
            return FALSE;
        }

        #ifdef LASER_COM_LOG_OUTPUT

        LOG("LASERCOM(%d)<-", pUartMag->m_wRecvLen);
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


/****************************************************************
@FUNCTION：获取探头带在线探头数
@INPUT：
    @param  MaxCnt - 可能的最大探头数
    @param  IntVal - 单个探头基础超时，单位10ms。若为0,则取默认值10
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若回复多个报文，可以增大MaxCnt或者IntVal值
****************************************************************/
void LaserCom_Prot_GetOnlineNum(LASERCOMTASK_MAG *pMag, uint8_t MaxCnt, uint8_t IntVal)
{
    FRAME_BUF *pFrame = NULL;

    assert_param(pMag != NULL);

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x5A;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xA0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = MaxCnt;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = (IntVal == 0 ? 0x0A : IntVal);
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xAA;
    //填充协议长度
    pFrame->m_byBuf[1] = pFrame->m_wBufLen;

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);

}

/****************************************************************
@FUNCTION：LASER发送模式切换命令
@INPUT：LASER底层管理结构体指针，模式
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:无
****************************************************************/
void LaserCom_Prot_CtrlMode(LASERCOMTASK_MAG *pMag, LASER_MODE a_Mod)
{
    FRAME_BUF *pFrame = NULL;

    assert_param(pMag != NULL);

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x5A;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xB1;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = (uint8_t)a_Mod;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xAA;
    //填充协议长度
    pFrame->m_byBuf[1] = pFrame->m_wBufLen;

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);

}


LASERCOMTASK_MAG g_sLaserComTask_Mag;

/****************************************************************
@FUNCTION：激光接收串口功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_Init(void)
{
    LASERCOMTASK_MAG *pMag = &g_sLaserComTask_Mag;

    memset((void *)pMag, 0, sizeof(LASERCOMTASK_MAG));

    COM_HW_OPERATIONS HwOps =
    {
        .hw_init = LaserCom_ComInit,
        .read = LaserCom_ComRead,
        .write = LaserCom_ComWrite,
        .gpio_power_on  = LaserCom_GpioPowerOn,
        .gpio_power_off = LaserCom_GpioPowerOff,
    };

    LaserCom_LowLevelInit(HwOps);

    pMag->m_pLLMag = LaserCom_GetLLMagHandle();

    pMag->m_bIsInit = ComTask_Init(&pMag->m_sTaskMag, LASER_COM_FRAME_LEN_MAX, 100);

    //接口上电
    LaserCom_PowerOn();

    //获取在线探头数
    LaserCom_Prot_GetOnlineNum(pMag, 0x0A, 0x0A);
}


/****************************************************************
@FUNCTION：激光接收串口功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_DeInit(void)
{
    LASERCOMTASK_MAG *pMag = &g_sLaserComTask_Mag;

    ComTask_DeInit(&pMag->m_sTaskMag);

    memset((void *)pMag, 0, sizeof(LASERCOMTASK_MAG));
}




//激光接收串口协议处理
void LaserCom_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    LASERCOMTASK_MAG *pMag = (LASERCOMTASK_MAG *)pHandle;
    uint8_t Offset;

    switch(pBuf[2])
    {

        case 0xA0:
            Offset = 4;
            pMag->m_byActualNum = pBuf[Offset];
            break;


        default:
            break;
    }


}



/****************************************************************
@FUNCTION：激光接收串口功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void LaserComTask_Process(void)
{
    LASERCOMTASK_MAG *pMag = &g_sLaserComTask_Mag;
    COMTASK_MAG *pComTaskMag = &pMag->m_sTaskMag;
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
        if(FALSE == LaserCom_LowLevelTimedProcess())
        {
            //检查发送队列
            pFrame = LinkQueue_Header(pComTaskMag->m_pSendQueue);

            if(pFrame != NULL)
            {
                pMag->m_pLLMag->m_sHwOps.write(pFrame->m_byBuf, pFrame->m_wBufLen);
                LinkQueue_Retrieve(pComTaskMag->m_pSendQueue);
                sdram_free(0, (void * )pFrame);
            }
        }

        /***********应用逻辑***************************/
        // 1. 如果50s端口没收到数据，则获取在线探头数
        if((NowTime - pComTaskMag->m_dwLastCommTime) >= 50000)
        {
            LaserCom_Prot_GetOnlineNum(pMag, 0x0A, 0x0A);
            pComTaskMag->m_dwLastCommTime = NowTime;
        }
    }

    //串口接收处理
    if(TRUE == pMag->m_pLLMag->m_sHwOps.read(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], &Len))
    {
        //转发数据到上行口
        ResComTask_RecvSendToUpcom(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], Len, 0x06);

        pComTaskMag->m_wProtBufLen += Len;

        for(i = 0; i < pComTaskMag->m_wProtBufLen;)
        {
            Len = pComTaskMag->m_wProtBufLen - i;

            if(TRUE == Protocal_Find5AAAFrame(&pComTaskMag->m_pProtBuf[i], &Len, &IsCutted))
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
                    LaserCom_Analysis(&pComTaskMag->m_pProtBuf[i], Len, (void *)pMag);

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


    }

}



