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
        
        *tcb->threadID          = threadIDCounter;
        *tcb->state             = ready;
        *tcb->quantum_count     = 0;
        *tcb->restored          = 0;
        
        threadIDCounter++;
        
        struct ucontext_t *context = malloc(sizeof(struct ucontext_t));
        if (curContext == NULL) {
                perror("malloc: ");
                return 1;
        }
        
        *tcb->thread_context = context;
        
        return 0;
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
        
        // Need to check how arg will be passed to makecontext. Seems to use varargs.
        makecontext(newContext, function, 1, arg);
        
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

	current_running_thread->state = ready;
	// makecontext(current_running_thread->thread_context);
	
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
