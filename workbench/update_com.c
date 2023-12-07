/******************************************************************
*��Ȩ���� (C)2021, ���ջ������Ƽ����޹�˾
*
*�ļ�����:  update_com.c
*�ļ���ʶ:
*����ժҪ:
*����˵��:
*��ǰ�汾:
*��    ��:
*�������:  2021/07/02
*
*�޸ļ�¼1:
*    �޸�����:
*    �� �� ��:
*    �� �� ��:
*    �޸�����:
******************************************************************/


/******************************************************************
*                             ͷ�ļ�                              *
******************************************************************/

#include "update_com.h"

#include "upgrade.h"
#include "malloc.h"

/****************************************************************
@FUNCTION��������������󻺴�Э�鳤��
@INPUT��
	UPCOM_MAG	-	ͨѶ�ڹ���ָ�룬����Ϊ��
    Len   - ��󳤶�
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��Ҫָ����������󻺴泤�ȣ���󲻳���8k

****************************************************************/
void UpdateCom_SetMaxPackLen(UPDATECOM_MAG *pMag, uint32_t Len)
{
    assert_param(pMag != NULL);

    pMag->m_wProtBufMaxLen = MIN(Len, 8 * 1024);
}

/****************************************************************
@FUNCTION�������ڳ�ʼ��
@INPUT��
	UPCOM_HW_OPERATIONS - ����ͨѶ����صײ��������
	UPCOM_MAG	-	����ͨѶ�ڹ���ָ�룬����Ϊ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��Ҫָ����������󻺴泤��

****************************************************************/
void UpdateCom_Init(UPDATECOM_HW_OPERATIONS a_sHwOps, UPDATECOM_MAG *pMag)
{
    assert_param(pMag != NULL);

    pMag->m_sHwOps.hw_init = a_sHwOps.hw_init;
    pMag->m_sHwOps.read = a_sHwOps.read;
    pMag->m_sHwOps.write = a_sHwOps.write;

    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    pMag->m_pProtBuf = (uint8_t *)sdram_malloc(0, pMag->m_wProtBufMaxLen);

    pMag->m_bIsInit = (pMag->m_pProtBuf == NULL ? FALSE : TRUE);


}

/****************************************************************
@FUNCTION�������ڳ�ʼ������
@INPUT��
	UPCOM_MAG	-	����ͨѶ�ڹ���ָ�룬����Ϊ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��ʼ����

****************************************************************/
void UpdateCom_DeInit(UPDATECOM_MAG *pMag)
{
    assert_param(pMag != NULL);


    if(pMag->m_pProtBuf != NULL)
    {
        //�ͷ������ڴ�
        sdram_free(0, pMag->m_pProtBuf);
    }

    memset((void *)pMag, 0, sizeof(UPDATECOM_MAG));
}

/****************************************************************
@FUNCTION���������ڽ��մ������
@INPUT��
	UPDATECOM_MAG	-	�������ڹ���ָ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ֧������

****************************************************************/
void UpdateCom_RecvProcess(UPDATECOM_MAG *pMag)
{
    uint16_t Len = pMag->m_wProtBufMaxLen - pMag->m_wProtBufLen;
    uint16_t i, DataLen;
    uint32_t IsCutted = FALSE, IsLong55AA = FALSE;

    if(FALSE == pMag->m_bIsInit)
    {
        return;
    }

    if(TRUE == pMag->m_sHwOps.read(&pMag->m_pProtBuf[pMag->m_wProtBufLen], &Len))
    {
        pMag->m_wProtBufLen += Len;

        pMag->m_dwNoCommTimeout = 0;

        for(i = 0; i < pMag->m_wProtBufLen;)
        {
            Len = pMag->m_wProtBufLen - i;

            if(TRUE == Update_ProtocalParse(&pMag->m_pProtBuf[i], &Len, &IsCutted, &IsLong55AA))
            {
                if(TRUE == IsCutted)
                {
                    if(pMag->m_wProtBufLen == pMag->m_wProtBufMaxLen)
                    {
                        //�������δ��������������˻��������ұ����Ǳ��ضϵģ�������û���������
                        pMag->m_wProtBufLen = 0;
                        break;
                    }

                    //LOG("parse Cutted,i=%d,Len=%d\n\r",i,Len);
                    memcpy(pMag->m_pProtBuf, &pMag->m_pProtBuf[i], pMag->m_wProtBufLen - i);
                    pMag->m_wProtBufLen -= i;
                    break;
                }
                else
                {
                    Update_ConfigSendFunc(pMag->m_sHwOps.write);

                    //����Ϊ55AA����
                    if (IsLong55AA)
                    {
                        DataLen	= pMag->m_pProtBuf[i + 3] | (pMag->m_pProtBuf[i + 4] << 8);
                        Update_ProtocalProcess(&pMag->m_pProtBuf[i + 7], DataLen);
                    }
                    else
                    {
                        //LOG("parse succ,i=%d,Len=%d\n\r",i,Len);
                        Update_ProtocalProcess(&pMag->m_pProtBuf[i + 3], pMag->m_pProtBuf[i + 2]);
                    }

                    i = Len + i;
                }
            }
            else
            {
                i++;
            }

            if(i == pMag->m_wProtBufLen)
            {
                pMag->m_wProtBufLen = 0;
            }
        }


    }
}



/****************************************************************
@FUNCTION���������ڽ��մ��� - �ҵ�����Э���ʽ�ı���
@INPUT��
	UPDATECOM_MAG	-	�������ڹ���ָ��
@OUTPUT: ��
@RETURN: 0 -  δ�ҵ�,  1 - �ҵ�
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
uint32_t UpdateCom_Find55AAFrame(UPDATECOM_MAG *pMag)
{
    uint16_t Len = pMag->m_wProtBufMaxLen - pMag->m_wProtBufLen;
    uint16_t i, DataLen;
    uint32_t IsCutted = FALSE, IsLong55AA = FALSE;

    if(NULL == pMag->m_pProtBuf)
    {
        return 0;
    }

    if(TRUE == pMag->m_sHwOps.read(&pMag->m_pProtBuf[pMag->m_wProtBufLen], &Len))
    {
        pMag->m_wProtBufLen += Len;

        pMag->m_dwNoCommTimeout = 0;

        for(i = 0; i < pMag->m_wProtBufLen;)
        {
            Len = pMag->m_wProtBufLen - i;

            if(TRUE == Update_ProtocalParse(&pMag->m_pProtBuf[i], &Len, &IsCutted, &IsLong55AA))
            {
                if(TRUE == IsCutted)
                {
                    //LOG("parse Cutted,i=%d,Len=%d\n\r",i,Len);
                    memcpy(pMag->m_pProtBuf, &pMag->m_pProtBuf[i], pMag->m_wProtBufLen - i);
                    pMag->m_wProtBufLen -= i;
                    break;
                }
                else
                {
                    //����Ϊ55AA����

                    i = Len + i;

                    return 1;
                }
            }
            else
            {
                i++;
            }

            if(i == pMag->m_wProtBufLen)
            {
                pMag->m_wProtBufLen = 0;
            }
        }
    }

    return 0;
}

