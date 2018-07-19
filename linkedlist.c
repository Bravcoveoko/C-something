#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//typedef struct{
//    int age;
//    char name[50];

//}Kocka;


typedef struct Node{
    int value;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct{
    Node *head;
    Node *tail;
} DoubleLinkedList;

DoubleLinkedList* createList(){
    DoubleLinkedList *list = (DoubleLinkedList*)malloc(sizeof( DoubleLinkedList));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void addNode(DoubleLinkedList *list, int value){
    Node *node = (Node*)malloc(sizeof(Node));
    Node *old = list->head;
    node->value = value;
    node->next = old;
    node->prev = NULL;

    if (old == NULL){
        list->tail = node;
    }
    else{
        old->prev = node;
    }
    list->head = node;
}

void addNodeLast(DoubleLinkedList *list, int value){
    Node *node = (Node*)malloc(sizeof(Node));
    Node *old = list->tail;
    node->value = value;
    node->prev = old;
    node->next = NULL;

    if (old == NULL){
        list->head = node;
    }
    else{
        old->next = node;
    }
    list->tail = node;

}

void deleteNode(DoubleLinkedList *list, int value){
    Node *node = list->head;
    while (node != NULL){
        if (node->value == value){
            deleteByNode(list, node);
        }
        node = node->next;
    }
}

void deleteByNode(DoubleLinkedList *list, Node *node){
    Node *oldNext = node->next;
    Node *oldPrev = node->prev;

    if (oldNext != NULL){
        oldNext->prev = oldPrev;
    }else{
        list->tail = oldPrev;
    }

    if (oldPrev != NULL){
        oldPrev->next = oldNext;
    }else{
        list->head = oldNext;
    }

    free(node);
}

Node *findFirst(DoubleLinkedList *list, int value){
    Node *node = list->head;
    while (node != NULL){
        if (node->value == value){
            return node;
        }
        node = node->next;
    }
    return NULL;

}

void destroyList(DoubleLinkedList *list){
    Node *node = list->head;
    while(node != NULL){
        Node *tmp = node->next;
        free(node);
        node = tmp;
    }
    free(list);
}

void print(DoubleLinkedList *list){
    Node *node = list->head;
    while(node != NULL){
        printf("%d --> ", node->value);
        node = node->next;
    }
    printf("\n");
}

int main(void)
{
//    printf("DF\n");
    int a[] = {5, 8, 6, 7, 6};
    DoubleLinkedList *list = createList();
    for (int i = 0; i < sizeof(a)/sizeof(a[0]); i++){
       addNode(list, a[i]);
    }
    print(list);
    Node *f = findFirst(list, 6);
    if (!f){
        printf("NULL\n");
    }
    else{
        printf("Najdeny: %d\n", f->value);
        deleteNode(list, 6);
    }
    addNodeLast(list, 100);
    print(list);
    destroyList(list);
    return 0;
}

