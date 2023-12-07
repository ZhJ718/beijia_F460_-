#ifndef _MY_SEQLIST_H_
#define _MY_SEQLIST_H_

/**
 * @brief SeqList 线性表顺序存储的设计和实现
 */

typedef void SeqList;
typedef void SeqListNode;


SeqList* SeqList_Create(int capacity);


void SeqList_Destroy(SeqList* list);


void SeqList_Clear(SeqList* list);


int SeqList_Length(SeqList* list);


int SeqList_Capacity(SeqList* list);


int SeqList_Insert(SeqList* list, SeqListNode* node, int pos);


SeqListNode* SeqList_Get(SeqList* list, int pos);


SeqListNode* SeqList_Delete(SeqList* list, int pos);

#endif
