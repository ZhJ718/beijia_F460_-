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
@FUNCTION：端口纯转发数据
@INPUT：
    @param  pBuf - 数据
    @param  BufLen - 数据长度
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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
@FUNCTION：端口任务重置调度时间
@INPUT：
    @param  pMag - 句柄
    @param  ScheTim - 调度时间
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void ComTask_SetScheduleTime(COMTASK_MAG *pMag, uint32_t ScheTim)
{
    pMag->m_dwScheduleTime = ScheTim;
}

/****************************************************************
@FUNCTION：端口任务重置调度时间
@INPUT：
    @param  pMag - 句柄
    @param  Num - 节点数量
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void ComTask_SetQueueNodeLen(COMTASK_MAG *pMag, uint32_t Num)
{
    pMag->m_wSendQueueNodeMax = Num;
}

/****************************************************************
@FUNCTION：端口任务初始化
@INPUT：
    @param  MaxLen - 最大长度
    @param  ScheTim - 调度时间
@OUTPUT：TRUE - 成功，FALSE - 失败
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 申请内存要释放！！！
****************************************************************/
uint8_t ComTask_Init(COMTASK_MAG *pMag, uint16_t MaxLen, uint32_t ScheTim)
{
    uint8_t IsInit = FALSE;

    assert_param(pMag != NULL);

    pMag->m_wProtBufMaxLen = MaxLen;   //最大协议长度
    pMag->m_pProtBuf = (uint8_t *)sdram_malloc(0, pMag->m_wProtBufMaxLen);

    //任务调度最小时间间隔
    pMag->m_dwScheduleTime = ScheTim;

    //创建发送队列
    pMag->m_pSendQueue =  LinkQueue_Create();

    //发送队列最大缓存节点数
    pMag->m_wSendQueueNodeMax = SEND_LIST_NODE_MAX;

    //任何分配内存失败，串口任务初始化失败
    IsInit = ((pMag->m_pProtBuf == NULL || pMag->m_pSendQueue == NULL) ? FALSE : TRUE);

    return IsInit;
}

/****************************************************************
@FUNCTION：端口任务反初始化
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void ComTask_DeInit(COMTASK_MAG *pMag)
{
    if(pMag->m_pProtBuf != NULL)
    {
        //释放申请协议解析缓冲区
        sdram_free(0, pMag->m_pProtBuf);
    }

    //释放申请队列空间
    LinkQueue_Destroy(pMag->m_pSendQueue);
}

/****************************************************************
@FUNCTION：端口任务是否正常
@INPUT：无
@OUTPUT：无
@RETURN: TRUE - 正常，FALSE - 异常
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t ComTask_IsComOK(COMTASK_MAG *pMag)
{
    uint32_t NowTim = GetSysRunMilliSecond();

    //超过1分钟没收到数据
    if(NowTim > pMag->m_dwLastCommTime
            && (NowTim - pMag->m_dwLastCommTime > 60 * 1000))
    {
        return FALSE;
    }

    return TRUE;
}

/****************************************************************
@FUNCTION：端口任务创建发送链表
@INPUT：无
@OUTPUT：操作句柄
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
LinkList* SComTask_SendListCreate(void)
{
    return LinkList_Create();
}


/****************************************************************
@FUNCTION：端口任务销毁发送链表
@INPUT：
    @param  pList - 句柄
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void SComTask_SendListDestroy(LinkList* pList)
{
    SComTask_SendListClear(pList);
    LinkList_Destroy(pList);
}

/****************************************************************
@FUNCTION：端口任务清空发送链表
@INPUT：
    @param  pList - 句柄
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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
@FUNCTION：端口任务删除发送链表指定位置节点
@INPUT：
    @param  pList - 链表句柄
    @param  Pos - 指定位置
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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
@FUNCTION：端口任务发送链表尾部插入数据
@INPUT：
    @param  pList - 链表句柄
    @param  pFrame - 业务数据句柄
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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

    memset(tmp, 0, sizeof (TSendLinkNode));//做一个负责任的人
    // tmp->node.next = NULL;
    tmp->item = pFrame;

    //需要把栈的item(栈的业务节点)转化 成链表的 LinkListNode
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
@FUNCTION：端口任务发送删除指定链表节点
@INPUT：
    @param  pList - 链表句柄
    @param  Id - 指定帧身份信息
@OUTPUT：无
@RETURN: 删除结点个数
@AUTHOR：xfw
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

    //遍历发送链表
    for(i = 0, cnt = 0; i < len; i++)
    {
        //获取链表指定位置结点
        pNode = (TSendLinkNode *)LinkList_Get(pList, i);

        //  若数据节点不存在，则跳出
        if(pNode == NULL)
        {
            break;
        }

        //结点取出数据
        pFrame = (FRAME_BUF *)pNode->item;

        //  若结点数据不存在，则退出本次循环
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
@FUNCTION：端口任务发送链表尾部插入不同数据
@INPUT：
    @param  pList - 链表句柄
    @param  pFrame - 业务数据句柄
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 插入前会检测链表有没有相同数据，有的话会删除
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
        
    memset(tmp, 0, sizeof (TSendLinkNode));//做一个负责任的人
    // tmp->node.next = NULL;
    tmp->item = pFrame;

    //需要把栈的item(栈的业务节点)转化 成链表的 LinkListNode
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
@FUNCTION：端口纯转发数据
@INPUT：
    @param  pBuf - 数据
    @param  BufLen - 数据长度
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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
    pFrame->m_dwCnt = 1; //固定只发送一次
    pFrame->m_dwScheTime = 0;

    SComTask_SendListAppend(pMag->m_pSendList, (void *)pFrame);
}

/****************************************************************
@FUNCTION：端口任务重置调度时间
@INPUT：
    @param  pMag - 句柄
    @param  ScheTim - 调度时间
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void SComTask_SetScheduleTime(SCOMTASK_MAG *pMag, uint32_t ScheTim)
{
    pMag->m_dwScheduleTime = ScheTim;
}

/****************************************************************
@FUNCTION：端口任务重置发送链表节点数
@INPUT：
    @param  pMag - 句柄
    @param  Num - 节点数量
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void SComTask_SetListNodeLen(SCOMTASK_MAG *pMag, uint32_t Num)
{
    pMag->m_wSendListNodeMax = Num;
}



/****************************************************************
@FUNCTION：端口任务初始化
@INPUT：
    @param  MaxLen - 最大长度
    @param  ScheTim - 调度时间
@OUTPUT：TRUE - 成功，FALSE - 失败
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 申请内存要释放！！！
****************************************************************/
uint8_t SComTask_Init(SCOMTASK_MAG *pMag, uint16_t MaxLen, uint32_t ScheTim)
{
    uint8_t *pBuf = NULL;
    uint8_t IsInit = FALSE;

    assert_param(pMag != NULL);

    pMag->m_wProtBufMaxLen = MaxLen;   //最大协议长度
    pMag->m_pProtBuf = (uint8_t *)sdram_malloc(0, pMag->m_wProtBufMaxLen);

    //任务调度最小时间间隔
    pMag->m_dwScheduleTime = ScheTim;

    //创建发送队列
    pMag->m_pSendList =  SComTask_SendListCreate();

    //发送队列最大缓存节点数
    pMag->m_wSendListNodeMax = SEND_LIST_NODE_MAX;

    //任何分配内存失败，串口任务初始化失败
    IsInit = ((pMag->m_pProtBuf == NULL || pMag->m_pSendList == NULL) ? FALSE : TRUE);

    return IsInit;
}

/****************************************************************
@FUNCTION：端口任务反初始化
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void SComTask_DeInit(SCOMTASK_MAG *pMag)
{
    if(pMag->m_pProtBuf != NULL)
    {
        //释放申请协议解析缓冲区
        sdram_free(0, pMag->m_pProtBuf);
    }

    //释放发送链表
    SComTask_SendListDestroy(pMag->m_pSendList);
}

/****************************************************************
@FUNCTION：端口任务是否正常
@INPUT：无
@OUTPUT：无
@RETURN: TRUE - 正常，FALSE - 异常
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t SComTask_IsComOK(SCOMTASK_MAG *pMag)
{
    uint32_t NowTim = GetSysRunMilliSecond();

    //超过1分钟没收到数据
    if(NowTim > pMag->m_dwLastCommTime
            && (NowTim - pMag->m_dwLastCommTime > 60 * 1000))
    {
        return FALSE;
    }

    return TRUE;
}


#ifdef _APP
/****************************************************************
@FUNCTION：端口任务定时调度函数
                    1 - 发送链表长度监控
                    2 - 遍历发送链表，取出符合条件的业务数据句柄
@INPUT：
    @param  pMag - 句柄
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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

    //检查发送链表长度，若超长，则清空链表，释放内存
    if(len > pMag->m_wSendListNodeMax)
    {
        SComTask_SendListClear(pMag->m_pSendList);
        //LOG("HmiComTask SendQueue Clean\n\r");
        return NULL;
    }

    //遍历发送链表
    for(i = 0; i < len; i++)
    {
        //获取链表指定位置结点
        pNode = (TSendLinkNode *)LinkList_Get(pMag->m_pSendList, i);

        //  若数据节点不存在，则跳出
        if(pNode == NULL)
        {
            break;
        }

        //结点取出数据
        pFrame = (FRAME_BUF *)pNode->item;

        //  若结点数据不存在，则退出本次循环
        if(pFrame == NULL)
        {
            continue;
        }


        //发送帧计数不为0，且距离上次发送已经到调度时间了
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
        //发送帧计数为0，删除此节点，释放申请内存
        else if(pFrame->m_dwCnt == 0)
        {
            SComTask_SendListDelete(pMag->m_pSendList, i);
        }
    }

    return NULL;
}

#endif


/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
