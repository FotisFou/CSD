#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

#define MIN 5
#define MAX 15


typedef struct DLLNode{
    long int productID;
    pthread_mutex_t lock;
    struct DLLNode *next;
    struct DLLNode *prev; 
} DLLNode;

typedef struct LinkedList{
    struct DLLNode *head;
    struct DLLNode *tail;
} LinkedList;

typedef struct HTNode{
    int productID;
    pthread_mutex_t lock;
} HTNode;

typedef struct stackNode{
    int productID;
    struct stackNode *next;
} stackNode;

int N;
LinkedList *product_list;
pthread_barrier_t barrier;
HTNode ***HTables;
stackNode *Top;





typedef struct args{
    int j;
    int N;
    int HT_index;
    int first_ID;
    int last_ID;
    int counter;
    int product;
} args;