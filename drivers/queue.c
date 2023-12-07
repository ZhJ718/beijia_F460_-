/******************************************************************
*版权所有 (C)2019, 北京华如科技南京分公司
*
*文件名称:  queue.c
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者:
*完成日期:  2019/5/5
*
*修改记录1:
*    修改日期:
*    版 本 号:
*    修 改 人:xfw
*    修改内容:
******************************************************************/

/******************************************************************
*                             头文件                              *
******************************************************************/
#include "queue.h"
#include "malloc.h"

/******************************************************************
*                           全局变量声明                          *
******************************************************************/


/*************************************************************************************************************************************************************************
 * 函数
 ************************************************************************************************************************************************************************
 */
extern uint32_t QueueListInit(QUEUE **Queue)
{
    QUEUE *pTemp = NULL;
    pTemp = (QUEUE *)sdram_malloc(0, sizeof(QUEUE));

    if(pTemp == NULL)
    {
        //fprintf(stderr,"%s ERROR:%s\n",__FUNCTION__,strerror(errno));
        return FALSE;
    }

    memset(pTemp, 0, sizeof(QUEUE));
    *Queue = pTemp;
    return TRUE;
}

extern void QueueListDeInit(QUEUE **ppQueue)
{
    QUEUE_NODE *pTempHead = NULL;
    QUEUE_NODE *pTemp = NULL;
    QUEUE *pQueue = NULL;

    //DPrintf("@@@@@@@@@@ 准备释放队列所占用的空间...\r\n");
    if(*ppQueue != NULL)
    {
        pQueue = *ppQueue;
        pTempHead = pQueue->Head;

        while(pTempHead != NULL)
        {
            pTemp = pTempHead->Next;
            sdram_free(0, pTempHead);
            pTempHead = pTemp;
        }

        sdram_free(0, pQueue);
        *ppQueue = NULL;
    }
}


/*
 ************************************************************************************************************************************************************************
 *函数名称: QueuePush
 *功能描述: 向队列添加数据
 *输入描述: 队列、节点数据
 *输出描述: 无
 *返回描述: TRUE成功/FALSE失败
 *作者日期: XFW
 *全局声明: 无
 *特殊说明: 无
 ************************************************************************************************************************************************************************
 */
extern uint32_t QueuePush(QUEUE *pQueue, PQUEUE_DATA pReq)
{
    QUEUE_NODE *pNewNode = NULL;

    if((pQueue == NULL) || (pReq == NULL))
        return FALSE;

    pNewNode = (QUEUE_NODE *)sdram_malloc(0, sizeof(QUEUE_NODE));

    if(pNewNode == NULL)
    {
        //fprintf(stderr,"%s ERROR:%s\n",__FUNCTION__,strerror(errno));
        return FALSE;
    }

    memset(pNewNode, 0, sizeof(QUEUE_NODE));
    memcpy(&pNewNode->Data, pReq, sizeof(QUEUE_DATA));

    //PTHREAD_MUTEX_SAFE_LOCK(FtpSendFileMutex, OldStatus);
    if((pQueue->Head == NULL) && (pQueue->Tail == NULL))
    {
        pQueue->Head = pQueue->Tail = pNewNode;
    }
    else
    {
        pQueue->Tail->Next = pNewNode;
        pQueue->Tail = pNewNode;
    }

    //PTHREAD_MUTEX_SAFE_UNLOCK(FtpSendFileMutex, OldStatus);
    return TRUE;
}

/*
 ************************************************************************************************************************************************************************
 *函数名称: QueuePop
 *功能描述: 从队列中弹出数据
 *输入描述: 队列
 *输出描述: 节点数据
 *返回描述: TRUE成功/FALSE失败
 *作者日期: XFW
 *全局声明: 无
 *特殊说明:
 ************************************************************************************************************************************************************************
 */
extern uint32_t QueueGetHead(QUEUE *pQueue)
{
    if((pQueue == NULL) || (pQueue->Head == NULL))
        return FALSE;

    return TRUE;
}

extern uint32_t QueuePop(QUEUE *pQueue, PQUEUE_DATA pReq)
{
    QUEUE_NODE *pDelNode = NULL;

    if((pQueue == NULL) || (pQueue->Head == NULL))
        return FALSE;

    //PTHREAD_MUTEX_SAFE_LOCK(FtpSendFileMutex, OldStatus);
    pDelNode = pQueue->Head;

    if(pReq != NULL)
        memcpy(pReq, &pDelNode->Data, sizeof(QUEUE_DATA));

    if(pQueue->Head == pQueue->Tail)
    {
        pQueue->Head = pQueue->Tail = NULL;
    }
    else
    {
        pQueue->Head = pDelNode->Next;
    }

    sdram_free(0, pDelNode);
    //PTHREAD_MUTEX_SAFE_UNLOCK(FtpSendFileMutex, OldStatus);
    return TRUE;
}


