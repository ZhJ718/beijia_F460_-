#ifndef LINKQUEUE_H
#define LINKQUEUE_H

/**
 * @brief LinkQueue 队列的链式存储
 */

typedef void LinkQueue;

LinkQueue* LinkQueue_Create(void);

void LinkQueue_Destroy(LinkQueue* queue);

void LinkQueue_Clear(LinkQueue* queue);

int LinkQueue_Append(LinkQueue* queue, void* item);

void* LinkQueue_Retrieve(LinkQueue* queue);

void* LinkQueue_Header(LinkQueue* queue);

int LinkQueue_Length(LinkQueue* queue);


#endif // LINKQUEUE_H
