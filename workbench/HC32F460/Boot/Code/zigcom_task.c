/********************************************************************************
* @File name: zigcom_task.c
* @Author: xfw
* @Version: 1.0
* @Date: 2023-3-8
* @Description:
********************************************************************************/

#include "zigcom_task.h"
#include "protocal.h"

/*************�ײ�ʵ��************************************************/
//���ڳ�ʼ��
void ZigCom_ComInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[ZIG_COM];

    memcpy((void *)&Config, (void *)&g_scUartDefConfig[ZIG_COM], sizeof(UART_CONFIG));
    Config.m_dwBaudRate = ZIG_COM_BAUDRATE;
    //��Ӧ��������
    Uart_ComConfiguration(Config, pUartMag);
    Uart_ComInit(pUartMag);

    Zigbee_GpioResetInit();
}

//����д����
uint32_t ZigCom_ComWrite(uint8_t *pBuf, uint16_t Len)
{
    #ifdef ZIG_COM_LOG_OUTPUT

    LOG("ZIGCOM(%d)->", Len);
    printf_with_data(pBuf, Len);

    #endif

    return Uart_SendBuf(&g_sUartSetMag.Com[ZIG_COM], pBuf, Len);
}

//���ڶ�����
uint32_t ZigCom_ComRead(uint8_t *pBuf, uint16_t *pLen)
{
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[ZIG_COM];

    /**/
    if(TRUE == pUartMag->m_bIsComRecv)
    {
        if(pUartMag->m_wRecvLen == 0)
        {
            //pUartMag->m_bIsComRecv = FALSE;
            return FALSE;
        }

        #ifdef ZIG_COM_LOG_OUTPUT

        LOG("ZIGCOM(%d)<-", pUartMag->m_wRecvLen);
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


ZIGCOMTASK_MAG g_sZigComTask_Mag;

/****************************************************************
@FUNCTION��zigbee ���ڹ��ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void ZigComTask_Init(void)
{
    ZIGCOMTASK_MAG *pMag = &g_sZigComTask_Mag;

    memset((void *)pMag, 0, sizeof(ZIGCOMTASK_MAG));

    COM_HW_OPERATIONS HwOps =
    {
        .hw_init = ZigCom_ComInit,
        .read = ZigCom_ComRead,
        .write = ZigCom_ComWrite,
        .gpio_set = Zigbee_GpioResetOff,
        .gpio_reset = Zigbee_GpioResetOn,
    };

    NewZigbee_LowLevelInit(HwOps);

    pMag->m_pLLMag = NewZigbee_GetLLMagHandle();

    ComTask_Init(&pMag->m_sTaskMag, ZIG_COM_FRAME_LEN_MAX, 100);

    //�κη����ڴ�ʧ�ܣ����������ʼ��ʧ��
    pMag->m_bIsInit = ((pMag->m_sTaskMag.m_pProtBuf == NULL || pMag->m_sTaskMag.m_pSendQueue == NULL) ? FALSE : TRUE);
    //��λģ��
    NewZigbee_Reset(5);

    //��ȡģ�����
    NewZigbee_StartGetParam();

    //����ģ�����
    NewZigbee_SetTxPwr(ZIGBEE_TXPWR_10DB);
}


/****************************************************************
@FUNCTION��zigbee ���ڹ��ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void ZigComTask_DeInit(void)
{
    ZIGCOMTASK_MAG *pMag = &g_sZigComTask_Mag;

    ComTask_DeInit(&pMag->m_sTaskMag);

    memset((void *)pMag, 0, sizeof(ZIGCOMTASK_MAG));
}

uint32_t ZigRetSend(uint8_t *pBuf, uint16_t Len)
{
    ZIGCOMTASK_MAG *pMag = &g_sZigComTask_Mag;

    return NewZigbee_Send(pMag->m_aLastCommMacBuf, 8, pBuf, Len);
}

//��zigbeeģ��Э�鴦��
void ZigCom_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    ZIGCOMTASK_MAG *pMag = (ZIGCOMTASK_MAG *)pHandle;
    COMTASK_MAG *pComTaskMag = &pMag->m_sTaskMag;
    uint16_t TmpLen;
    uint16_t Offset, RecvLen, DatOffset, DataLen;
    uint32_t IsCutted = FALSE, IsLong55AA = FALSE;

    switch(pBuf[2])
    {
        //����ָ��
        case 0xFC:

            //��ȡ�����ذ�
            if(pBuf[5] == 0x0F && BufLen >= 38)
            {
                NewZigbee_StopGetParam();

                memcpy((void *)&pMag->m_pLLMag->m_sParam, &pBuf[9], 28);

                Update_SetDevMac(pMag->m_pLLMag->m_sParam.m_aSelfExtAddr, 8);
            }
            //���ò����ذ�
            else if(pBuf[5] == 0x0E && BufLen >= 28)
            {
                NewZigbee_StopSetParam();
                NewZigbee_StartSoftReset();
            }
            //��λ����Ҫ���»�ȡ����
            else if(pBuf[5] == 0x87 && BufLen >= 10)
            {
                NewZigbee_StopSoftReset();
                NewZigbee_StartGetParam();
            }

            break;

        //���ݴ���ָ��
        case 0xFD:
            Offset = 3;

            if(pBuf[Offset] > 1)
            {
                break;
            }

            pMag->m_byLastCommMode = pBuf[Offset++];
            TmpLen = (pMag->m_byLastCommMode == 0 ? 23 : 17);

            //��Ե�
            if(BufLen > TmpLen)
            {

                //���ݳ���
                RecvLen = pBuf[Offset++];

                if(RecvLen != BufLen - TmpLen)
                {
                    break;
                }

                //ģ����չ��ַ
                Offset += (pMag->m_byLastCommMode == 0 ? 8 : 2);

                //��¼Э����
                DatOffset = Offset;
                pMag->m_bIsRecvNew = TRUE;


                Offset += RecvLen;
                memcpy((void *)pMag->m_aLastCommMacBuf, (void *)&pBuf[Offset], 8);
                Offset += 8;
                pMag->m_byLastCommRSSI = pBuf[Offset];
            }

            //�����غɲ���Э�����
            if(TRUE == pMag->m_bIsRecvNew)
            {
                pMag->m_bIsRecvNew = FALSE;

                for(Offset = DatOffset; Offset < RecvLen + DatOffset;)
                {
                    TmpLen = RecvLen + DatOffset - Offset;

                    if(TRUE == Update_ProtocalParse(&pBuf[Offset], &TmpLen, &IsCutted, &IsLong55AA))
                    {
                        if(TRUE == IsCutted)
                        {
                            Offset++;
                            continue;//�������Э�鲻�������򲻹ܣ�����������
                        }
                        else
                        {

                            Update_ConfigSendFunc(ZigRetSend);

                            //����Ϊ55AA����
                            if (IsLong55AA)
                            {
                                DataLen	= pComTaskMag->m_pProtBuf[Offset + 3] | (pComTaskMag->m_pProtBuf[Offset + 4] << 8);

                                //���˵���ع㲥ָ��
                                if((pMag->m_byLastCommMode == 0)
                                        || ((pMag->m_byLastCommMode == 1 ) && (pComTaskMag->m_pProtBuf[Offset + 7] == 0xF0))
                                        || ((pMag->m_byLastCommMode == 1 ) && (pComTaskMag->m_pProtBuf[Offset + 7] == 0xF2)))
                                {
                                    Update_ProtocalProcess(&pComTaskMag->m_pProtBuf[Offset + 7], DataLen);
                                }

                            }
                            else
                            {
                                //���˵���ع㲥ָ��
                                if((pMag->m_byLastCommMode == 0)
                                        || ((pMag->m_byLastCommMode == 1 ) && (pComTaskMag->m_pProtBuf[Offset + 3] == 0xF0))
                                        || ((pMag->m_byLastCommMode == 1 ) && (pComTaskMag->m_pProtBuf[Offset + 3] == 0xF2)))
                                {
                                    //LOG("parse succ,i=%d,Len=%d\n\r",i,Len);
                                    Update_ProtocalProcess(&pComTaskMag->m_pProtBuf[Offset + 3], pComTaskMag->m_pProtBuf[Offset + 2]);

                                }
                            }

                            Offset = TmpLen + Offset;
                        }
                    }

                    else
                    {
                        Offset++;
                    }

                }
            }

            break;

        default:
            break;
    }


}


/****************************************************************
@FUNCTION��zigbee ���ڹ�������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void ZigComTask_Process(void)
{
    ZIGCOMTASK_MAG *pMag = &g_sZigComTask_Mag;
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
        if(FALSE == NewZigbee_LowLevelTimedProcess())
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

    }

    //���ڽ��մ���
    if(TRUE == pMag->m_pLLMag->m_sHwOps.read(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], &Len))
    {
        //ת�����ݵ����п�
        //ResComTask_RecvSendToUpcom(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], Len, 0x0A);

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
                    ZigCom_Analysis(&pComTaskMag->m_pProtBuf[i], Len, (void *)pMag);

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


