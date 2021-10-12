// File:	mypthread.c

// List all group member's name: Philip C. Okoh, Paul Kotys
// username of iLab: pco23, pjk151
// iLab Server: ilab1

#include "mypthread.h"

#define STACK_SIZE 1892
#define LOCKED 1
// Used to assign thread IDs to threads
mypthread_t threadIDCounter = 0;
struct threadControlBlock* current_running_thread = NULL;


/*
	Enqueues the pthread into the queue;
	This function was created and used for the mutex locking 
	and unlocking mechanism. Might be repurposed for other cases.

	returns:
		 0: success
		-1: failure
*/
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

        struct threadControlBlock *newTCB = malloc(sizeof(struct threadControlBlock));
        if (newTCB == NULL) {
                /*error handler*/
        }
        
        threadIDCounter++;
        newTCB->threadID      = threadIDCounter;
        newTCB->state        = ready;
        newTCB->quantum_count = 0;
        
        struct ucontext_t *newContext = malloc(sizeof(struct ucontext_t));
        if (newContext == NULL) {
                /*error handler*/
        }
        
        if (getcontext(newContext) != 0) {
                /*error handler*/
        }
        
        void *stack = malloc(STACK_SIZE);
        if (stack == NULL) {
                /*error handler*/
        }
        
        newContext->uc_link           = NULL;             // Set successor stack to null
        newContext->uc_stack.ss_sp    = stack;            // Set the starting address of the stack
        newContext->uc_stack.ss_size  = STACK_SIZE;       // Set the size of the stack
        newContext->uc_stack.ss_flags = 0;                // Might be necessary
        
        // Need to check how arg will be passed to makecontext. Seems to use varargs.
        makecontext(newContext, function, 1, arg);
        
        // newTCB is ready to go.
        newTCB->thread_context = newContext;
        
        
        
        
        return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int mypthread_yield() {

	// change thread state from Running to Ready
	// save context of this thread to its thread control block
	// wwitch from thread context to scheduler context

	// YOUR CODE HERE
	return 0;
};

/* terminate a thread */
void mypthread_exit(void *value_ptr) {
	// Deallocated any dynamic memory created when starting this thread

	// YOUR CODE HERE
};


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {

	// wait for a specific thread to terminate
	// de-allocate any dynamic memory created by the joining thread

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
		if(test_and_set(&mutex->lock) == 1){
			/*
				Add the thread that is want the mutex into the mutex queue and then yields it.
			*/

			mypthread_queue_enqueue(&mutex->thread_queue, current_running_thread);

			// current_thread_block->threadID = 

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
