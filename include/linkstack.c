#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "linkstack.h"
#include "linklist.h"

/**
 * @brief LinkStack 栈的链式存储
 */

typedef struct _tag_LinkStackNode  //链表的业务节点
{
    LinkListNode node; //第一个域 （第一个元素）
    void* item; //栈的业务节点

}TLinkStackNode;

//创建栈 相当于 创建一个线性表
LinkStack* LinkStack_Create()
{
    return LinkList_Create();
}

//销毁一个栈 相当于 销毁一个线性表
void LinkStack_Destroy(LinkStack* stack)
{
    LinkStack_Clear(stack);
    LinkStack_Destroy(stack);
}

//清空一个栈 相当于 清空一个线性表
//清空栈的时候 涉及到 栈元素生命周期的管理

//所有入栈的节点都是malloc
//若要清空栈；把栈中的元素弹出，并且释放节点内存
void LinkStack_Clear(LinkStack* stack)
{
    if(stack == NULL)
    {
        return;
    }
    while(LinkStack_Size(stack)>0)
    {
        LinkStack_Pop(stack);  //在这个函数中释放节点的内存
    }
    return ;
}

//向栈中添加元素 相当于 向线性表的头部插入元素
int LinkStack_Push(LinkStack* stack, void* item)
{
    TLinkStackNode *tmp = NULL;
    int ret = 0;

    tmp = (TLinkStackNode*)malloc(sizeof (TLinkStackNode));
    if(tmp == NULL)
    {
        return  -1;
    }
    memset(tmp, 0, sizeof(TLinkStackNode));
    tmp->item = item;

    ret = LinkList_Insert(stack,(LinkListNode*)tmp, 0);
    if(ret !=0 )
    {
        printf("func LinkList_Insert() err:%d \n", ret);
        if(tmp != NULL)
        {
            free(tmp);
        }
        return ret;
    }

    return ret;
}

//从栈中弹出元素，相当于从线性表的头部删除元素
//把线性表的业务节点 转化成 栈的业务节点
void* LinkStack_Pop(LinkStack* stack)
{
    void *item = NULL; //栈的业务节点
    TLinkStackNode *tmp = NULL;
    tmp = (TLinkStackNode*)LinkList_Delete(stack, 0);
    if(tmp == NULL)
    {
        return NULL;
    }
    item = tmp->item;
    //因为LinkList_Insert的时候 分配了内存，所以LinkList_Delete释放内
    free(tmp);
    return item;
}

//获取栈顶元素 相当于 获取线性表的0号位置
void* LinkStack_Top(LinkStack* stack)
{
    TLinkStackNode *tmp = NULL;
    tmp = (TLinkStackNode *)LinkList_Get(stack, 0);
    if(tmp == NULL)
    {
        return NULL;
    }
    return tmp->item;
}

//求栈的大小，相当于求线性表的len
int LinkStack_Size(LinkStack* stack)
{
    return LinkList_Length(stack);
}


