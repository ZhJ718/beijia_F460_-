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
    uint32_t    m_dwScheduleTime;//最小调度时间，单位ms
    uint32_t    m_dwLastScheduleTime;//上次调度时间，单位ms

    LinkQueue   *m_pSendQueue;       //异步发送队列
    uint16_t    m_wSendQueueNodeMax; //发送队列最大节点数

    //应用协议缓冲区
    uint8_t     *m_pProtBuf;   //根据实际情况申请
    uint16_t    m_wProtBufMaxLen; //最大不超过65535
    uint16_t	m_wProtBufLen;


    uint32_t    m_dwLastCommTime;
} COMTASK_MAG;


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
uint8_t ComTask_Init(COMTASK_MAG *pMag, uint16_t MaxLen, uint32_t ScheTim);

/****************************************************************
@FUNCTION：端口任务反初始化
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void ComTask_DeInit(COMTASK_MAG *pMag);

/****************************************************************
@FUNCTION：端口任务是否正常
@INPUT：无
@OUTPUT：无
@RETURN: TRUE - 正常，FALSE - 异常
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t ComTask_IsComOK(COMTASK_MAG *pMag);

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
void ComTask_RetransmissionData(COMTASK_MAG *pMag, uint8_t *pBuf, uint16_t BufLen);

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
void ComTask_SetScheduleTime(COMTASK_MAG *pMag, uint32_t ScheTim);


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
void ComTask_SetQueueNodeLen(COMTASK_MAG *pMag, uint32_t Num);


#define SEND_LIST_NODE_MAX 60   //最大发送节点数

//队列也是一种特殊的线性表
//队列的业务节点的数据结构
typedef struct _tag_SendLinkNode
{
    LinkListNode node;  //节点
    void *item; //业务数据
} TSendLinkNode;


typedef struct
{
    uint32_t    m_dwScheduleTime;//最小调度时间，单位ms
    uint32_t    m_dwLastScheduleTime;//上次调度时间，单位ms

    LinkList    *m_pSendList;       //异步发送队列
    uint16_t    m_wSendListNodeMax; //发送队列最大节点数
    uint16_t    m_wScheNodeIdx; //当前遍历发送队列节点序号

    //应用协议缓冲区
    uint8_t     *m_pProtBuf;   //根据实际情况申请
    uint16_t    m_wProtBufMaxLen; //最大不超过65535
    uint16_t	m_wProtBufLen;


    uint32_t    m_dwLastCommTime;
} SCOMTASK_MAG;


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
uint8_t SComTask_Init(SCOMTASK_MAG *pMag, uint16_t MaxLen, uint32_t ScheTim);

/****************************************************************
@FUNCTION：端口任务反初始化
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void SComTask_DeInit(SCOMTASK_MAG *pMag);

/****************************************************************
@FUNCTION：端口任务是否正常
@INPUT：无
@OUTPUT：无
@RETURN: TRUE - 正常，FALSE - 异常
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t SComTask_IsComOK(SCOMTASK_MAG *pMag);

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
void SComTask_RetransmissionData(SCOMTASK_MAG *pMag, uint8_t *pBuf, uint16_t BufLen);

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
void SComTask_SetScheduleTime(SCOMTASK_MAG *pMag, uint32_t ScheTim);


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
void SComTask_SetListNodeLen(SCOMTASK_MAG *pMag, uint32_t Num);


//创建发送链表
/****************************************************************
@FUNCTION：端口任务创建发送链表
@INPUT：无
@OUTPUT：操作句柄
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
LinkList* SComTask_SendListCreate(void);

/****************************************************************
@FUNCTION：端口任务销毁发送链表
@INPUT：
    @param  pList - 句柄
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void SComTask_SendListDestroy(LinkList* pList);

/****************************************************************
@FUNCTION：端口任务清空发送链表
@INPUT：
    @param  pList - 句柄
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void SComTask_SendListClear(LinkList* pList);

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
void SComTask_SendListDelete(LinkList* pList, int Pos);


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
int SComTask_SendListAppend(LinkList *pList, void *pFrame);

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
int SComTask_SendListAppendDiff(LinkList *pList, void *pFrame);

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
int SComTask_SendListDeleteByFrameId(LinkList *pList, uint32_t Id);

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
void* SComTask_TimedProcess(SCOMTASK_MAG *pMag);





#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
