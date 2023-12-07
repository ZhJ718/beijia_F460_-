#ifndef SEQSTACK_H
#define SEQSTACK_H

/**
 * @brief SeQStack ջ��˳��洢
 */
typedef void SeqStack;

SeQStack* SeqStack_Create(int capacity);

void SeqStack_Destroy(SeQStack* stack);

void SeqStack_Clear(SeQStack* stack);

int SeqStack_Push(SeQStack* stack, void* item);

void* SeqStack_Pop(SeQStack* stack);

void* SeqStack_Top(SeQStack* stack);

int SeqStack_Size(SeQStack* stack);

//��ȡջ���������൱�� ��ȡ���Ա������
int SeqStack_Capacity(SeQStack* stack);


#endif // SEQSTACK_H
