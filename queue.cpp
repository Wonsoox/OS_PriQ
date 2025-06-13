#include <iostream>

#include "queue.h"

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
    std::lock_guard<std::mutex> lock(queue->mtx);
    Reply reply = {false, {}};

    if (!queue->head) return reply;

    Node* node = queue->head;
    queue->head = node->next;

    reply.success = true;
    reply.item.key = node->item.key;
    reply.item.value_size = node->item.value_size;
    if (node->item.value_size > 0 && node->item.value != nullptr) {
        reply.item.value = new char[node->item.value_size];
        std::memcpy(reply.item.value, node->item.value, node->item.value_size);
    } else {
        reply.item.value = nullptr;
    }

    nfree(node);
    return reply;
}

Queue* range(Queue* queue, Key start, Key end) {
    Queue* result = init();
    std::lock_guard<std::mutex> lock(queue->mtx);

    Node* cur = queue->head;
    Node** tail = &result->head;

    while (cur) {
        Key k = cur->item.key;
        if (k > end) break;

        if (k >= start) {
            Node* copy = new Node();
            copy->item.key = cur->item.key;
            copy->item.value_size = cur->item.value_size;
            if (cur->item.value_size > 0 && cur->item.value != nullptr) {
                copy->item.value = new char[cur->item.value_size];
                std::memcpy(copy->item.value, cur->item.value, cur->item.value_size);
            } else {
                copy->item.value = nullptr;
            }
            copy->next = nullptr;
            *tail = copy;
            tail = &copy->next;
        }
        cur = cur->next;
    }

    return result;
}