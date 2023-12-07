/********************************************************************************
* @File name: rescom_laser.c
* @Author: xfw
* @Version: 1.0
* @Date: 2023-3-8
* @Description:
********************************************************************************/

#include "rescom_task.h"



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

UPDATECOM_MAG g_sResComMag;


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
    UPDATECOM_HW_OPERATIONS ResComHwOps =
    {
        .hw_init = ResCom_ComInit,
        .read = ResCom_ComRead,
        .write = ResCom_ComWrite
    };

    memset((void *)&g_sResComMag, 0, sizeof(UPDATECOM_MAG));

    UpdateCom_SetMaxPackLen(&g_sResComMag, RES_COM_FRAME_LEN_MAX);
    UpdateCom_Init(ResComHwOps, &g_sResComMag);

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
    UpdateCom_DeInit(&g_sResComMag);
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
    //��ʱ����


    //���ڽ��մ���
    UpdateCom_RecvProcess(&g_sResComMag);


}

