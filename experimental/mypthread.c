/*
        File:                          mypthread.c
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/
#include "mypthread.h"

#define STACK_SIZE 1048576
#define TIME_INTERVAL 20
#define SUCCESS 0
#define FAILURE 1

mypthread_t threadCounter                = 0;     // Assigns thread IDs
ucontext_t*  schedulerContext            = NULL;  // Zeroes out the schedulerContext struct
struct threadControlBlock *currentThread = NULL;  // No thread is initially running.
struct mypthread_queue *readyQueue       = NULL;  // Queue to store the ready threads.
struct mypthread_queue *blockedQueue     = NULL;  // Queue to store the blocked threads 
void *returnValues[1000]                 = {0};   // Zeroes out the array storing the return values of threads

bool schedulerInitialized = false;
bool ignoreTimer          = false;

int mypthread_enqueue(struct mypthread_queue **front,
                      struct threadControlBlock* pthread_item)
{
        ignoreTimer = true;
        
        // Case where there are no items in the queue.
        if (*front == NULL) {
                
                *front = malloc(sizeof(struct mypthread_queue));
                if (*front == NULL) {
                        perror("Malloc : Unable to allocate space for mypthread_queue node ");
                        exit(EXIT_FAILURE);
                }
                
                (*front)->tcb  = pthread_item;
                (*front)->next = NULL;
                
                return SUCCESS;
        }
        
        // Create the mypthread_queue node.
        struct mypthread_queue *temp = malloc(sizeof(struct mypthread_queue));
        if (temp == NULL) {
                perror("Malloc : Unable to allocate space for mypthread_queue node ");
                exit(EXIT_FAILURE);
        }
        temp->tcb  = pthread_item;
        
        struct mypthread_queue* cursor = *front;
        struct mypthread_queue* prev   = NULL;
        
        // Case where the new thread has the highest priority (added to the front of the queue).
        if (pthread_item->quantumCount <= (*front)->tcb->quantumCount) {
                
                temp->next = (*front);
                (*front)   = temp;
                
                return SUCCESS;
        }
        
        // All other cases.
        while (cursor != NULL) {
                if (pthread_item->quantumCount <= cursor->tcb->quantumCount) {
                        prev->next = temp;
                        temp->next = cursor;
                        
                        return SUCCESS;
                }
                
                prev   = cursor;
                cursor = cursor->next;
        }
        
        prev->next = temp;
        
        ignoreTimer = false;
        
        return EXIT_SUCCESS;
}

int mypthread_enqueue_front(struct mypthread_queue **front,
                            struct threadControlBlock *pthread_item)
{
        ignoreTimer = true;
        
        struct mypthread_queue *entry = malloc(sizeof(struct mypthread_queue));
        if (entry == NULL) {
                perror("malloc : unable to allocate space for the queue node ");
                exit(EXIT_FAILURE);
        }
        
        entry->tcb  = pthread_item;
        entry->next = (*front);
        
        (*front) = entry;
        
        ignoreTimer = false;
        
        return SUCCESS;
        
}
int mypthread_dequeue(struct mypthread_queue** front,
                      struct threadControlBlock** pthread_item)
{
        ignoreTimer = true;
        
        if (front == NULL) {
                return FAILURE;
        }
        
        // temp pointer to the front of the queue
        struct mypthread_queue *temp = *front;
        
        *pthread_item = temp->tcb;
        *front = (*front)->next;
        free(temp);
        
        ignoreTimer = false;
        
        return SUCCESS;
        
}

void start_timer()
{
        struct itimerval timer;
        
        // Set the period between now and the first timer interrupt
        timer.it_value.tv_sec   = 0;                            // Timer does not last for over a second
        timer.it_value.tv_usec  = TIME_INTERVAL * 1000;         // Convert milliseconds to microseconds
        
        // Set the period between each timer interrupt
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = TIME_INTERVAL * 1000;
        
        int ret = setitimer(ITIMER_REAL, &timer, NULL);
        if (ret != 0) {
                perror("setitimer : unable to set the timer ");
                exit(EXIT_FAILURE);
        }
}

static void scheduler()
{       
        ignoreTimer = true;
        
        //start_timer();
        
        enum status currentState = currentThread->state;
        int ret;
        
        assert(currentState != ready);
        
        if (currentState == running) {
                currentThread->state         = ready;   // currentThread goes from running to ready.
                currentThread->quantumCount += 1;       // currentThread finished running so quantum is incremented.
                
                // Running thread isn't finished. We add it back to the ready queue.
                ret = mypthread_enqueue(&readyQueue, currentThread);
                
                if (ret != SUCCESS) {
                        fprintf(stderr, "mypthread_enqueue : Failed to add current thread to queue ");
                        exit(EXIT_FAILURE);
                }
                
        } else if (currentState == finished) {
                free(currentThread->threadContext->uc_stack.ss_sp);    // free the stack of the finished thread.
                free(currentThread->threadContext);                    // free the context of the finished thread.
                free(currentThread);                                   // free the tcb of the finished thread.
        }
        
        // The following always occurs (even is state of current thread is blocked/finished)
        ret = mypthread_dequeue(&readyQueue, &currentThread);
        if (ret != SUCCESS) {
                fprintf(stderr, "mypthread_dequeue : Failed to find a new thread to run ");
                exit(EXIT_FAILURE);
        }
        
        currentThread->state = running;                 // Set the state of the current thread to running.
        
        ignoreTimer = false;
        
        ret = setcontext(currentThread->threadContext); // Get out of the scheduler.
        if (ret != 0) {
                perror("setcontext : Unable to switch to current thread ");
                exit(EXIT_FAILURE);
        }
        
}

void SIGALRM_handler(int signum)
{
        
        if (ignoreTimer == true) return;
        
        // Save the current (running) context and switch to the scheduler.
        int ret = swapcontext(currentThread->threadContext, schedulerContext);
        if (ret != 0) {
                perror("swapcontext : Failed to swap the current context with the scheduler context ");
                exit(EXIT_FAILURE);
        }
        
        return;
}

static void thread_wrapper(void* (*function) (void *), void *args)
{
        void *returnValue       = function(args);          // Calls the function on behalf of the scheduler
        pthread_t threadID      = currentThread->threadID; // Gets the threadID of the currently running thread.
        returnValues[threadID]  = returnValue;             // Saves the return value to an array.
        
        currentThread->state    = finished;                // Sets the state of the tcb to finished.
        
        return;                                            // Should return to the scheduler context.
}

static int initialize_scheduler_context()
{
        ignoreTimer = true;
        
        schedulerContext = malloc(sizeof(struct ucontext_t));
        if (schedulerContext == NULL) {
                perror("Malloc : Unable to allocate space for scheduler context ");
                exit(EXIT_FAILURE);
        }
        
        getcontext(schedulerContext);
        
        void *stack = malloc(STACK_SIZE);
        if (stack == NULL) {
                perror("malloc : Could not allocate stack for the scheduler ");
                exit(EXIT_FAILURE);
        }
        
        schedulerContext->uc_link            = 0;          // Set successor context to null
        schedulerContext->uc_stack.ss_sp     = stack;      // Set the starting address of the stack
        schedulerContext->uc_stack.ss_size   = STACK_SIZE; // Set the size of the stack
        schedulerContext->uc_stack.ss_flags  = 0;          // Might be necessary
        
        makecontext(schedulerContext, (void *) &scheduler, 0);
        
        ignoreTimer = false;
        
        return SUCCESS;
}

static int initialize_signal_handler()
{
        ignoreTimer = true;
        
        struct sigaction act = {0};               // Zeroes out a sigaction struct
        act.sa_handler = &SIGALRM_handler;        // SIGALRM_handler is the signal handler function
        
        int ret = sigaction(SIGALRM, &act, NULL); // Register the signal handler.
        
        if (ret != 0) {
                perror("sigaction : Failed to register the signal handler ");
                exit(EXIT_FAILURE);
        }
        
        ignoreTimer = false;
        
        return SUCCESS;
}

static int initialize_main_tcb(struct threadControlBlock **mainTCB)
{
        ignoreTimer = true;
        
        *mainTCB = malloc(sizeof(struct threadControlBlock));
        if (*mainTCB == NULL) {
                perror("malloc : Could not allocate tcb for the main thread ");
                exit(EXIT_FAILURE);
        }
        
        (*mainTCB)->threadID        = threadCounter;            // Main thread should have a threadID 0
        (*mainTCB)->state           = running;                  // Main thread is currently running thread
        (*mainTCB)->quantumCount    = 0;                        // QuantumCount initially zero
        
        threadCounter++;
        
        ucontext_t *mainContext     = malloc(sizeof(struct ucontext_t));
        if (mainContext == NULL) {
                perror("malloc : Could not allocate context for main ");
                exit(EXIT_FAILURE);
        }
        
        (*mainTCB)->threadContext   = mainContext;
        
        ignoreTimer = false;
        return SUCCESS;
}

static int create_new_thread(struct threadControlBlock **tcb, void* (*function) (void*), void *args) 
{
        ignoreTimer = true;
        
        /* Create the newTCB (thread) */
        *tcb = malloc(sizeof(struct threadControlBlock));
        if ((*tcb) == NULL) {
                perror("malloc : Could not allocate tcb for the new thread ");
                exit(EXIT_FAILURE);
        }
        
        (*tcb)->threadID      = threadCounter;
        (*tcb)->state         = ready;
        (*tcb)->quantumCount  = 0;
        
        threadCounter++;
        
        
        /* Create the threadWrapperContext
         * The threadWrapperContext calls the passed in function, saves its return value to a global
         * array, sets the status of the thread to finished, and switches to the scheduler context
         * by invoking the signal handler.
         */
        
        ucontext_t *threadWrapperContext = malloc(sizeof(struct ucontext_t));
        if (threadWrapperContext == NULL) {
                perror("malloc : Could not allocate context for threadWrapper ");
                exit(EXIT_FAILURE);
        }
        
        void *threadWrapperStack = malloc(STACK_SIZE);
        if (threadWrapperStack == NULL) {
                perror("malloc : Could not allocate stack for threadWrapper ");
                exit(EXIT_FAILURE);
        }
        
        getcontext(threadWrapperContext);
        
        threadWrapperContext->uc_link           = schedulerContext;     // Set the successor context to the scheduler context
        threadWrapperContext->uc_stack.ss_sp    = threadWrapperStack;   // Set the starting address of the stack.
        threadWrapperContext->uc_stack.ss_size  = STACK_SIZE;           // Set the size of the stack.
        threadWrapperContext->uc_stack.ss_flags = 0;                    // Might be necessary.
        
        makecontext(threadWrapperContext, (void *) thread_wrapper, 2, function, args);
        (*tcb)->threadContext = threadWrapperContext;
        
        ignoreTimer = false;
        return SUCCESS;
}

int mypthread_create(mypthread_t *thread, pthread_attr_t *attr,
                     void* (*function) (void*), void *args)
{
        
        if (schedulerInitialized == false) {
                initialize_scheduler_context();
                initialize_signal_handler();
                schedulerInitialized = true;
        }
        
        
        // currentThread is only null when main calls mypthread_create for the first time. 
        if (currentThread == NULL) {
                // Note that we do not enqueue the mainTCB since it is already the currently
                // running thread.
                struct threadControlBlock *mainTCB = NULL;
                initialize_main_tcb(&mainTCB);
                currentThread = mainTCB;
        }
        
        
        struct threadControlBlock *newTCB = NULL;
        create_new_thread(&newTCB, function, args);
        *thread = newTCB->threadID;
        
        
        int ret = 0;
        ret = mypthread_enqueue(&readyQueue, newTCB);
        if (ret == FAILURE) {
                fprintf(stderr, "mypthread_enqueue : Failed to enqueue new thread\n");
                exit(EXIT_FAILURE);
        }
        
        ret = swapcontext(currentThread->threadContext, schedulerContext);
        if (ret != 0) {
                perror("swapcontext : Failed to swap the current context with the scheduler context ");
                exit(EXIT_FAILURE);
        }
        
        return 0;
        
}

int mypthread_yield()
{
        int ret = swapcontext(currentThread->threadContext, schedulerContext);
        if (ret != 0) {
                perror("swapcontext : Failed to swap the current context with the scheduler context ");
                exit(EXIT_FAILURE);
        }
        
        return SUCCESS;
}

void mypthread_exit(void *returnValue)
{
        pthread_t threadID      = currentThread->threadID; // Gets the threadID of the currently running thread.
        returnValues[threadID]  = returnValue;             // Saves the return value to an array.
        
        currentThread->state    = finished;                // Sets the state of the tcb to finished.
        
        // Gets the attention of the scheduler to cleanup the thread.
        int ret = swapcontext(currentThread->threadContext, schedulerContext);
        if (ret != 0) {
                perror("swapcontext : Failed to swap the current context with the scheduler context ");
                exit(EXIT_FAILURE);
        }
}

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
	mutex->thread_queue = NULL;
	mutex->owner_id = 0;
	// YOUR CODE HERE
	return 0;
};

/* aquire the mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
        // use the built-in test-and-set atomic function to test the mutex
        // if the mutex is acquired successfully, enter the critical section
        // if acquiring mutex fails, push current thread into block list and //
        // context switch to the scheduler thread
		

		if(mutex == NULL){
			printf("Mutex has not been initialized\n");

			return -1;
		}

		
		if(__sync_lock_test_and_set(&mutex->lock, 1) == 1){
			/*
				Add the thread that is want the mutex into the mutex queue and then yield it.
			*/	
			currentThread->state = blocked;								// MIGHT CHANGE LATER
                        mypthread_enqueue_front(&(mutex->thread_queue), currentThread);
			mypthread_yield();
                        
		
		}
        // YOUR CODE HERE
        return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// so that they could compete for mutex later.

		if(mutex == NULL){
			printf("Mutex has not been initialized\n");
			return -1;
		}

		if(__sync_lock_test_and_set(&mutex->lock, 0) == 0){
			
			/*
				Need global queue of running queue;
			*/
			struct threadControlBlock* cursor = NULL;
                        mypthread_dequeue(&(mutex->thread_queue), &cursor);
                        
			while(mutex->thread_queue != NULL){
				// add queue item to running queues; 

				cursor->state = running;
				
                                mypthread_dequeue(&(mutex->thread_queue), &cursor);
                                mypthread_enqueue(&readyQueue, cursor);
			}
			
		}

	// YOUR CODE HERE
	return 0;
};

void queue_cleanup(struct mypthread_queue* front)
{
	if(front == NULL)
	{
		return;
	}

	struct mypthread_queue* cursor = front;
	
	struct mypthread_queue* temp = NULL;
	
	while(cursor != NULL)
	{
	
		temp = cursor;
	
		cursor = cursor->next;
	
		free(temp->tcb);
	
		free(temp);
	}
	
	return;
}

/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in mypthread_mutex_init

	queue_cleanup(mutex->thread_queue);
	
	free(mutex);

	return 0;
};
