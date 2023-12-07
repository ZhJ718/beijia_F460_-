#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "linkqueue.h"
#include "linklist.h"

/**
 * @brief LinkQueue 队列的链式存储
 */

//队列也是一种特殊的线性表
//队列的业务节点的数据结构
typedef struct _tag_LinkQueueNode
{
    LinkListNode node;
    void *item;
}TLinkQueueNode;


//创建队列 相当于创建线性表
LinkQueue* LinkQueue_Create(void)
{
    return LinkList_Create();
}

//销毁队列 相当于 销毁线性表
//节点的内存管理
void LinkQueue_Destroy(LinkQueue* queue)
{
    LinkQueue_Clear(queue);
    LinkList_Destroy(queue);
}

//如果清空队列 需要显示的把队列中的所有节点搞出来
//释放每一个节点
void LinkQueue_Clear(LinkQueue* queue)
{
    while(LinkQueue_Length(queue) > 0)
    {
        LinkQueue_Retrieve(queue);
    }

    LinkList_Clear(queue);
}

//向队列中添加元素 相当于 向线性表的尾部添加元素
int LinkQueue_Append(LinkQueue* queue, void* item)
{
    int ret = 0;
    TLinkQueueNode *tmp = NULL;

    tmp = (TLinkQueueNode *)sdram_malloc(0,sizeof(TLinkQueueNode));
    if(tmp == NULL)
    {
        ret = -1;
        //printf("func LinkQueue_Append() malloc err: %d ",ret);
        return ret;
    }
    memset(tmp, 0, sizeof (TLinkQueueNode));//做一个负责任的人
   // tmp->node.next = NULL;
    tmp->item = item;

    //需要把栈的item(栈的业务节点)转化 成链表的 LinkListNode
    ret = LinkList_Insert(queue, (LinkListNode *)tmp, LinkList_Length(queue));
    if(ret != 0)
    {
        //printf("func LinkList_Insert() err:%d \n", ret);
       // if(tmp != NULL) sdram_free(0,tmp);
        return ret;
    }
    return ret;
}

//从队列中删除元素 相当于 从线性表的头部删除元素
void* LinkQueue_Retrieve(LinkQueue* queue)
{
    TLinkQueueNode *tmp = NULL;
    void *ret = NULL;
    tmp = (TLinkQueueNode *) LinkList_Delete(queue, 0);
    if(tmp == NULL)
    {
        //printf("func LinkList_Delete() err\n");
        return NULL;
    }

    //删除之前缓存
    ret = tmp->item;

    if(ret != NULL)
    {
        sdram_free(0,ret);
    }
    
    if(tmp != NULL)
    {
        sdram_free(0,tmp);
    }
    
    return ret;
}

//获取队列头部元素 相当于从线性表的0号位置 拿数据
void* LinkQueue_Header(LinkQueue* queue)
{
    TLinkQueueNode *tmp = NULL;
    void *ret = NULL;

    tmp =(TLinkQueueNode *)LinkList_Get(queue, 0);
    if(tmp == NULL)
    {
        //printf("func LinkList_Get() err\n");
        return NULL;
    }
    return tmp->item;
}

//求队列的长度 相当于 求线性表的长度
int LinkQueue_Length(LinkQueue* queue)
{
    return LinkList_Length(queue);
}


