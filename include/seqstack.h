#ifndef SEQSTACK_H
#define SEQSTACK_H

/**
 * @brief SeQStack 栈的顺序存储
 */
typedef void SeqStack;

SeQStack* SeqStack_Create(int capacity);

void SeqStack_Destroy(SeQStack* stack);

void SeqStack_Clear(SeQStack* stack);

int SeqStack_Push(SeQStack* stack, void* item);

void* SeqStack_Pop(SeQStack* stack);

void* SeqStack_Top(SeQStack* stack);

int SeqStack_Size(SeQStack* stack);

//获取栈的容量，相当于 获取线性表的容量
int SeqStack_Capacity(SeQStack* stack);


#endif // SEQSTACK_H
