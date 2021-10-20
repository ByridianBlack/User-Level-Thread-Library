int mypthread_prior_queue_enqueue(mypthread_queue **front, struct threadControlBlock* pthread_item){

	if(*front == NULL){
		*front = malloc(sizeof(mypthread_queue));
		(*front)->context = pthread_item;
		(*front)->next = NULL;
		return 0;
	}

    mypthread_queue* cursor = *front;
    mypthread_queue* prev = NULL;

    if(pthread_item->quantum_count <= (*front)->context->quantum_count){
        mypthread_queue *temp = malloc(sizeof(mypthread_queue));
        temp->next = (*front)->next;
        (*front)->next = temp;
        temp->context = (*front)->context;
        (*front)->context = pthread_item;
        return 0;
    }

    while(cursor != NULL){
        if(pthread_item->quantum_count <= cursor->context->quantum_count){
            mypthread_queue *temp = malloc(sizeof(mypthread_queue));
            temp->context = pthread_item;
            prev->next = temp;
            temp->next = cursor;
            return 0;
        }   
        prev = cursor;
        cursor = cursor->next;
    }
    
    cursor = malloc(sizeof(mypthread_queue));
    cursor->next = NULL;
    cursor->context = pthread_item;
    prev->next = cursor;

	return -1;
}