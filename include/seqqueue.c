#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "seqqueue.h"
#include "seqlist.h"

/**
 * @brief SeqQueue 队列的顺序存储
 */

//队列也是一种特殊的线性表

// 创建一个队列 相当于 创建一个顺序存储线性表
SeqQueue* SeqQueue_Create(int capacity)
{
    return SeqList_Create(capacity);
}

//销毁队列 相当于销毁一个线性表
void SeqQueue_Destroy(SeqQueue* queue)
{
    SeqList_Destroy(queue);
}

//清空队列 相当于 清空线性表
void SeqQueue_Clear(SeqQueue* queue)
{
    SeqList_Clear(queue);
}

// 向队列中添加元素 相当于 向线性表中添加一个元素
int SeqQueue_Append(SeqQueue* queue, void* item)
{
    return SeqList_Insert(queue, item, SeqList_Length(queue));
}

// 出队列 相当于 从线性表中删除0号位置元素
void* SeqQueue_Retrieve(SeqQueue* queue)
{
    return SeqList_Delete(queue,0);
}

void* SeqQueue_Header(SeqQueue* queue)
{
    return SeqList_Get(queue, 0);
}

int SeqQueue_Length(SeqQueue* queue)
{
    return SeqList_Length(queue);
}

int SeqQueue_Capacity(SeqQueue* queue)
{
    return SeqList_Capacity(queue);
}
