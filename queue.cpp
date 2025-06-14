#include <iostream>

#include "queue.h"

#define MAX_SIZE 100000

Queue* init(void) {
    Queue* q = new Queue();
    q->size = 0;
    return q;
}

void release(Queue* queue) {
    for (int i = 0; i < queue->size; ++i) {
        delete[] static_cast<char*>(queue->heap[i].value);
    }
    delete queue;
}



//아래는 힙구현 관련 함수가 들어올 예정 (down,up)
void heapify_up(Queue* q, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (q->heap[parent].key <= q->heap[idx].key) break;
        std::swap(q->heap[parent], q->heap[idx]);
        idx = parent;
    }
}

void heapify_down(Queue* q, int idx) {
    int smallest = idx;
    while (true) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;

        if (left < q->size && q->heap[left].key < q->heap[smallest].key)
            smallest = left;
        if (right < q->size && q->heap[right].key < q->heap[smallest].key)
            smallest = right;

        if (smallest == idx) break;

        std::swap(q->heap[smallest], q->heap[idx]);
        idx = smallest;
    }
}



//이 아래의 함수들은 힙기반 우선순위 큐를 구현하는데에는 사용되지 않는다. 고로 맨아래로 위치를 변경한다.
Node* nalloc(Item item) {
	// Node ����, item���� �ʱ�ȭ
	return NULL;
}


void nfree(Node* node) {
	return;
}


Node* nclone(Node* node) {
	return NULL;
}


Reply enqueue(Queue* queue, Item item) {
    std::lock_guard<std::mutex> lock(queue->mtx);
    Reply reply = { false, {0, nullptr, 0} };

    if (queue->size >= MAX_SIZE) return reply;

    //동일 key판별 부분
    for (int i = 0; i < queue->size; ++i) {
        if (queue->heap[i].key == item.key) {
            
            delete[] static_cast<char*>(queue->heap[i].value);

            
            void* new_val = new char[item.value_size];
            memcpy(new_val, item.value, item.value_size);

            queue->heap[i].value = new_val;
            queue->heap[i].value_size = item.value_size;

            reply.success = true;
            reply.item = queue->heap[i];
            return reply;
        }
    }

    //깊은복사구현부분
    Item& new_item = queue->heap[queue->size];
    new_item.key = item.key;
    new_item.value_size = item.value_size;
    new_item.value = new char[item.value_size];
    //
    memcpy(new_item.value, item.value, item.value_size);

    heapify_up(queue, queue->size);

    reply.success = true;
    reply.item = new_item;
    queue->size++;
    return reply;
}


Reply dequeue(Queue* queue) {
    std::lock_guard<std::mutex> lock(queue->mtx);
    Reply reply = { false, {0, nullptr, 0} };

    if (queue->size == 0) return reply;

    Item& top = queue->heap[0];

    // 깊은 복사해서 반환
    reply.success = true;
    reply.item.key = top.key;
    reply.item.value_size = top.value_size;
    reply.item.value = new char[top.value_size];
    memcpy(reply.item.value, top.value, top.value_size);

    // 원본 value 메모리 해제
    delete[] static_cast<char*>(top.value);

    // 힙에서 제거
    queue->heap[0] = queue->heap[--queue->size];
    heapify_down(queue, 0);

    return reply;
}


Queue* range(Queue* queue, Key start, Key end) {
    std::lock_guard<std::mutex> lock(queue->mtx);

    Queue* result = new Queue();
    result->size = 0;

    for (int i = 0; i < queue->size; ++i) {
        Key k = queue->heap[i].key;
        if (k >= start && k <= end) {
            Item& src = queue->heap[i];
            Item& dst = result->heap[result->size];
            dst.key = src.key;
            dst.value_size = src.value_size;
            dst.value = new char[src.value_size];
            memcpy(dst.value, src.value, src.value_size);
            result->size++;
        }
    }
    // 힙 속성 보장 (필요시)
    for (int i = result->size / 2 - 1; i >= 0; --i) {
        heapify_down(result, i);
    }
    return result;
}
