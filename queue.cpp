#include <iostream>
#include "qtype.h"
#include "queue.h"

Queue* init(void) {
    Queue* q = new Queue();
    q->head = nullptr;
    return q;
}

void release(Queue* queue) {
	return;
}


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
    Reply reply = {false, item};

    Node* prev = nullptr;
    Node* cur = queue->head;

    while (cur && cur->item.key < item.key) {
        prev = cur;
        cur = cur->next;
    }

    
    if (cur && cur->item.key == item.key) {
        delete[] (char*)cur->item.value;
        cur->item.value = new char[item.value_size];
        std::memcpy(cur->item.value, item.value, item.value_size);
        cur->item.value_size = item.value_size;

        reply.success = true;
        reply.item = cur->item;
        return reply;
    }

    
    Node* new_node = nalloc(item);
    if (!prev) {
        new_node->next = queue->head;
        queue->head = new_node;
    } else {
        new_node->next = cur;
        prev->next = new_node;
    }

    reply.success = true;
    reply.item = new_node->item;
    return reply;
}


Reply dequeue(Queue* queue) {
	Reply reply = { false, NULL };
	return reply;
}

Queue* range(Queue* queue, Key start, Key end) {
	return NULL;
}
