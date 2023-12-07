/********************************************************************************
* @File name: rescom_laser.c
* @Author: xfw
* @Version: 1.0
* @Date: 2023-3-8
* @Description:
********************************************************************************/

#include "upcom_task.h"

//���Կڳ�ʼ��
void UpCom_ComInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[UP_COM];

    memcpy((void *)&Config, (void *)&g_scUartDefConfig[UP_COM], sizeof(UART_CONFIG));
    Config.m_dwBaudRate = UP_COM_BAUDRATE;
    //��Ӧ��������
    Uart_ComConfiguration(Config, pUartMag);
    Uart_ComInit(pUartMag);
}

//���Կ�д����
uint32_t UpCom_ComWrite(uint8_t *pBuf, uint16_t Len)
{
    #ifdef UP_COM_LOG_OUTPUT

    LOG("UPCOM(%d)->", Len);
    printf_with_data(pBuf, Len);

    #endif

    return Uart_SendBuf(&g_sUartSetMag.Com[UP_COM], pBuf, Len);
}

//���Կڶ�����
uint32_t UpCom_ComRead(uint8_t *pBuf, uint16_t *pLen)
{
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[UP_COM];

    /**/
    if(TRUE == pUartMag->m_bIsComRecv)
    {
        if(pUartMag->m_wRecvLen == 0)
        {
            //pUartMag->m_bIsComRecv = FALSE;
            return FALSE;
        }

        #ifdef UP_COM_LOG_OUTPUT

        LOG("UPCOM(%d)<-", pUartMag->m_wRecvLen);
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

UPCOMTASK_MAG g_sUpComTask_Mag;

static UPCOM_LOWLEVEL_MAG UpComLLMag;


UPCOM_LOWLEVEL_MAG *UpCom_GetLLMagHandle(void)
{
    return &UpComLLMag;
}


/****************************************************************
@FUNCTION�����д��ڹ��ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
void UpComTask_Init(void)
{
    UPCOMTASK_MAG *pMag = &g_sUpComTask_Mag;

    memset((void *)pMag, 0, sizeof(UPCOMTASK_MAG));

    pMag->m_pLLMag = UpCom_GetLLMagHandle();

    pMag->m_pLLMag->m_sHwOps.hw_init = UpCom_ComInit;
    pMag->m_pLLMag->m_sHwOps.read = UpCom_ComRead;
    pMag->m_pLLMag->m_sHwOps.write = UpCom_ComWrite;

    pMag->m_pLLMag->m_sHwOps.hw_init();

    ComTask_Init(&pMag->m_sTaskMag, UP_COM_FRAME_LEN_MAX, 100);

    //�κη����ڴ�ʧ�ܣ����������ʼ��ʧ��
    pMag->m_bIsInit = ((pMag->m_sTaskMag.m_pProtBuf == NULL || pMag->m_sTaskMag.m_pSendQueue == NULL) ? FALSE : TRUE);
}


/****************************************************************
@FUNCTION�����д��ڹ��ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
void UpComTask_DeInit(void)
{
    UPCOMTASK_MAG *pMag = &g_sUpComTask_Mag;

    ComTask_DeInit(&pMag->m_sTaskMag);

    memset((void *)pMag, 0, sizeof(UPCOMTASK_MAG));
}


/****************************************************************
@FUNCTION�����д��ڹ�������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
void UpComTask_Process(void)
{
    UPCOMTASK_MAG *pMag = &g_sUpComTask_Mag;
    COMTASK_MAG *pComTaskMag = &pMag->m_sTaskMag;
    uint16_t i, Len = pComTaskMag->m_wProtBufMaxLen - pComTaskMag->m_wProtBufLen;
    uint32_t DataLen, IsCutted = FALSE, IsLong55AA = FALSE;
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
        //if(FALSE == LaserCom_LowLevelTimedProcess())
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

    }

    //���ڽ��մ���
    if(TRUE == pMag->m_pLLMag->m_sHwOps.read(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], &Len))
    {
        //ת�����ݵ����п�
        //ResComTask_RecvSendToUpcom(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen],Len,0x06);

        pComTaskMag->m_wProtBufLen += Len;

        for(i = 0; i < pComTaskMag->m_wProtBufLen;)
        {
            Len = pComTaskMag->m_wProtBufLen - i;

            if(TRUE == Update_ProtocalParse(&pComTaskMag->m_pProtBuf[i], &Len, &IsCutted, &IsLong55AA))
            {
                if(TRUE == IsCutted)
                {
                    i++;
                    continue;//�������Э�鲻�������򲻹ܣ�����������
                }
                else
                {

                    Update_ConfigSendFunc(pMag->m_pLLMag->m_sHwOps.write);

                    //����Ϊ55AA����
                    if (IsLong55AA)
                    {
                        DataLen	= pComTaskMag->m_pProtBuf[i + 3] | (pComTaskMag->m_pProtBuf[i + 4] << 8);


                        Update_ProtocalProcess(&pComTaskMag->m_pProtBuf[i + 7], DataLen);


                    }
                    else
                    {
                        //LOG("parse succ,i=%d,Len=%d\n\r",i,Len);
                        Update_ProtocalProcess(&pComTaskMag->m_pProtBuf[i + 3], pComTaskMag->m_pProtBuf[i + 2]);

                    }

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
