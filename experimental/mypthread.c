/*
        File:                          mypthread.c
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/
#include "mypthread.h"

#define STACK_SIZE 1892
#define SUCCESS 0
#define FAILURE 1

mypthread_t threadCounter                = 0;     // Assigns thread IDs
ucontext_t  schedulerContext             = {0};   // Zeroes out the schedulerContext struct
struct threadControlBlock *currentThread = NULL;  // No thread is initially running.
struct threadControlBlock *queueFront    = NULL;  // No thread is initially queued.
void *returnValues[1000]                 = {0};   // Zeroes out the array storing the return values of threads

bool schedulerInitialized = false;

static void scheduler()
{
        return;
        
}

static void thread_wrapper(void* (*function) (void *), void *args)
{
        void *returnValue       = (*function) (args);      // Calls the function on behalf of the scheduler
        pthread_t threadID      = currentThread->threadID; // Gets the threadID of the currently running thread.
        returnValues[threadID]  = returnValue;             // Saves the return value to an array.
        
        currentThread->state    = finished;                // Sets the state of the tcb to finished.
        
        return;                                            // Returns to the scheduler context.
        
}

int initialize_scheduler_context()
{
        getcontext(&schedulerContext);
        
        void *stack = malloc(STACK_SIZE);
        if (stack == NULL) {
                perror("Malloc : Could not allocate stack for the scheduler ");
                exit(EXIT_FAILURE);
        }
        
        schedulerContext.uc_link            = 0;          // Set successor context to null
        schedulerContext.uc_stack.ss_sp     = stack;      // Set the starting address of the stack
        schedulerContext.uc_stack.ss_size   = STACK_SIZE; // Set the size of the stack
        schedulerContext.uc_stack.ss_flags  = 0;          // Might be necessary
        
        makecontext(&schedulerContext, (void *) &scheduler, 0);
        
        return SUCCESS;
}

int initialize_scheduler_queues()
{
        
}


int create_new_thread(struct threadControlBlock **tcb, void* (*function) (void*), void *args) 
{
        /* Create the tcb (thread) */
        *tcb = malloc(sizeof(struct threadControlBlock));
        if (*tcb == NULL) {
                perror("Malloc : Could not allocate tcb for the new thread ");
                exit(EXIT_FAILURE);
        }
        
        (*tcb)->threadID      = threadCounter;
        (*tcb)->state         = ready;
        (*tcb)->quantumCount  = 0;
        
        threadCounter++;
        
        
        /* Create the threadWrapperContext
         * The threadWrapperContext calls the passed in function, saves its return value to a global
         * array, sets the status of the thread to finished, and switches to the scheduler context
         * by setting uc_link to schedulerContext. 
         */
        
        ucontext_t *threadWrapperContext = malloc(sizeof(struct ucontext_t));
        if (threadWrapperContext == NULL) {
                perror("Malloc : Could not allocate context for threadWrapper ");
                exit(EXIT_FAILURE);
        }
        
        void *threadWrapperStack = malloc(sizeof(STACK_SIZE));
        if (threadWrapperStack == NULL) {
                perror("Malloc : Could not allocate stack for threadWrapper ");
                exit(EXIT_FAILURE);
        }
        
        getcontext(threadWrapperContext);
        
        threadWrapperContext->uc_link           = &schedulerContext;    // Set the successor context to the scheduler.
        threadWrapperContext->uc_stack.ss_sp    = threadWrapperStack;   // Set the starting address of the stack.
        threadWrapperContext->uc_stack.ss_size  = STACK_SIZE;           // Set the size of the stack.
        threadWrapperContext->uc_stack.ss_flags = 0;                    // Might be necessary.
        
        makecontext(threadWrapperContext, (void *) &thread_wrapper, 2, function, args);
        
        (*tcb)->threadContext = threadWrapperContext;
        
        return SUCCESS;
}

int mypthread_create(mypthread_t *thread, pthread_attr_t *attr,
                     void* (*function) (void*), void *args)
{
        *thread = threadCounter;
        threadCounter++;
        
        if (schedulerInitialized == false) {
                initialize_scheduler_context();
                schedulerInitialized = true;
        }
        
        struct threadControlBlock *newTCB = NULL;
        create_new_thread(&newTCB, function, args);
        
        return 0;
        
}