/********************************************************************************
* @File name: lasercom_task.c
* @Author: xfw
* @Version: 1.0
* @Date: 2023-4-4
* @Description:
********************************************************************************/

#include "lasercom_task.h"

/*************�ײ�ʵ��************************************************/
//���ڳ�ʼ��
void LaserCom_ComInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[LASER_COM];

    memcpy((void *)&Config, (void *)&g_scUartDefConfig[LASER_COM], sizeof(UART_CONFIG));
    Config.m_dwBaudRate = LASER_COM_BAUDRATE;
    //��Ӧ��������
    Uart_ComConfiguration(Config, pUartMag);
    Uart_ComInit(pUartMag);

    LaserCom_GpioPowerOnInit();
}

//����д����
uint32_t LaserCom_ComWrite(uint8_t *pBuf, uint16_t Len)
{
    #ifdef LASER_COM_LOG_OUTPUT

    LOG("LASERCOM(%d)->", Len);
    printf_with_data(pBuf, Len);

    #endif

    return Uart_SendBuf(&g_sUartSetMag.Com[LASER_COM], pBuf, Len);
}

//���ڶ�����
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
            //����������,��������������
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, *pLen);
        }

        pUartMag->m_wRecvLen = 0;
        pUartMag->m_bIsComRecv = FALSE;

        return TRUE;
    }

    return FALSE;
}

/*************�ײ�ʵ�ֽ���************************************************/


/****************************************************************
@FUNCTION����ȡ̽ͷ������̽ͷ��
@INPUT��
    @param  MaxCnt - ���ܵ����̽ͷ��
    @param  IntVal - ����̽ͷ������ʱ����λ10ms����Ϊ0,��ȡĬ��ֵ10
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���ظ�������ģ���������MaxCnt����IntValֵ
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
    //���Э�鳤��
    pFrame->m_byBuf[1] = pFrame->m_wBufLen;

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);

}

/****************************************************************
@FUNCTION��LASER����ģʽ�л�����
@INPUT��LASER�ײ����ṹ��ָ�룬ģʽ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:��
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
    //���Э�鳤��
    pFrame->m_byBuf[1] = pFrame->m_wBufLen;

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);

}


LASERCOMTASK_MAG g_sLaserComTask_Mag;

/****************************************************************
@FUNCTION��������մ��ڹ��ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
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

    //�ӿ��ϵ�
    LaserCom_PowerOn();

    //��ȡ����̽ͷ��
    LaserCom_Prot_GetOnlineNum(pMag, 0x0A, 0x0A);
}


/****************************************************************
@FUNCTION��������մ��ڹ��ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_DeInit(void)
{
    LASERCOMTASK_MAG *pMag = &g_sLaserComTask_Mag;

    ComTask_DeInit(&pMag->m_sTaskMag);

    memset((void *)pMag, 0, sizeof(LASERCOMTASK_MAG));
}




//������մ���Э�鴦��
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
@FUNCTION��������մ��ڹ�������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
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

    //��ʱ����
    if((NowTime >  pComTaskMag->m_dwLastScheduleTime) &&
            (NowTime -  pComTaskMag->m_dwLastScheduleTime >= pComTaskMag->m_dwScheduleTime) )
    {
        pComTaskMag->m_dwLastScheduleTime = NowTime;

        //���ȱ�֤�ײ����
        if(FALSE == LaserCom_LowLevelTimedProcess())
        {
            //��鷢�Ͷ���
            pFrame = LinkQueue_Header(pComTaskMag->m_pSendQueue);

            if(pFrame != NULL)
            {
                pMag->m_pLLMag->m_sHwOps.write(pFrame->m_byBuf, pFrame->m_wBufLen);
                LinkQueue_Retrieve(pComTaskMag->m_pSendQueue);
                sdram_free(0, (void * )pFrame);
            }
        }

        /***********Ӧ���߼�***************************/
        // 1. ���50s�˿�û�յ����ݣ����ȡ����̽ͷ��
        if((NowTime - pComTaskMag->m_dwLastCommTime) >= 50000)
        {
            LaserCom_Prot_GetOnlineNum(pMag, 0x0A, 0x0A);
            pComTaskMag->m_dwLastCommTime = NowTime;
        }
    }

    //���ڽ��մ���
    if(TRUE == pMag->m_pLLMag->m_sHwOps.read(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], &Len))
    {
        //ת�����ݵ����п�
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



