#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "linkqueue.h"
#include "linklist.h"

/**
 * @brief LinkQueue ���е���ʽ�洢
 */

//����Ҳ��һ����������Ա�
//���е�ҵ��ڵ�����ݽṹ
typedef struct _tag_LinkQueueNode
{
    LinkListNode node;
    void *item;
}TLinkQueueNode;


//�������� �൱�ڴ������Ա�
LinkQueue* LinkQueue_Create(void)
{
    return LinkList_Create();
}

//���ٶ��� �൱�� �������Ա�
//�ڵ���ڴ����
void LinkQueue_Destroy(LinkQueue* queue)
{
    LinkQueue_Clear(queue);
    LinkList_Destroy(queue);
}

//�����ն��� ��Ҫ��ʾ�İѶ����е����нڵ�����
//�ͷ�ÿһ���ڵ�
void LinkQueue_Clear(LinkQueue* queue)
{
    while(LinkQueue_Length(queue) > 0)
    {
        LinkQueue_Retrieve(queue);
    }

    LinkList_Clear(queue);
}

//����������Ԫ�� �൱�� �����Ա��β�����Ԫ��
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
    memset(tmp, 0, sizeof (TLinkQueueNode));//��һ�������ε���
   // tmp->node.next = NULL;
    tmp->item = item;

    //��Ҫ��ջ��item(ջ��ҵ��ڵ�)ת�� ������� LinkListNode
    ret = LinkList_Insert(queue, (LinkListNode *)tmp, LinkList_Length(queue));
    if(ret != 0)
    {
        //printf("func LinkList_Insert() err:%d \n", ret);
       // if(tmp != NULL) sdram_free(0,tmp);
        return ret;
    }
    return ret;
}

//�Ӷ�����ɾ��Ԫ�� �൱�� �����Ա��ͷ��ɾ��Ԫ��
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

    //ɾ��֮ǰ����
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

//��ȡ����ͷ��Ԫ�� �൱�ڴ����Ա��0��λ�� ������
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

//����еĳ��� �൱�� �����Ա�ĳ���
int LinkQueue_Length(LinkQueue* queue)
{
    return LinkList_Length(queue);
}


