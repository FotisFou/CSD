#include "structs.h"

int N = 0;

LinkedList *init_DLL(){

    struct LinkedList *dummy = (struct LinkedList*)malloc(sizeof(struct LinkedList));
    dummy->head = (struct DLLNode*)malloc(sizeof(struct DLLNode));
    dummy->tail = (struct DLLNode*)malloc(sizeof(struct DLLNode));

    dummy->head->productID = LONG_MIN;
    dummy->tail->productID = LONG_MAX;

    dummy->head->next = dummy->tail;
    dummy->tail->prev = dummy->head;
    dummy->head->prev = NULL;
    dummy->tail->next = NULL;

    pthread_mutex_init(&dummy->head->lock, NULL);
    pthread_mutex_init(&dummy->tail->lock, NULL);
    

    return dummy;
}


int ListInsert(int productID,  LinkedList *list){

    DLLNode *pred, *curr; /*for lazy-sync*/ 
    int result;  
   

    pthread_mutex_lock(&list->head->lock);
    pred = list->head;
    curr = pred->next;
    pthread_mutex_lock(&curr->lock);
    
    while(curr->productID < productID){
        pthread_mutex_unlock(&pred->lock);
        pred= curr;
        curr = curr->next;
        pthread_mutex_lock(&curr->lock);
    }
    if(productID == curr->productID) result = FALSE;
    else{
        DLLNode *node = (DLLNode*)malloc(sizeof(DLLNode));
        pthread_mutex_init(&node->lock, NULL);
        node->next = curr;
        curr->prev = node;
        node->productID = productID;
        pred->next = node;
        node->prev = pred;
        result = TRUE;
    }
    pthread_mutex_unlock(&pred->lock);
    pthread_mutex_unlock(&curr->lock);
    return result;
 
}

int ListDelete(int productID){
    DLLNode *pred, *curr;
    int result;

    pthread_mutex_lock(&product_list->head->lock);
    pred = product_list->head;
    if(pred->productID== productID){
        product_list->head = pred->next;
        pthread_mutex_unlock(&pred->lock);
        return TRUE;
    }
    curr = pred->next;
    pthread_mutex_lock(&curr->lock);
    while(curr->productID < productID){
        pthread_mutex_unlock(&pred->lock);
        pred = curr;
        curr = curr->next;
        pthread_mutex_lock(&curr->lock);

    }
    if(productID == curr->productID){
        pred->next = curr->next;
        if(curr->next !=NULL){
            curr->next->prev = pred;
        }
        result=TRUE;
    }else result=FALSE;

    pthread_mutex_unlock(&pred->lock);
    pthread_mutex_unlock(&curr->lock);

    return result;
}

int squareof(int num){
    return num*num;
}

void evaluate_list(){
    printf("\n -List Evaluation- \n");
    int counter=-1, err_flag; 
    long int keysum=0;
    DLLNode *node = product_list->head;
    while( node->next !=NULL){
        counter++;
        keysum = keysum + node->productID;
        node = node->next;
    }
    long int expected_keysum = (squareof(N)*(squareof(N) - 1))/2;
    if(counter!=squareof(N)){
        printf("Error.Evaluation failed\n");
        exit(0);
    }
    
    printf("List size check(expected: %d, found: %d)\n", squareof(N), counter);
    printf("List keysum check(expected: %d, found: %d)\n", expected_keysum, keysum);

}

void evaluate_list_del(){
    printf("\n -List Evaluation- \n");
    int counter=-1; 
    DLLNode *node = product_list->head;
    while( node->next !=NULL){
        counter++;
        node = node->next;
    }
    
  
    printf("List size check(expected: %d, found: %d)\n", squareof(N)/3, counter);
    

}

void print_DLL(){
    DLLNode *print_node = product_list->head;

       
       while(print_node->next !=NULL){
           printf("Product---> %d\n", print_node->productID);
           print_node = print_node->next;
       } 
}

//HASH_TABLE

int is_prime(int n){
    int i, flag=1;
    for(i =2; i<n/2; ++i){
        if(n%i== 0) flag=0;
    
    }
    if(flag==0)return FALSE;
    if(flag==1)return TRUE;
}

int get_size(int n){
    while(!is_prime(n)){
        n++;
    }
    return n;
}


void init_HT(int N){
   
   int M = N/3;
   int i,j;
   HTables = (HTNode***)malloc(M*sizeof(HTNode**));
   for(i=0;i<M;i++){
       HTables[i] = (HTNode**)malloc(get_size(3*N)*sizeof(HTNode*));
   }

   for(i=0;i<M;i++){
       for(j=0;j<get_size(3*N);j++){
           HTables[i][j] = (HTNode*)malloc(sizeof(HTNode));
           HTables[i][j]->productID = -1;
           pthread_mutex_init(&HTables[i][j]->lock, NULL);
       }
   }

}

void print_HT(){
    int i, j;
    for(i=0;i<N/3;i++){
        printf("HT[%d]\n", i);
       for(j=0;j<get_size(3*N);j++){
           if(HTables[i][j]->productID !=-1){
                printf("[%d] " , HTables[i][j]->productID);
           }
       }
       printf("\n");
   }
}

int get_prime(){
    int size = get_size(3*N);
    int i, j;
    for(i=size-1; i>=1; i-- ){
        int flag=0;
        for(j=2; j<=(int)sqrt(i); j++){
            if( i%j == 0){
                flag++;
            }
        }
        if(flag==0){
            return i;
        }
    }
    return 3;
}
int get_max_prime(int n){
    while(!is_prime(n)){
        n--;
    }
    return n;
}

int hash1(int key){
    int size = get_size(3*N);
    return( key%size );
}

int hash2(int key){
     int size = get_size(3*N);
     int prime = get_max_prime(size);

  //   return (prime - (key%prime));
    return((key%11) + 1);
}



int insertHT(int productID, int num_HT, int thread_id){
    
    int tmp = productID;
    int index = hash1(productID);
    int old_index = 0;
  //  printf("Thread %d started\n", thread_id);
    pthread_mutex_lock(&HTables[num_HT][index]->lock);
    
    while(HTables[num_HT][index]->productID !=-1){
        old_index = index;
       
        index = (index + hash2(tmp)) % get_size(3*N);
       
        pthread_mutex_unlock(&HTables[num_HT][old_index]->lock);
        pthread_mutex_lock(&HTables[num_HT][index]->lock);
        
    }

    HTables[num_HT][index]->productID = productID;
    pthread_mutex_unlock(&HTables[num_HT][index]->lock);
  
    return TRUE;
        
}

int delete_HT(int productID, int num_HT){
   
   int index=0;
   pthread_mutex_lock(&HTables[num_HT][0]->lock);
   while(HTables[num_HT][index]->productID !=productID){
    
       index++;
       if(index == get_size(3*N)){
           pthread_mutex_unlock(&HTables[num_HT][0]->lock);
           return FALSE;
       }
    
   }
  
    HTables[num_HT][index]->productID =-1;
    pthread_mutex_unlock(&HTables[num_HT][0]->lock);
   return TRUE;
}

int evaluate_HT(){
    int i , z, j, counter;
    long int keysum=0;
    printf("\n -HT Evaluation- \n");

    for(i=0;i<N/3;i++){
        printf("HT[%d] size check", i);
        counter=0;
        
        for(z=0;z<get_size(3*N);z++){
            
            if(HTables[i][z]->productID!=-1){
                counter++;
                keysum += HTables[i][z]->productID;
            }

        }
        printf("(expected: %d , found:%d)\n", 3*N , counter);
    }
    long int expected_keysum = (squareof(N)*(squareof(N) -1))/2; 
    printf("HT keysum check (expected:%d , found: %d)\n",expected_keysum, keysum);
    
}

int evaluate_HT_del(){
    int i , z,counter;
    printf("\n -HT DELETE valuation- \n");

    for(i=0;i<N/3;i++){
        printf("HT[%d] size check", i);
        counter=0;
        
        for(z=0;z<get_size(3*N);z++){            
            if(HTables[i][z]->productID!=-1){
                counter++;
            }
        }
        printf("(expected: %d , found:%d)\n", 2*N , counter);
    }
      
}

// STACK

stackNode *newCell( int productID){
    stackNode *node = (stackNode*)malloc(sizeof(stackNode));
    node->productID = productID;
    return node;
}

void Backoff(){
    static int limit = MIN;
    int delay =(rand()%( MAX-MIN +1)) +MIN;

    if(MAX<2*limit){
        limit = MAX;
    }else{
        limit = 2*limit;
    }
    usleep(delay);
}

int TryPush( stackNode *node){
    stackNode *oldTop = Top;
    node->next = oldTop;

    if(__sync_bool_compare_and_swap((long*)&Top, (long*)oldTop,(long*)node)){
        return TRUE;
    }else{
        return FALSE;
    }
}

void push(int productID){
    stackNode *node = newCell(productID);
    while(TRUE){
        if(TryPush(node)){ 
            return;
        }else{
            Backoff();
        }
    }
}

stackNode *TryPop(){
    stackNode *oldTop = Top;
    stackNode *newTop;
    if(oldTop ==NULL){
        return NULL;
    }
    if(oldTop->next!=NULL){
        newTop = oldTop->next;
    }
    

    if(__sync_bool_compare_and_swap((long*)&Top,(long*)oldTop,(long*)newTop)){
        return oldTop;
    }else{
        return NULL;
    }
}

int pop(){
    stackNode *rn;
    while(TRUE){
        rn =TryPop();
        if(rn==NULL){
            return -1;
        }
        if(rn !=NULL){
            return rn->productID ;
        }else{
            Backoff();
        }
    }
}

void print_stack(){
    stackNode *printNode = Top;
    printf("STACK: ");
    while(printNode !=NULL){
        printf("[%d]\t", printNode->productID);
        printNode = printNode->next;
    }
    printf("\n");
}

void evaluate_stack(){
    int counter=0;
    stackNode *Node=Top;
    while(Node->next !=NULL){
        counter++;
        Node = Node->next;
    }
    int expected_size = squareof(N)/3;
    printf("\n -Stack Evaluation- \n");
    printf("Stack size check(expected:%d , found:%d)\n", expected_size, counter);

}


void *th_func(void* input){
    int i= 0, j=0, z=0;
    args *myArgs = (args *)input;
    // Insert in DLL
    for (i=0;i<myArgs->N;i++){
        int product_ID =  i*myArgs->N + myArgs->j;
        
        ListInsert(product_ID, product_list);
    }
    pthread_barrier_wait(&barrier);
    
    // list evaluation 
    if(myArgs->j == 0) {
       evaluate_list();
     
    } 
    pthread_barrier_wait(&barrier);

    // Insert in HT
    int M = myArgs->N/3;
    
    myArgs->first_ID= myArgs->N*myArgs->j; // first product sold id
    myArgs->last_ID = (myArgs->N*myArgs->j) + (myArgs->N-1);
    myArgs->HT_index = (myArgs->j)%M;
  
       
    for(j= myArgs->first_ID; j<=myArgs->last_ID; j++){
        ListDelete(j);
        insertHT(j, (myArgs->HT_index++)%M, myArgs->j);    
    }
    
    pthread_barrier_wait(&barrier);
    
    // HT evaluation 
    if(myArgs->j == 0) {
       evaluate_HT();
       
    } 
    pthread_barrier_wait(&barrier);

    //3rd phase
    for(i=0;i<M;i++){
        myArgs->product =myArgs->j;
        while(!delete_HT(myArgs->product, (myArgs->HT_index)%M)){
            myArgs->product +=2;
           
        }
        push(myArgs->product);
        myArgs->HT_index++;
            
    }
    pthread_barrier_wait(&barrier);
    if(myArgs->j == 0) {
       evaluate_HT_del(); 
       evaluate_stack(); 
       
     //  print_DLL();
    } 
    pthread_barrier_wait(&barrier);


    if(myArgs->j == 0){
        int tmp= pop();
        while(TRUE){
           if(tmp<0) break;
           ListInsert(tmp, product_list); 
           tmp = pop();
        }
        evaluate_list_del();    
    }   
}



int main(int argc, char* argv[]){

    if( argc == 2){
             
       N = atoi(argv[1]);
       int M = N/3;
        
       pthread_t threads[N];      
       product_list = init_DLL();
       
       init_HT(N);
       Top = (stackNode*)malloc(sizeof(stackNode));
       Top->productID=-1;
    
       int j =0; 
       pthread_barrier_init(&barrier, NULL, N);
       for(j=0; j<N; j++){ 
            args *myArgs = (args*)malloc(sizeof(args));
             myArgs->N = N;
           myArgs->j = j;
           pthread_create(&(threads[j]),NULL,th_func,(void*)myArgs);
       }
       for(j=0; j<N; j++){
           pthread_join(threads[j], NULL);
       }
       
    }
   
}