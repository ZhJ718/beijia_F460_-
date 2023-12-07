/********************************************************************************
* @File name: rescom_laser.c
* @Author: xfw
* @Version: 1.0
* @Date: 2023-3-8
* @Description:
********************************************************************************/

#include "rescom_task.h"
#include "protocal.h"

#include "zigcom_task.h"
#include "gpscom_task.h"

#include "lasercom_task.h"

#include "ir38com_task.h"
//���Կڳ�ʼ��
void ResCom_ComInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[RES_COM];

    memcpy((void *)&Config, (void *)&g_scUartDefConfig[RES_COM], sizeof(UART_CONFIG));
    Config.m_dwBaudRate = RES_COM_BAUDRATE;
    //��Ӧ��������
    Uart_ComConfiguration(Config, pUartMag);
    Uart_ComInit(pUartMag);
}

//���Կ�д����
uint32_t ResCom_ComWrite(uint8_t *pBuf, uint16_t Len)
{
    #ifdef RES_COM_LOG_OUTPUT

    LOG("RESCOM(%d)->", Len);
    printf_with_data(pBuf, Len);

    #endif

    return Uart_SendBuf(&g_sUartSetMag.Com[RES_COM], pBuf, Len);
}

//���Կڶ�����
uint32_t ResCom_ComRead(uint8_t *pBuf, uint16_t *pLen)
{
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[RES_COM];

    /**/
    if(TRUE == pUartMag->m_bIsComRecv)
    {
        if(pUartMag->m_wRecvLen == 0)
        {
            //pUartMag->m_bIsComRecv = FALSE;
            return FALSE;
        }

        #ifdef RES_COM_LOG_OUTPUT

        LOG("RESCOM(%d)<-", pUartMag->m_wRecvLen);
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

RESCOMTASK_MAG g_sResComTask_Mag;

static RESCOM_LOWLEVEL_MAG ResComLLMag;


RESCOM_LOWLEVEL_MAG *ResCom_GetLLMagHandle(void)
{
    return &ResComLLMag;
}


/****************************************************************
@FUNCTION���������ڹ��ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_Init(void)
{
    RESCOMTASK_MAG *pMag = &g_sResComTask_Mag;

    memset((void *)pMag, 0, sizeof(RESCOMTASK_MAG));

    pMag->m_pLLMag = ResCom_GetLLMagHandle();

    pMag->m_pLLMag->m_sHwOps.hw_init = ResCom_ComInit;
    pMag->m_pLLMag->m_sHwOps.read = ResCom_ComRead;
    pMag->m_pLLMag->m_sHwOps.write = ResCom_ComWrite;

    pMag->m_pLLMag->m_sHwOps.hw_init();

    ComTask_Init(&pMag->m_sTaskMag, RES_COM_FRAME_LEN_MAX, 100);

    //�κη����ڴ�ʧ�ܣ����������ʼ��ʧ��
    pMag->m_bIsInit = ((pMag->m_sTaskMag.m_pProtBuf == NULL || pMag->m_sTaskMag.m_pSendQueue == NULL) ? FALSE : TRUE);
}


/****************************************************************
@FUNCTION���������ڹ��ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_DeInit(void)
{
    ComTask_DeInit(&g_sResComTask_Mag);
}


//���пڴ���Э�鴦��
void ResCom_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    RESCOMTASK_MAG *pMag = (RESCOMTASK_MAG *)pHandle;
    uint8_t DevIdx, DevCtrl, Offset;

    DevIdx = pBuf[1];
    DevCtrl = pBuf[2];


    switch(pBuf[0])
    {

        case 0x0:
            switch(DevIdx)
            {
                case 0x05:

                    ComTask_RetransmissionData(&g_sLaserComTask_Mag.m_sTaskMag, &pBuf[3], BufLen - 3);
                    break;

                case 0x07:
                    ComTask_RetransmissionData(&g_sIR38ComTask_Mag.m_sTaskMag, &pBuf[3], BufLen - 3);
                    break;

                case 0x0A:
                    ComTask_RetransmissionData(&g_sZigComTask_Mag.m_sTaskMag, &pBuf[3], BufLen - 3);
                    break;

                case 0x0B:
                    ComTask_RetransmissionData(&g_sGpsComTask_Mag.m_sTaskMag, &pBuf[3], BufLen - 3);
                    break;

                default:
                    break;
            }

            break;


        default:
            break;
    }


}


/****************************************************************
@FUNCTION���������ڹ�������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_Process(void)
{
    RESCOMTASK_MAG *pMag = &g_sResComTask_Mag;
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

                        //���˵�����ָ��
                        if(pComTaskMag->m_pProtBuf[i + 7] == 0x0)
                        {
                            ResCom_Analysis(&pComTaskMag->m_pProtBuf[i + 7], DataLen, (void *)pMag);
                        }
                        else
                        {
                            Update_ProtocalProcess(&pComTaskMag->m_pProtBuf[i + 7], DataLen);
                        }

                    }
                    else
                    {
                        //���˵�����ָ��
                        if(pComTaskMag->m_pProtBuf[i + 3] == 0x0)
                        {
                            ResCom_Analysis(&pComTaskMag->m_pProtBuf[i + 3], pComTaskMag->m_pProtBuf[i + 2], (void *)pMag);
                        }
                        else
                        {
                            //LOG("parse succ,i=%d,Len=%d\n\r",i,Len);
                            Update_ProtocalProcess(&pComTaskMag->m_pProtBuf[i + 3], pComTaskMag->m_pProtBuf[i + 2]);

                        }
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


void ResComTask_RecvSendToUpcom(uint8_t *pBuf, uint16_t BufLen, uint8_t DevIdx)
{
    //uint8_t temp[256];
    //uint16_t len;
    FRAME_BUF *pFrame = NULL;

    assert_param(pMag != NULL);

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    if(BufLen < 250)
    {
        pFrame->m_wBufLen = 0;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x55;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = GetProtAddr();
        pFrame->m_byBuf[pFrame->m_wBufLen++] = 3 + BufLen;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = PROTOCAL_CMD_BOARD_CTRL;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = DevIdx;//Zigbee
        pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x01;//R
        memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], pBuf, BufLen);
        pFrame->m_wBufLen += BufLen;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = Protocal_MakeU8Sum(&pFrame->m_byBuf[3], 3 + BufLen);
        pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xAA;
        LinkQueue_Append(g_sResComTask_Mag.m_sTaskMag.m_pSendQueue, (void*)pFrame);
    }

}
