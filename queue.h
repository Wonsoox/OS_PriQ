#include "qtype.h"
// 큐 초기화 및 해제
Queue* init(void);
void release(Queue* queue);

// 노드 생성, 초기화, 해제 및 복제
Node* nalloc(Item item);
void nfree(Node* node);
Node* nclone(Node* node);

// 큐 작업
Reply enqueue(Queue* queue, Item item);
Reply dequeue(Queue* queue);
Queue* range(Queue* queue, Key start, Key end);