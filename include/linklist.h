#ifndef _MYLINKLIST_H
#define _MYLINKLIST_H

#include "malloc.h"

/**
 * @brief LinkList ���Ա���ʽ�洢����ƺ�ʵ��
 */

typedef void LinkList;

typedef struct _tag_LinkListNode
{
    struct _tag_LinkListNode* next;
}LinkListNode;


LinkList* LinkList_Create(void);


void LinkList_Destroy(LinkList* list);


void LinkList_Clear(LinkList* list);


int LinkList_Length(LinkList* list);

int LinkList_Insert(LinkList* list, LinkListNode* node, int pos);


LinkListNode* LinkList_Get(LinkList* list, int pos);


LinkListNode* LinkList_Delete(LinkList* list, int pos);



#endif // LINKLIST_H
