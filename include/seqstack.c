#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "seqstack.h"
#include "seqlist.h"

/**
 * @brief SeQStack ջ��˳��洢
 */

//����ջ �൱�� �������Ա�
SeQStack* SeqStack_Create(int capacity)
{
    return SeqList_Create(capacity);
}

//����ջ �൱�� �������Ա�
void SeqStack_Destroy(SeQStack* stack)
{
    SeqList_Destroy(stack);
}

//���ջ �൱�� ������Ա�
void SeqStack_Clear(SeQStack* stack)
{
    SeqList_Clear(stack);
}

//��ջ��ѹ��Ԫ�أ��൱�� �������β������Ԫ��
int SeqStack_Push(SeQStack* stack, void* item)
{
    return SeqList_Insert(stack,item,SeqList_Length(stack)); //β������
}

//��ջ�е���Ԫ�� �൱�� �������β���ó�Ԫ��
void* SeqStack_Pop(SeQStack* stack)
{
    return SeqList_Delete(stack,SeqList_Length(stack)-1);
}

//��ȡջ��Ԫ�� �൱�� ��ȡ�����β��Ԫ��
void* SeqStack_Top(SeQStack* stack)
{
    return SeqList_Get(stack, SeqList_Length(stack)-1);
}

//��ȡջ�Ĵ�С �൱�� ��ȡ�����ʵ�ʳ���
int SeqStack_Size(SeQStack* stack)
{
    return SeqList_Length(stack);
}

//��ȡջ���������൱�� ��ȡ���Ա������
int SeqStack_Capacity(SeQStack* stack)
{
    return SeqList_Capacity(stack);
}

