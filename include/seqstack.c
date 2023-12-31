#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "seqstack.h"
#include "seqlist.h"

/**
 * @brief SeQStack 栈的顺序存储
 */

//创建栈 相当于 创建线性表
SeQStack* SeqStack_Create(int capacity)
{
    return SeqList_Create(capacity);
}

//销毁栈 相当于 销毁线性表
void SeqStack_Destroy(SeQStack* stack)
{
    SeqList_Destroy(stack);
}

//清空栈 相当于 清空线性表
void SeqStack_Clear(SeQStack* stack)
{
    SeqList_Clear(stack);
}

//向栈中压入元素，相当于 向链表的尾部插入元素
int SeqStack_Push(SeQStack* stack, void* item)
{
    return SeqList_Insert(stack,item,SeqList_Length(stack)); //尾部插入
}

//从栈中弹出元素 相当于 从链表的尾部拿出元素
void* SeqStack_Pop(SeQStack* stack)
{
    return SeqList_Delete(stack,SeqList_Length(stack)-1);
}

//获取栈顶元素 相当于 获取链表的尾部元素
void* SeqStack_Top(SeQStack* stack)
{
    return SeqList_Get(stack, SeqList_Length(stack)-1);
}

//获取栈的大小 相当于 获取链表的实际长度
int SeqStack_Size(SeQStack* stack)
{
    return SeqList_Length(stack);
}

//获取栈的容量，相当于 获取线性表的容量
int SeqStack_Capacity(SeQStack* stack)
{
    return SeqList_Capacity(stack);
}

