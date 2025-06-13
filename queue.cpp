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


// Reply dequeue(Queue* queue) {
//     std::lock_guard<std::mutex> lock(queue->mtx);
//     Reply reply = {false, {}};

//     if (!queue->head) return reply;

//     Node* node = queue->head;
//     queue->head = node->next;

//     reply.success = true;
//     reply.item.key = node->item.key;
//     reply.item.value_size = node->item.value_size;
//     if (node->item.value_size > 0 && node->item.value != nullptr) {
//         reply.item.value = new char[node->item.value_size];
//         std::memcpy(reply.item.value, node->item.value, node->item.value_size);
//     } else {
//         reply.item.value = nullptr;
//     }

//     nfree(node);
//     return reply;
// }

// Queue* range(Queue* queue, Key start, Key end) {
//     Queue* result = init();
//     std::lock_guard<std::mutex> lock(queue->mtx);

//     Node* cur = queue->head;
//     Node** tail = &result->head;

//     while (cur) {
//         Key k = cur->item.key;
//         if (k > end) break;

//         if (k >= start) {
//             Node* copy = new Node();
//             copy->item.key = cur->item.key;
//             copy->item.value_size = cur->item.value_size;
//             if (cur->item.value_size > 0 && cur->item.value != nullptr) {
//                 copy->item.value = new char[cur->item.value_size];
//                 std::memcpy(copy->item.value, cur->item.value, cur->item.value_size);
//             } else {
//                 copy->item.value = nullptr;
//             }
//             copy->next = nullptr;
//             *tail = copy;
//             tail = &copy->next;
//         }
//         cur = cur->next;
//     }

//     return result;
// }