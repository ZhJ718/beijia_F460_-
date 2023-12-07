#include "excom.h"
#include "protocal.h"
#include "main_task.h"

EXCOM_MANAGE g_sExcomMag;


//���ӽ������ڳ�ʼ��
void ExChange_HwInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[EXCHAN_COM];

    memcpy((void *)&Config, (void *)&g_scUartDefConfig[EXCHAN_COM], sizeof(UART_CONFIG));
    //��Ӧ��������
    Config.m_dwBaudRate = EXCHAN_BAUDRATE;

    Uart_ComConfiguration(Config, pUartMag);
    Uart_ComInit(pUartMag);
}

uint32_t ExChange_HwWrite(uint8_t *pBuf, uint16_t Len)
{
    #ifdef EXCHANGE_COM_LOG_OUTPUT
    LOG("EXCHANGE_COM(%d)->", Len);
    printf_with_data(pBuf, Len);
    #endif

    return Uart_SendBuf(&g_sUartSetMag.Com[EXCHAN_COM], pBuf, Len);
}

uint32_t ExChange_HwRead(uint8_t *pBuf, uint16_t *pLen)
{
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[EXCHAN_COM];

    /**/
    if(TRUE == pUartMag->m_bIsComRecv)
    {
        if(pUartMag->m_wRecvLen == 0)
        {
            //pUartMag->m_bIsComRecv = FALSE;
            return FALSE;
        }

        #ifdef EXCHANGE_COM_LOG_OUTPUT
        LOG("EXCHANGE_COM(%d)<-", pUartMag->m_wRecvLen);
        printf_with_data(pUartMag->m_byRecvBuf, pUartMag->m_wRecvLen);
        #endif

        if(*pLen >= pUartMag->m_wRecvLen)
        {
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, pUartMag->m_wRecvLen);
            *pLen = pUartMag->m_wRecvLen;
        }
        else
        {
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, *pLen);
        }

        pUartMag->m_wRecvLen = 0;
        pUartMag->m_bIsComRecv = FALSE;

        return TRUE;
    }

    return FALSE;
}

void PrintExComMag(EXCOM_MANAGE *pMag)
{
    uint32_t i;

    LOG("\n\r@@@@@@@@@@@@@@ ��չ���ڹ���@@@@@@@@@@@@@@\n\r");

    for(i = 0; i < pMag->m_byUsedExUartCnt; i++)
    {
        LOG("EXCOM%d S:%d,R:%d\n\r", pMag->m_saExUartMag[i].m_sConfig.m_byComSeq + 1, \
            pMag->m_saExUartMag[i].m_dwSendCnt, pMag->m_saExUartMag[i].m_dwRecvCnt);
    }

    LOG("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\r");
}

/****************************************************************
@FUNCTION����ȡ���������ţ����������Ÿ��ݴ���ʵ��ʹ��������ı�
@INPUT��
	@@@ pMag - ������չ���ܹ���ָ��
	@@@ eComSeq -�������к�
@OUTPUT����
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint8_t ExCom_GetComIdx(EXCOM_MANAGE *pMag, ENUM_COM_EX_SEQ eComSeq)
{
    uint8_t i;


    for(i = 0; i < EXCOM_IDX_MAX; i++)
    {
        if(eComSeq == pMag->m_saExUartMag[i].m_sConfig.m_byComSeq)
        {
            break;
        }
    }

    return i;
}





void ExCom_LogOutput(uint8_t IsRead, ENUM_COM_EX_SEQ eExCom, uint8_t *pBuf, uint16_t Len)
{

    switch(eExCom)
    {
        case EXCOM1:
            #ifdef EX_COM1_LOG_OUTPUT
            LOG("EXCOM1(%d)%s", Len, IsRead ? "<-" : "->");
            printf_with_data(pBuf, Len);
            #endif
            break;

        case EXCOM2:
            #ifdef EX_COM2_LOG_OUTPUT
            LOG("EXCOM2(%d)%s", Len, IsRead ? "<-" : "->");
            printf_with_data(pBuf, Len);
            #endif
            break;

        case EXCOM3:
            #ifdef EX_COM3_LOG_OUTPUT
            LOG("EXCOM3(%d)%s", Len, IsRead ? "<-" : "->");
            printf_with_data(pBuf, Len);
            #endif
            break;

        case EXCOM4:
            #ifdef EX_COM4_LOG_OUTPUT
            LOG("EXCOM4(%d)%s", Len, IsRead ? "<-" : "->");
            printf_with_data(pBuf, Len);
            #endif
            break;

        case EXCOM5:
            #ifdef EX_COM5_LOG_OUTPUT
            LOG("EXCOM5(%d)%s", Len, IsRead ? "<-" : "->");
            printf_with_data(pBuf, Len);
            #endif
            break;

        case EXCOM6:
            #ifdef EX_COM6_LOG_OUTPUT
            LOG("EXCOM6(%d)%s", Len, IsRead ? "<-" : "->");
            printf_with_data(pBuf, Len);
            #endif
            break;

        case EXCOM7:
            #ifdef EX_COM7_LOG_OUTPUT
            LOG("EXCOM7(%d)%s", Len, IsRead ? "<-" : "->");
            printf_with_data(pBuf, Len);
            #endif
            break;

        case EXCOM8:
            #ifdef EX_COM8_LOG_OUTPUT
            LOG("EXCOM8(%d)%s", Len, IsRead ? "<-" : "->");
            printf_with_data(pBuf, Len);
            #endif
            break;
    }
}


/****************************************************************
@FUNCTION��������չ����
@INPUT��
	@@@ Cfg - ���ò���
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL: �����ݲ���Ч����ʱ�����з������ñ���
****************************************************************/
int32_t ExCom_ConfigUart(EXUART_CONFIG Cfg, EXCOM_MANAGE *pMag)
{
    uint8_t Idx = 0;


    //�ӵ�ǰ��չ�����б�����
    Idx = ExCom_GetComIdx(pMag, Cfg.m_byComSeq);

    //�ҵ�
    if(Idx < EXCOM_IDX_MAX)
    {
        memcpy((void *)&pMag->m_saExUartMag[Idx].m_sConfig, (void *)&Cfg, sizeof(EXUART_CONFIG));
        pMag->m_saExUartMag[Idx].m_bIsSendConfig = TRUE;
    }
    //û�ҵ������ǵ�ǰ�б��п�λ
    else if(pMag->m_byUsedExUartCnt < EXCOM_IDX_MAX)
    {
        memcpy((void *)&pMag->m_saExUartMag[pMag->m_byUsedExUartCnt].m_sConfig, (void *)&Cfg, sizeof(EXUART_CONFIG));
        pMag->m_saExUartMag[pMag->m_byUsedExUartCnt].m_bIsSendConfig = TRUE;
        pMag->m_byUsedExUartCnt += 1;
    }
    else
    {
        return RET_FALSE;
    }

    return RET_TRUE;
}

/****************************************************************
@FUNCTION�������������ñ��ĵ���չ����
@INPUT��
	@@@ Cfg - ���ò���
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_Send_ConfigUart(EXUART_CONFIG Cfg, EXCOM_MANAGE *pMag)
{
    uint8_t CmdBuf[10];
    uint16_t CmdBufLen;

    //���Ϳ��Ʊ���
    CmdBufLen = 0;
    CmdBuf[CmdBufLen] = 0x11;	//11������
    CmdBufLen += 1;
    CmdBuf[CmdBufLen] = Cfg.m_byComSeq;	//���ں�
    CmdBufLen += 1;

    CmdBuf[CmdBufLen] = Cfg.m_dwBaudrate >> 24;	//������
    CmdBufLen += 1;
    CmdBuf[CmdBufLen] = Cfg.m_dwBaudrate >> 16;	//������
    CmdBufLen += 1;
    CmdBuf[CmdBufLen] = Cfg.m_dwBaudrate >> 8;	//������
    CmdBufLen += 1;
    CmdBuf[CmdBufLen] = Cfg.m_dwBaudrate >> 0;	//������
    CmdBufLen += 1;

    CmdBuf[CmdBufLen] = Cfg.m_eParity;	//У��λ
    CmdBufLen += 1;
    CmdBuf[CmdBufLen] = Cfg.m_eWordLength;	//����λ
    CmdBufLen += 1;
    CmdBuf[CmdBufLen] = Cfg.m_eStopBits;	//ֹͣλ
    CmdBufLen += 1;

    pMag->m_wTmpBufLen = sizeof(pMag->m_aTmpBuf);
    Protocal_ProtocolMakeShort55AA(CmdBuf, CmdBufLen, pMag->m_aTmpBuf, &pMag->m_wTmpBufLen);


    return pMag->m_sHwOps.write(pMag->m_aTmpBuf, pMag->m_wTmpBufLen);
}


/****************************************************************
@FUNCTION������չ����
@INPUT��
	@@@ eExComSeq - ��չ�������к�
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_OpenUart(ENUM_COM_EX_SEQ eExComSeq, EXCOM_MANAGE *pMag)
{
    uint8_t Idx = 0;

    if(eExComSeq == EXCOM_SEQ_MAX)
    {
        return RET_FALSE;
    }

    //�ӵ�ǰ��չ�����б�����
    Idx = ExCom_GetComIdx(pMag, eExComSeq);

    //�ҵ�
    if(Idx < EXCOM_IDX_MAX)
    {
        pMag->m_saExUartMag[Idx].m_bIsSendEnable = TRUE;
    }
    //û�ҵ�������Ĭ������
    else if(pMag->m_byUsedExUartCnt < EXCOM_IDX_MAX)
    {
        memcpy((void *)&pMag->m_saExUartMag[pMag->m_byUsedExUartCnt].m_sConfig, (void *)&g_scExUartDefConfig[eExComSeq], sizeof(EXUART_CONFIG));
        pMag->m_saExUartMag[pMag->m_byUsedExUartCnt].m_bIsSendEnable = TRUE;
        pMag->m_byUsedExUartCnt += 1;
    }
    else
    {
        return RET_FALSE;
    }

    return RET_TRUE;

}


/****************************************************************
@FUNCTION���������ʹ򿪴��ڱ��ĵ���չ����
@INPUT��
	@@@ eExComSeq - ��չ�������к�
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_Send_OpenUart(ENUM_COM_EX_SEQ eExComSeq, EXCOM_MANAGE *pMag)
{
    uint8_t CmdBuf[10];
    uint16_t CmdBufLen;

    //���Ϳ��Ʊ���
    CmdBufLen = 0;
    CmdBuf[CmdBufLen] = 0x12;	//12������
    CmdBufLen += 1;
    CmdBuf[CmdBufLen] = eExComSeq;	//���ں�
    CmdBufLen += 1;
    CmdBuf[CmdBufLen] = 0x01;	//ʹ��
    CmdBufLen += 1;

    pMag->m_wTmpBufLen = sizeof(pMag->m_aTmpBuf);
    Protocal_ProtocolMakeShort55AA(CmdBuf, CmdBufLen, pMag->m_aTmpBuf, &pMag->m_wTmpBufLen);

    return pMag->m_sHwOps.write(pMag->m_aTmpBuf, pMag->m_wTmpBufLen);

}


/****************************************************************
@FUNCTION���ر���չ����
@INPUT��
	@@@ eExComSeq - ��չ�������к�
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_CloseUart(ENUM_COM_EX_SEQ eExComSeq, EXCOM_MANAGE *pMag)
{
    uint8_t Idx = 0;

    if(eExComSeq == EXCOM_SEQ_MAX)
    {
        return RET_FALSE;
    }

    //�ӵ�ǰ��չ�����б�����
    Idx = ExCom_GetComIdx(pMag, eExComSeq);

    //δ�ҵ�
    if(Idx >= EXCOM_IDX_MAX)
    {
        return RET_FALSE;
    }

    //�ݲ�ʵ�ֶ�̬����չ�����б������ڹرպ󲻻�����Դ
    pMag->m_saExUartMag[Idx].m_bIsSendDisEnable = TRUE;

    return RET_TRUE;
}

/****************************************************************
@FUNCTION���������͹رմ��ڱ��ĵ���չ����
@INPUT��
	@@@ eExComSeq - ��չ�������к�
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_Send_CloseUart(ENUM_COM_EX_SEQ eExComSeq, EXCOM_MANAGE *pMag)
{
    uint8_t CmdBuf[10];
    uint16_t CmdBufLen;

    //���Ϳ��Ʊ���
    CmdBufLen = 0;
    CmdBuf[CmdBufLen] = 0x12;	//12������
    CmdBufLen += 1;
    CmdBuf[CmdBufLen] = eExComSeq;	//���ں�
    CmdBufLen += 1;
    CmdBuf[CmdBufLen] = 0x0;	//��ֹ
    CmdBufLen += 1;

    pMag->m_wTmpBufLen = sizeof(pMag->m_aTmpBuf);
    Protocal_ProtocolMakeShort55AA(CmdBuf, CmdBufLen, pMag->m_aTmpBuf, &pMag->m_wTmpBufLen);

    return pMag->m_sHwOps.write(pMag->m_aTmpBuf, pMag->m_wTmpBufLen);
}

/****************************************************************
@FUNCTION������͸�����ݵ�ָ����չ����
@INPUT��
	@@@ eExCom - ��չ�������к�
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_SendDataPayloadToSpecifyUart(ENUM_COM_EX_SEQ eExComSeq, uint8_t *pBuf, uint16_t Len, EXCOM_MANAGE *pMag)
{
    uint8_t ExComIdx = ExCom_GetComIdx(pMag, eExComSeq);
    EXUART_MANAGER *pUartMag = &pMag->m_saExUartMag[ExComIdx];

    if(TRUE != pUartMag->m_bIsEnable)
    {
        return RET_FALSE;
    }

    pUartMag->m_bySendBuf[0] = eExComSeq;

    if(Len < (sizeof(pUartMag->m_bySendBuf) - 1))
    {
        pUartMag->m_wSendLen = Len + 1;
        memcpy(pUartMag->m_bySendBuf + 1, pBuf, pUartMag->m_wSendLen);
    }
    else
    {
        pUartMag->m_wSendLen = sizeof(pUartMag->m_bySendBuf);
        memcpy(pUartMag->m_bySendBuf + 1, pBuf, pUartMag->m_wSendLen);

    }

    pMag->m_wTmpBufLen = sizeof(pMag->m_aTmpBuf);
    Protocal_ProtocolMakeLong55AA(pUartMag->m_bySendBuf, pUartMag->m_wSendLen, pMag->m_aTmpBuf, &pMag->m_wTmpBufLen);

    //��־���
    ExCom_LogOutput(FALSE, eExComSeq, pUartMag->m_bySendBuf + 1, pUartMag->m_wSendLen - 1);
    //����
    pUartMag->m_dwSendCnt += (pUartMag->m_wSendLen - 1);

    return pMag->m_sHwOps.write(pMag->m_aTmpBuf, pMag->m_wTmpBufLen);
}


/****************************************************************
@FUNCTION����ȡָ����չ���ڵĽ��������غ�
@INPUT��
	@@@ eExCom - ��չ�������к�
	@@@ pMag - ������չ���ܹ���ָ��
	@@@ pBuf - �������ݻ�����
	@@@ pLen - �������ݻ���������
@OUTPUT��
	@@@ pBuf - ��������
	@@@ pLen - ��������ʵ�ʳ���
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_RecvSpecifyUartDataPayload(ENUM_COM_EX_SEQ eExComSeq, uint8_t *pBuf, uint16_t *pLen, EXCOM_MANAGE *pMag)
{
    uint8_t ExComIdx = ExCom_GetComIdx(pMag, eExComSeq);
    EXUART_MANAGER *pUartMag = &pMag->m_saExUartMag[ExComIdx];

    /**/
    if((TRUE == pUartMag->m_bIsEnable) && (TRUE == pUartMag->m_bIsComRecv))
    {
        if(pUartMag->m_wRecvLen == 0)
        {
            //pUartMag->m_bIsComRecv = FALSE;
            return RET_FALSE;
        }

        if(*pLen >= pUartMag->m_wRecvLen)
        {
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, pUartMag->m_wRecvLen);
            *pLen = pUartMag->m_wRecvLen;
        }
        else
        {
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, *pLen);
        }

        pUartMag->m_wRecvLen = 0;
        pUartMag->m_bIsComRecv = FALSE;

        return RET_TRUE;
    }

    return RET_FALSE;

}

/****************************************************************
@FUNCTION��������չ���ʼ��
@INPUT��
	@@@ a_sHwOps - ������չ����Ӳ������ָ��
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: TRUE-��ͣ FALSE-δ��ͣ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void ExCom_Init(EXCOM_HW_OPERATIONS a_sHwOps, EXCOM_MANAGE *pMag)
{
    assert_param(pMag != NULL);

    pMag->m_sHwOps.hw_init = a_sHwOps.hw_init;
    pMag->m_sHwOps.read = a_sHwOps.read;
    pMag->m_sHwOps.write = a_sHwOps.write;

    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    //��ʼ����ز���
    //memset((void *)pMag,0,sizeof(EXCOM_MANAGE));

}

//����͸����������
void ExCom_Protocal_ExchProcess(uint8_t *pCmdBuf, uint16_t CmdLen, EXCOM_MANAGE *pMag)
{
    uint8_t ExComIdx;
    EXUART_MANAGER *pExUartMag = NULL;

    if(CmdLen <= 1)
        return;

    ExComIdx = ExCom_GetComIdx(pMag, (ENUM_COM_EX_SEQ)pCmdBuf[0]);

    if(ExComIdx != (uint8_t)EXCOM_IDX_MAX)
    {
        pExUartMag = &pMag->m_saExUartMag[ExComIdx];
        pExUartMag->m_bIsComRecv = TRUE;
        pExUartMag->m_wRecvLen = CmdLen - 1;
        memcpy(pExUartMag->m_byRecvBuf, pCmdBuf + 1, pExUartMag->m_wRecvLen);
        //��־���
        ExCom_LogOutput(TRUE, (ENUM_COM_EX_SEQ)pCmdBuf[0], pExUartMag->m_byRecvBuf, pExUartMag->m_wRecvLen);
        //����
        pExUartMag->m_dwRecvCnt += pExUartMag->m_wRecvLen;
    }

}


//������������Ӧ��������
void ExCom_Protocal_CmdProcess(uint8_t *pCmdBuf, uint16_t CmdLen, EXCOM_MANAGE *pMag)
{
    uint8_t Cmd, ExComIdx;

    if(CmdLen < 2)
    {
        return;
    }


    Cmd = pCmdBuf[0];

    switch(Cmd)
    {
        case 0x11:
            ExComIdx = ExCom_GetComIdx(pMag, (ENUM_COM_EX_SEQ)pCmdBuf[1]);
            pMag->m_saExUartMag[ExComIdx].m_bIsSendConfig = FALSE;
            break;

        case 0x12:
            if(CmdLen < 3)
            {
                return;
            }

            ExComIdx = ExCom_GetComIdx(pMag, (ENUM_COM_EX_SEQ)pCmdBuf[1]);

            if((pCmdBuf[2] == 1) && (TRUE == pMag->m_saExUartMag[ExComIdx].m_bIsSendEnable))
            {
                pMag->m_saExUartMag[ExComIdx].m_bIsSendEnable = FALSE;
            }

            if((pCmdBuf[2] == 0) && (TRUE == pMag->m_saExUartMag[ExComIdx].m_bIsSendDisEnable))
            {
                pMag->m_saExUartMag[ExComIdx].m_bIsSendDisEnable = FALSE;
            }

            pMag->m_saExUartMag[ExComIdx].m_bIsEnable = pCmdBuf[2] == 1 ? TRUE : FALSE;
            break;

        default:
            break;
    }

}

/****************************************************************
@FUNCTION��������չ�嶨ʱ����,��Ҫ��⴮�����õ��Ƿ�ɹ�
@INPUT��
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: TRUE-��ͣ FALSE-δ��ͣ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void ExCom_TimedProcess(EXCOM_MANAGE *pMag)
{
    uint8_t i;


    for(i = 0; i < pMag->m_byUsedExUartCnt; i++)
    {
        if(TRUE == pMag->m_saExUartMag[i].m_bIsSendConfig)
        {
            ExCom_Send_ConfigUart(pMag->m_saExUartMag[i].m_sConfig, pMag);
        }

        if(TRUE == pMag->m_saExUartMag[i].m_bIsSendEnable)
        {
            ExCom_Send_OpenUart(pMag->m_saExUartMag[i].m_sConfig.m_byComSeq, pMag);
        }

        if(TRUE == pMag->m_saExUartMag[i].m_bIsSendDisEnable)
        {
            ExCom_Send_CloseUart(pMag->m_saExUartMag[i].m_sConfig.m_byComSeq, pMag);
        }
    }


}

/****************************************************************
@FUNCTION��������չ�崦���������ݣ������󻺴��ڶ�Ӧ���ڻ�����
@INPUT��
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: TRUE-��ͣ FALSE-δ��ͣ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t ExCom_Process(EXCOM_MANAGE *pMag)
{
    EXCOM_MANAGE *pExMag = pMag;
    uint16_t Len = EXCOM_BUFFER_LEN_MAX - pExMag->m_wProtBufLen;
    uint16_t i, DataLen;
    uint32_t IsCutted = FALSE, IsLong55AA = FALSE;

    /**/
    if(TRUE == pMag->m_sHwOps.read(&pMag->m_aProtBuf[pMag->m_wProtBufLen], &Len))
    {
        pExMag->m_wProtBufLen += Len;

        for(i = 0; i < pExMag->m_wProtBufLen;)
        {
            Len = pExMag->m_wProtBufLen - i;

            if(TRUE == Protocal_ProtocolAnalyse55AA(&pExMag->m_aProtBuf[i], &Len, &IsCutted, &IsLong55AA, 1024))
            {
                if(TRUE == IsCutted)
                {
                    //LOG("parse Cutted,i=%d,Len=%d\n\r",i,Len);
                    memcpy(pExMag->m_aProtBuf, &pExMag->m_aProtBuf[i], pExMag->m_wProtBufLen - i);
                    pExMag->m_wProtBufLen -= i;
                    break;
                }
                else
                {
                    //����Ϊ55AA����
                    if (IsLong55AA)
                    {
                        DataLen	= pExMag->m_aProtBuf[i + 3] | (pExMag->m_aProtBuf[i + 4] << 8);
                        ExCom_Protocal_ExchProcess(&pExMag->m_aProtBuf[i + 7], DataLen, pMag);
                    }
                    else
                    {
                        ExCom_Protocal_CmdProcess(&pExMag->m_aProtBuf[i + 3], pExMag->m_aProtBuf[i + 2], pMag);
                    }

                    i = Len + i;
                }
            }
            else
            {
                i++;
            }

            if(i == pExMag->m_wProtBufLen)
            {
                pExMag->m_wProtBufLen = 0;
            }

        }



        return TRUE;
    }

    return FALSE;

}


