#pragma once

typedef int Key;
typedef int Value;

typedef struct {
    Key key;
    Value value;
} Item;

typedef struct node_t {
    Item item;
    struct node_t* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
} Queue;

typedef struct {
    bool success;
    Item item;
} Reply;