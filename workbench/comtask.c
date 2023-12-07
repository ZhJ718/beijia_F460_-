/**
  ******************************************************************************
  * @file    comtask.c
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

/* Includes ------------------------------------------------------------------*/
#include "comtask.h"
#include "timer.h"

#ifdef _APP
#include "rescom_task.h"
#endif
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
void ComTask_RetransmissionData(COMTASK_MAG *pMag, uint8_t *pBuf, uint16_t BufLen)
{
    FRAME_BUF *pFrame = NULL;

    assert_param(pMag != NULL);

    pFrame = sdram_malloc(0, sizeof(FRAME_BUF));

    if(pBuf == NULL || pFrame == NULL
            || BufLen > FRAME_BUF_LEN_MAX
            || BufLen == 0)
    {
        return;
    }

    memcpy(pFrame->m_byBuf, pBuf, BufLen);
    pFrame->m_wBufLen = BufLen;

    LinkQueue_Append(pMag->m_pSendQueue, (void*)pFrame);

}


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
void ComTask_SetScheduleTime(COMTASK_MAG *pMag, uint32_t ScheTim)
{
    pMag->m_dwScheduleTime = ScheTim;
}

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
void ComTask_SetQueueNodeLen(COMTASK_MAG *pMag, uint32_t Num)
{
    pMag->m_wSendQueueNodeMax = Num;
}

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
uint8_t ComTask_Init(COMTASK_MAG *pMag, uint16_t MaxLen, uint32_t ScheTim)
{
    uint8_t IsInit = FALSE;

    assert_param(pMag != NULL);

    pMag->m_wProtBufMaxLen = MaxLen;   //���Э�鳤��
    pMag->m_pProtBuf = (uint8_t *)sdram_malloc(0, pMag->m_wProtBufMaxLen);

    //���������Сʱ����
    pMag->m_dwScheduleTime = ScheTim;

    //�������Ͷ���
    pMag->m_pSendQueue =  LinkQueue_Create();

    //���Ͷ�����󻺴�ڵ���
    pMag->m_wSendQueueNodeMax = SEND_LIST_NODE_MAX;

    //�κη����ڴ�ʧ�ܣ����������ʼ��ʧ��
    IsInit = ((pMag->m_pProtBuf == NULL || pMag->m_pSendQueue == NULL) ? FALSE : TRUE);

    return IsInit;
}

/****************************************************************
@FUNCTION���˿����񷴳�ʼ��
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void ComTask_DeInit(COMTASK_MAG *pMag)
{
    if(pMag->m_pProtBuf != NULL)
    {
        //�ͷ�����Э�����������
        sdram_free(0, pMag->m_pProtBuf);
    }

    //�ͷ�������пռ�
    LinkQueue_Destroy(pMag->m_pSendQueue);
}

/****************************************************************
@FUNCTION���˿������Ƿ�����
@INPUT����
@OUTPUT����
@RETURN: TRUE - ������FALSE - �쳣
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t ComTask_IsComOK(COMTASK_MAG *pMag)
{
    uint32_t NowTim = GetSysRunMilliSecond();

    //����1����û�յ�����
    if(NowTim > pMag->m_dwLastCommTime
            && (NowTim - pMag->m_dwLastCommTime > 60 * 1000))
    {
        return FALSE;
    }

    return TRUE;
}

/****************************************************************
@FUNCTION���˿����񴴽���������
@INPUT����
@OUTPUT���������
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
LinkList* SComTask_SendListCreate(void)
{
    return LinkList_Create();
}


/****************************************************************
@FUNCTION���˿��������ٷ�������
@INPUT��
    @param  pList - ���
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void SComTask_SendListDestroy(LinkList* pList)
{
    SComTask_SendListClear(pList);
    LinkList_Destroy(pList);
}

/****************************************************************
@FUNCTION���˿�������շ�������
@INPUT��
    @param  pList - ���
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void SComTask_SendListClear(LinkList* pList)
{
    TSendLinkNode *tmp;

    while(LinkList_Length(pList) > 0)
    {
        tmp = (TSendLinkNode *) LinkList_Delete(pList, 0);

        if(tmp != NULL)
        {
            if(tmp->item != NULL)
            {
                sdram_free(0, tmp->item);
            }

            sdram_free(0, (void *)tmp);

        }
    }

    LinkList_Clear(pList);
}

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
void SComTask_SendListDelete(LinkList* pList, int Pos)
{
    TSendLinkNode *pNode = (TSendLinkNode *) LinkList_Delete(pList, Pos);

    if(pNode != NULL)
    {
        if(pNode->item != NULL)
        {
            sdram_free(0, pNode->item);
        }

        sdram_free(0, (void *)pNode);

    }
}

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
int SComTask_SendListAppend(LinkList *pList, void *pFrame)
{
    TSendLinkNode *tmp = NULL;
    int ret;

    if(pList == NULL || pFrame == NULL)
        return -1;

    tmp = (TSendLinkNode*)sdram_malloc(0, sizeof(TSendLinkNode));

    if(tmp == NULL)
    {
        ret = -1;
        return ret;
    }

    memset(tmp, 0, sizeof (TSendLinkNode));//��һ�������ε���
    // tmp->node.next = NULL;
    tmp->item = pFrame;

    //��Ҫ��ջ��item(ջ��ҵ��ڵ�)ת�� ������� LinkListNode
    ret = LinkList_Insert(pList, (LinkListNode *)tmp, LinkList_Length(pList));

    if(ret != 0)
    {
        //printf("func LinkList_Insert() err:%d \n", ret);
        // if(tmp != NULL) sdram_free(0,tmp);
        return ret;
    }

    return ret;
}

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
int SComTask_SendListDeleteByFrameId(LinkList *pList, uint32_t Id)
{
    TSendLinkNode *pNode = NULL;
    FRAME_BUF *pFrame = NULL;
    uint32_t i, len, cnt;

    if(pList == NULL)
        return -1;

    len = LinkList_Length(pList);

    //������������
    for(i = 0, cnt = 0; i < len; i++)
    {
        //��ȡ����ָ��λ�ý��
        pNode = (TSendLinkNode *)LinkList_Get(pList, i);

        //  �����ݽڵ㲻���ڣ�������
        if(pNode == NULL)
        {
            break;
        }

        //���ȡ������
        pFrame = (FRAME_BUF *)pNode->item;

        //  ��������ݲ����ڣ����˳�����ѭ��
        if(pFrame == NULL)
        {
            continue;
        }


        if(pFrame->m_dwID == Id)
        {
            SComTask_SendListDelete(pList, i);
            cnt++;
        }

    }

    return cnt;
}

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
int SComTask_SendListAppendDiff(LinkList *pList, void *pFrame)
{
    TSendLinkNode *tmp = NULL;
    FRAME_BUF *pFrameBuf = pFrame;
    int ret;

    if(pList == NULL || pFrame == NULL)
        return -1;

    tmp = (TSendLinkNode*)sdram_malloc(0, sizeof(TSendLinkNode));

    if(tmp == NULL)
    {
        ret = -1;
        return ret;
    }

    SComTask_SendListDeleteByFrameId(pList,pFrameBuf->m_dwID);
        
    memset(tmp, 0, sizeof (TSendLinkNode));//��һ�������ε���
    // tmp->node.next = NULL;
    tmp->item = pFrame;

    //��Ҫ��ջ��item(ջ��ҵ��ڵ�)ת�� ������� LinkListNode
    ret = LinkList_Insert(pList, (LinkListNode *)tmp, LinkList_Length(pList));

    if(ret != 0)
    {
        //printf("func LinkList_Insert() err:%d \n", ret);
        // if(tmp != NULL) sdram_free(0,tmp);
        return ret;
    }

    return ret;
}


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
void SComTask_RetransmissionData(SCOMTASK_MAG *pMag, uint8_t *pBuf, uint16_t BufLen)
{
    FRAME_BUF *pFrame = NULL;

    assert_param(pMag != NULL);

    pFrame = sdram_malloc(0, sizeof(FRAME_BUF));

    if(pBuf == NULL || pFrame == NULL
            || BufLen > FRAME_BUF_LEN_MAX
            || BufLen == 0)
    {
        return;
    }

    memset((void *)pFrame, 0, sizeof(FRAME_BUF));

    memcpy(pFrame->m_byBuf, pBuf, BufLen);
    pFrame->m_wBufLen = BufLen;

    pFrame->m_dwID = pFrame->m_byBuf[1];
    pFrame->m_dwCnt = 1; //�̶�ֻ����һ��
    pFrame->m_dwScheTime = 0;

    SComTask_SendListAppend(pMag->m_pSendList, (void *)pFrame);
}

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
void SComTask_SetScheduleTime(SCOMTASK_MAG *pMag, uint32_t ScheTim)
{
    pMag->m_dwScheduleTime = ScheTim;
}

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
void SComTask_SetListNodeLen(SCOMTASK_MAG *pMag, uint32_t Num)
{
    pMag->m_wSendListNodeMax = Num;
}



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
uint8_t SComTask_Init(SCOMTASK_MAG *pMag, uint16_t MaxLen, uint32_t ScheTim)
{
    uint8_t *pBuf = NULL;
    uint8_t IsInit = FALSE;

    assert_param(pMag != NULL);

    pMag->m_wProtBufMaxLen = MaxLen;   //���Э�鳤��
    pMag->m_pProtBuf = (uint8_t *)sdram_malloc(0, pMag->m_wProtBufMaxLen);

    //���������Сʱ����
    pMag->m_dwScheduleTime = ScheTim;

    //�������Ͷ���
    pMag->m_pSendList =  SComTask_SendListCreate();

    //���Ͷ�����󻺴�ڵ���
    pMag->m_wSendListNodeMax = SEND_LIST_NODE_MAX;

    //�κη����ڴ�ʧ�ܣ����������ʼ��ʧ��
    IsInit = ((pMag->m_pProtBuf == NULL || pMag->m_pSendList == NULL) ? FALSE : TRUE);

    return IsInit;
}

/****************************************************************
@FUNCTION���˿����񷴳�ʼ��
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void SComTask_DeInit(SCOMTASK_MAG *pMag)
{
    if(pMag->m_pProtBuf != NULL)
    {
        //�ͷ�����Э�����������
        sdram_free(0, pMag->m_pProtBuf);
    }

    //�ͷŷ�������
    SComTask_SendListDestroy(pMag->m_pSendList);
}

/****************************************************************
@FUNCTION���˿������Ƿ�����
@INPUT����
@OUTPUT����
@RETURN: TRUE - ������FALSE - �쳣
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t SComTask_IsComOK(SCOMTASK_MAG *pMag)
{
    uint32_t NowTim = GetSysRunMilliSecond();

    //����1����û�յ�����
    if(NowTim > pMag->m_dwLastCommTime
            && (NowTim - pMag->m_dwLastCommTime > 60 * 1000))
    {
        return FALSE;
    }

    return TRUE;
}


#ifdef _APP
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
void* SComTask_TimedProcess(SCOMTASK_MAG *pMag)
{
    TSendLinkNode *pNode = NULL;
    FRAME_BUF *pFrame = NULL;
		uint16_t Evtnum = 0;
    uint32_t i, len, NowTim;

    if(pMag == NULL
            || LinkList_Length(pMag->m_pSendList) <= 0)
    {
        return NULL;
    }

    len = LinkList_Length(pMag->m_pSendList);

    //��鷢�������ȣ�������������������ͷ��ڴ�
    if(len > pMag->m_wSendListNodeMax)
    {
        SComTask_SendListClear(pMag->m_pSendList);
        //LOG("HmiComTask SendQueue Clean\n\r");
        return NULL;
    }

    //������������
    for(i = 0; i < len; i++)
    {
        //��ȡ����ָ��λ�ý��
        pNode = (TSendLinkNode *)LinkList_Get(pMag->m_pSendList, i);

        //  �����ݽڵ㲻���ڣ�������
        if(pNode == NULL)
        {
            break;
        }

        //���ȡ������
        pFrame = (FRAME_BUF *)pNode->item;

        //  ��������ݲ����ڣ����˳�����ѭ��
        if(pFrame == NULL)
        {
            continue;
        }


        //����֡������Ϊ0���Ҿ����ϴη����Ѿ�������ʱ����
        NowTim = GetSysRunMilliSecond();

        if(pFrame->m_dwCnt > 0 && ((NowTim - pFrame->m_dwTime) >= pFrame->m_dwScheTime))
        {		
						if(((pFrame->m_byBuf[0] << 8 | pFrame->m_byBuf[1]) == 0xA893)
							|| ((pFrame->m_byBuf[0] << 8 | pFrame->m_byBuf[1]) == 0xA895))
						{
								if(g_sResComTask_Mag.m_IsAccess == FALSE)
									continue;				
						
								Evtnum = pFrame->m_byBuf[pFrame->m_wBufLen - 3] << 8 | pFrame->m_byBuf[pFrame->m_wBufLen - 2];					
								
								if(g_sResComTask_Mag.EvtListResPonseNum[Evtnum] == TRUE)
								{
										if(pFrame->m_bySendNum > 5)
										{
											SComTask_SendListDelete(pMag->m_pSendList, i);
											continue;
										}	
										
										pFrame->m_bySendNum++;
										pFrame->m_dwTime = NowTim;
									
										return (void *)pFrame;
								}
								else
								{
									SComTask_SendListDelete(pMag->m_pSendList, i);
									pFrame->m_dwCnt = 0;
									pFrame->m_bySendNum = 0;
									continue;
								}
						}
						else
						{
							pFrame->m_dwCnt--;
													
							pFrame->m_dwTime = NowTim;
						
							return (void *)pFrame;
						}
        }
        //����֡����Ϊ0��ɾ���˽ڵ㣬�ͷ������ڴ�
        else if(pFrame->m_dwCnt == 0)
        {
            SComTask_SendListDelete(pMag->m_pSendList, i);
        }
    }

    return NULL;
}

#endif


/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
