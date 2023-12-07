/**
  ******************************************************************************
  * @file    comtask.h
  * @author  xfw
  * @brief   This file contains definitions for com task
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2023 Beijing HuaRu Tech Co.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of HuaRu nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMTASK_H
#define __COMTASK_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "linklist.h"
#include "linkqueue.h"

typedef struct
{
    uint32_t    m_dwScheduleTime;//��С����ʱ�䣬��λms
    uint32_t    m_dwLastScheduleTime;//�ϴε���ʱ�䣬��λms

    LinkQueue   *m_pSendQueue;       //�첽���Ͷ���
    uint16_t    m_wSendQueueNodeMax; //���Ͷ������ڵ���

    //Ӧ��Э�黺����
    uint8_t     *m_pProtBuf;   //����ʵ���������
    uint16_t    m_wProtBufMaxLen; //��󲻳���65535
    uint16_t	m_wProtBufLen;


    uint32_t    m_dwLastCommTime;
} COMTASK_MAG;


/****************************************************************
@FUNCTION���˿������ʼ��
@INPUT��
    @param  MaxLen - ��󳤶�
    @param  ScheTim - ����ʱ��
@OUTPUT��TRUE - �ɹ���FALSE - ʧ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �����ڴ�Ҫ�ͷţ�����
****************************************************************/
uint8_t ComTask_Init(COMTASK_MAG *pMag, uint16_t MaxLen, uint32_t ScheTim);

/****************************************************************
@FUNCTION���˿����񷴳�ʼ��
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void ComTask_DeInit(COMTASK_MAG *pMag);

/****************************************************************
@FUNCTION���˿������Ƿ�����
@INPUT����
@OUTPUT����
@RETURN: TRUE - ������FALSE - �쳣
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t ComTask_IsComOK(COMTASK_MAG *pMag);

/****************************************************************
@FUNCTION���˿ڴ�ת������
@INPUT��
    @param  pBuf - ����
    @param  BufLen - ���ݳ���
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void ComTask_RetransmissionData(COMTASK_MAG *pMag, uint8_t *pBuf, uint16_t BufLen);

/****************************************************************
@FUNCTION���˿��������õ���ʱ��
@INPUT��
    @param  pMag - ���
    @param  ScheTim - ����ʱ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void ComTask_SetScheduleTime(COMTASK_MAG *pMag, uint32_t ScheTim);


/****************************************************************
@FUNCTION���˿��������õ���ʱ��
@INPUT��
    @param  pMag - ���
    @param  Num - �ڵ�����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void ComTask_SetQueueNodeLen(COMTASK_MAG *pMag, uint32_t Num);


#define SEND_LIST_NODE_MAX 60   //����ͽڵ���

//����Ҳ��һ����������Ա�
//���е�ҵ��ڵ�����ݽṹ
typedef struct _tag_SendLinkNode
{
    LinkListNode node;  //�ڵ�
    void *item; //ҵ������
} TSendLinkNode;


typedef struct
{
    uint32_t    m_dwScheduleTime;//��С����ʱ�䣬��λms
    uint32_t    m_dwLastScheduleTime;//�ϴε���ʱ�䣬��λms

    LinkList    *m_pSendList;       //�첽���Ͷ���
    uint16_t    m_wSendListNodeMax; //���Ͷ������ڵ���
    uint16_t    m_wScheNodeIdx; //��ǰ�������Ͷ��нڵ����

    //Ӧ��Э�黺����
    uint8_t     *m_pProtBuf;   //����ʵ���������
    uint16_t    m_wProtBufMaxLen; //��󲻳���65535
    uint16_t	m_wProtBufLen;


    uint32_t    m_dwLastCommTime;
} SCOMTASK_MAG;


/****************************************************************
@FUNCTION���˿������ʼ��
@INPUT��
    @param  MaxLen - ��󳤶�
    @param  ScheTim - ����ʱ��
@OUTPUT��TRUE - �ɹ���FALSE - ʧ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �����ڴ�Ҫ�ͷţ�����
****************************************************************/
uint8_t SComTask_Init(SCOMTASK_MAG *pMag, uint16_t MaxLen, uint32_t ScheTim);

/****************************************************************
@FUNCTION���˿����񷴳�ʼ��
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void SComTask_DeInit(SCOMTASK_MAG *pMag);

/****************************************************************
@FUNCTION���˿������Ƿ�����
@INPUT����
@OUTPUT����
@RETURN: TRUE - ������FALSE - �쳣
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t SComTask_IsComOK(SCOMTASK_MAG *pMag);

/****************************************************************
@FUNCTION���˿ڴ�ת������
@INPUT��
    @param  pBuf - ����
    @param  BufLen - ���ݳ���
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void SComTask_RetransmissionData(SCOMTASK_MAG *pMag, uint8_t *pBuf, uint16_t BufLen);

/****************************************************************
@FUNCTION���˿��������õ���ʱ��
@INPUT��
    @param  pMag - ���
    @param  ScheTim - ����ʱ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void SComTask_SetScheduleTime(SCOMTASK_MAG *pMag, uint32_t ScheTim);


/****************************************************************
@FUNCTION���˿��������÷�������ڵ���
@INPUT��
    @param  pMag - ���
    @param  Num - �ڵ�����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void SComTask_SetListNodeLen(SCOMTASK_MAG *pMag, uint32_t Num);


//������������
/****************************************************************
@FUNCTION���˿����񴴽���������
@INPUT����
@OUTPUT���������
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
LinkList* SComTask_SendListCreate(void);

/****************************************************************
@FUNCTION���˿��������ٷ�������
@INPUT��
    @param  pList - ���
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void SComTask_SendListDestroy(LinkList* pList);

/****************************************************************
@FUNCTION���˿�������շ�������
@INPUT��
    @param  pList - ���
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void SComTask_SendListClear(LinkList* pList);

/****************************************************************
@FUNCTION���˿�����ɾ����������ָ��λ�ýڵ�
@INPUT��
    @param  pList - ������
    @param  Pos - ָ��λ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void SComTask_SendListDelete(LinkList* pList, int Pos);


/****************************************************************
@FUNCTION���˿�����������β����������
@INPUT��
    @param  pList - ������
    @param  pFrame - ҵ�����ݾ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int SComTask_SendListAppend(LinkList *pList, void *pFrame);

/****************************************************************
@FUNCTION���˿�����������β�����벻ͬ����
@INPUT��
    @param  pList - ������
    @param  pFrame - ҵ�����ݾ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����ǰ����������û����ͬ���ݣ��еĻ���ɾ��
****************************************************************/
int SComTask_SendListAppendDiff(LinkList *pList, void *pFrame);

/****************************************************************
@FUNCTION���˿�������ɾ��ָ������ڵ�
@INPUT��
    @param  pList - ������
    @param  Id - ָ��֡�����Ϣ
@OUTPUT����
@RETURN: ɾ��������
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int SComTask_SendListDeleteByFrameId(LinkList *pList, uint32_t Id);

/****************************************************************
@FUNCTION���˿�����ʱ���Ⱥ���
                    1 - ���������ȼ��
                    2 - ������������ȡ������������ҵ�����ݾ��
@INPUT��
    @param  pMag - ���
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void* SComTask_TimedProcess(SCOMTASK_MAG *pMag);





#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
