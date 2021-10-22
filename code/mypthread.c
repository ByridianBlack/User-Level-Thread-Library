/*
        File:                          mypthread.c
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/

#include "mypthread.h"

#define STACK_SIZE 1892
#define LOCKED 1

// Used to assign thread IDs to threads
mypthread_t threadIDCounter = 0;

struct threadControlBlock *current_running_thread = NULL;

/* 
        Creates a generic tcb struct with context allocated.
        This generic tcb can then be manipulated into either the tcb of
        the current thread (parent) or the new thread (child).
        
        It is the responsibility of the caller to free 
        struct threadControlBlock and struct ucontext_t.
        
        0 : On success
        1 : On failure
*/
int create_new_tcb(struct threadControlBlock **tcb) {
        
        // *tcb must be initialized to NULL
        if (*tcb != NULL) return 1;

        *tcb = malloc(sizeof(struct threadControlBlock));
        if (*tcb == NULL) {
                perror("malloc: ");
                return 1;
        }
        
        (*tcb)->threadID          = threadIDCounter;
        (*tcb)->state             = ready;
        (*tcb)->quantum_count     = 0;
        (*tcb)->restored          = 0;
        
        threadIDCounter++;
        
        struct ucontext_t *context = malloc(sizeof(struct ucontext_t));
        if (context == NULL) {
                perror("malloc: ");
                return 1;
        }
        
        (*tcb)->thread_context = context;
        
        return 0;
}

struct threadControlBlock* mypthread_prior_queue_dequeue(mypthread_queue **front){


	if(*front == NULL){
		return NULL;
	}

	struct threadControlBlock* popped_value = (*front)->context;

	(*front) = (*front)->next;
	return popped_value;
}

/*
	Enqueues the pthread into the queue;
	This function was created and used for the mutex locking 
	and unlocking mechanism. Might be repurposed for other cases.

	returns:
		 0: success
		-1: failure
*/

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


int mypthread_queue_enqueue(mypthread_queue** front, struct threadControlBlock* pthread_item)
{
	if(*front == NULL){
	
		*front = malloc(sizeof(mypthread_queue));
	
		(*front)->next = NULL;
	
		(*front)->context = pthread_item;
	
		return 0;
	}

	mypthread_queue* cursor = front;

	while(cursor->next != NULL){
	
		cursor = cursor->next;
	
	}

	cursor->next = malloc(sizeof(mypthread_queue));
	
	cursor->next->next = NULL;
	
	cursor->next->context = pthread_item;
	
	return 0;
}


struct threadControlBlock* mypthread_queue_dequeue(mypthread_queue **front){
	
	if(*front == NULL){
	
		return NULL;
	
	}
	
	struct threadControlBlock* ret_val = (*front)->context;
	
	*front = (*front)->next;
	return ret_val;
}


/*
	Cleans up the queue if any items remains
*/
void queue_cleanup(mypthread_queue* front)
{
	if(front == NULL)
	{
		return;
	}

	mypthread_queue* cursor = front;
	
	mypthread_queue* temp = NULL;
	
	while(cursor != NULL)
	{
	
		temp = cursor;
	
		cursor = cursor->next;
	
		free(temp->context);
	
		free(temp);
	}
	
	return;
}
/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr,
                      void *(*function)(void*), void * arg) {
        
        struct threadControlBlock *currentThread = current_running_thread;
        struct threadControlBlock *newThread     = NULL;
        int ret;
        
        if (currentThread == NULL) {
                ret = create_new_tcb(&currentThread);
                if (ret != 0) {
                        fprintf(STDERR_FILENO, "create_new_tcb: unable to create tcb structure\
                                                for the current thread\n");
                        return 1;
                }
                
                ret = getcontext(currentThread->thread_context);
                if (ret != 0) {
                        perror("getcontext: ");
                        return 1;
                }
                
                // When the main thread is restored by the scheduler, it will return here.
                // All the work past this has been done, so we should return.
                if (currentThread != NULL && currentThread->restored) {
                        return 1;
                }
        }
        
        ret = create_new_tcb(&newThread);
        if (ret != 0) {
                fprintf(STDERR_FILENO, "create_new_tcb: unable to create tcb structure for the\
                                        new thread\n");
                return 1;
        }
        
        ret = getcontext(newThread->thread_context);
        if (ret != 0) {
                perror("getcontext: ");
                return 1;
        }
        
        void *stack = malloc(STACK_SIZE);
        if (stack == NULL) {
                perror("malloc: ");
                return 1;
        }
        
        newThread->thread_context->uc_link           = NULL;             // Set successor stack to null
        newThread->thread_context->uc_stack.ss_sp    = stack;            // Set the starting address of the stack
        newThread->thread_context->uc_stack.ss_size  = STACK_SIZE;       // Set the size of the stack
        newThread->thread_context->uc_stack.ss_flags = 0;                // Might be necessary
		newThread->threadID = threadIDCounter++;
        // Need to check how arg will be passed to makecontext. Seems to use varargs.
        makecontext(newThread->thread_context, function, 1, arg);
		active_threads[threadIDCounter] = newThread;
        if (current_running_thread == NULL) {
                /* enqueue currentThread*/
        }
        /*enqueue newThread*/
        
        return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int mypthread_yield() {

	// change thread state from Running to Ready
	// save context of this thread to its thread control block
	// wwitch from thread context to scheduler context
	raise(SIGALRM);
	// YOUR CODE HERE
	return 0;
};

/* terminate a thread */
void mypthread_exit(void *value_ptr) {
	// Deallocated any dynamic memory created when starting this thread

	struct threadControlBlock* current_thread_copy = current_running_thread;
	current_thread_copy->state = finished;

	// TELL scheduiler to yield
	
	if(value_ptr != NULL){
		values_returned[current_thread_copy->threadID] = value_ptr;
	}


	// SCHEDULER SHOULD THEN FREE THIS

	// YOUR CODE HERE
};


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {

	// wait for a specific thread to terminate
	// de-allocate any dynamic memory created by the joining thread

	// struct threadControlBlock* block = active

	// YOUR CODE HERE
	return 0;
};

/* initialize the mutex lock 
	return:
		 0 - Success
		-1 - Failure
*/
int mypthread_mutex_init(mypthread_mutex_t *mutex,
                          const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex


	/*
		Mutex not successfully Initialized
	*/
	if(mutex == NULL)
	{
		return -1;
	}

	mutex = malloc(sizeof(mypthread_mutex_t));
	mutex->lock = 0;
	mutex->flag = 0;
	mutex->thread_queue = malloc(sizeof(mypthread_queue));
	mutex->thread_queue->next = NULL;
	mutex->owner_id = 0;
	// YOUR CODE HERE
	return 0;
};

u_int8_t test_and_set(u_int8_t* lock){

	u_int8_t old = &lock;
	*lock = LOCKED;

	return old;
}

/* aquire the mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
        // use the built-in test-and-set atomic function to test the mutex
        // if the mutex is acquired successfully, enter the critical section
        // if acquiring mutex fails, push current thread into block list and //
        // context switch to the scheduler thread
		

		if(mutex == NULL){
			printf("Mutex has not be initialized\n");

			return -1;
		}

		
		if(__sync_lock_test_and_set(&mutex->lock, 1) == 1){
			/*
				Add the thread that is want the mutex into the mutex queue and then yields it.
			*/	

			// current_thread_block->threadID = 
		
			mypthread_queue_enqueue(&mutex->thread_queue, current_running_thread);
		}
        // YOUR CODE HERE
        return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// so that they could compete for mutex later.
	
	// YOUR CODE HERE
	return 0;
};


/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in mypthread_mutex_init

	queue_cleanup(mutex->thread_queue);
	
	free(mutex);

	return 0;
};

/* scheduler */
static void schedule() {
	// Every time when timer interrup happens, your thread library
	// should be contexted switched from thread context to this
	// schedule function

	// Invoke different actual scheduling algorithms
	// according to policy (STCF or MLFQ)

	// if (sched == STCF)
	//		sched_stcf();
	// else if (sched == MLFQ)
	// 		sched_mlfq();

	// YOUR CODE HERE

// schedule policy
#ifndef MLFQ
	// Choose STCF
#else
	// Choose MLFQ
#endif

}

/* Preemptive SJF (STCF) scheduling algorithm */
static void sched_stcf() {

	mypthread_queue* ready_queue;
	mypthread_queue* priority_queue;
	
	// Your own implementation of STCF
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
	// Your own implementation of MLFQ
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}

// Feel free to add any other functions you need

// YOUR CODE HERE
