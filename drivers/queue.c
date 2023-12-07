/******************************************************************
*��Ȩ���� (C)2019, ��������Ƽ��Ͼ��ֹ�˾
*
*�ļ�����:  queue.c
*�ļ���ʶ:
*����ժҪ:
*����˵��:
*��ǰ�汾:
*��    ��:
*�������:  2019/5/5
*
*�޸ļ�¼1:
*    �޸�����:
*    �� �� ��:
*    �� �� ��:xfw
*    �޸�����:
******************************************************************/

/******************************************************************
*                             ͷ�ļ�                              *
******************************************************************/
#include "queue.h"
#include "malloc.h"

/******************************************************************
*                           ȫ�ֱ�������                          *
******************************************************************/


/*************************************************************************************************************************************************************************
 * ����
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

    //DPrintf("@@@@@@@@@@ ׼���ͷŶ�����ռ�õĿռ�...\r\n");
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
 *��������: QueuePush
 *��������: ������������
 *��������: ���С��ڵ�����
 *�������: ��
 *��������: TRUE�ɹ�/FALSEʧ��
 *��������: XFW
 *ȫ������: ��
 *����˵��: ��
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
 *��������: QueuePop
 *��������: �Ӷ����е�������
 *��������: ����
 *�������: �ڵ�����
 *��������: TRUE�ɹ�/FALSEʧ��
 *��������: XFW
 *ȫ������: ��
 *����˵��:
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


