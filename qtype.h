#pragma once
#include <mutex>  
typedef int Key;
typedef int Value;

typedef struct {
    Key key;
    void* value;
    int value_size;
} Item;


typedef struct node_t {
    Item item;
    struct node_t* next;
} Node;

typedef struct {
    int size;
    Item heap[100000];
    std::mutex mtx;
} Queue;

typedef struct {
    bool success;
    Item item;
} Reply;