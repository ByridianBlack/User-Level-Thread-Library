/*
        File:                          mypthread.c
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/
#include "mypthread.h"
#include "queue.h"

#define STACK_SIZE 1892
#define SUCCESS 0
#define FAILURE 1

mypthread_t threadCounter                = 0;     // Assigns thread IDs
ucontext_t  schedulerContext             = {0};   // Zeroes out the schedulerContext struct
struct threadControlBlock *currentThread = NULL;  // No thread is initially running.
struct mypthread_queue *readyQueue       = NULL;  // Initially, there is nothing in the queue.
void *returnValues[1000]                 = {0};   // Zeroes out the array storing the return values of threads

bool schedulerInitialized = false;

static void scheduler()
{       
        enum status currentState = currentThread->state;
        int ret;
        
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
        
        ret = setcontext(currentThread->threadContext); // Get out of the scheduler.
        if (ret != 0) {
                perror("setcontext : Unable to switch to current thread ");
                exit(EXIT_FAILURE);
        }
        
        return;
}

void SIGALRM_handler(int signum)
{
        
        // Save the current (running) context and switch to the scheduler.
        int ret = swapcontext(currentThread->threadContext, &schedulerContext);
        if (ret != 0) {
                perror("swapcontext : Failed to swap the current context with the scheduler context ");
                exit(EXIT_FAILURE);
        }
        
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

static int initialize_scheduler_context()
{
        getcontext(&schedulerContext);
        
        void *stack = malloc(STACK_SIZE);
        if (stack == NULL) {
                perror("malloc : Could not allocate stack for the scheduler ");
                exit(EXIT_FAILURE);
        }
        
        schedulerContext.uc_link            = 0;          // Set successor context to null
        schedulerContext.uc_stack.ss_sp     = stack;      // Set the starting address of the stack
        schedulerContext.uc_stack.ss_size   = STACK_SIZE; // Set the size of the stack
        schedulerContext.uc_stack.ss_flags  = 0;          // Might be necessary
        
        makecontext(&schedulerContext, (void *) &scheduler, 0);
        
        return SUCCESS;
}

static int initialize_signal_handler()
{
        struct sigaction act = {0};               // Zeroes out a sigaction struct
        act.sa_handler = &SIGALRM_handler;        // SIGALRM_handler is the signal handler function
        
        int ret = sigaction(SIGALRM, &act, NULL); // Register the signal handler.
        
        if (ret != 0) {
                perror("sigaction : Failed to register the signal handler ");
                exit(EXIT_FAILURE);
        }
        
        return SUCCESS;
}

static int initialize_main_tcb(struct threadControlBlock **mainTCB)
{
        *mainTCB = malloc(sizeof(struct threadControlBlock));
        if (*mainTCB == NULL) {
                perror("malloc : Could not allocate tcb for the main thread ");
                exit(EXIT_FAILURE);
        }
        
        (*mainTCB)->threadID        = threadCounter;
        (*mainTCB)->state           = running;
        (*mainTCB)->quantumCount    = 0;
        
        threadCounter++;
        
        ucontext_t *mainContext     = malloc(sizeof(struct ucontext_t));
        if (mainContext == NULL) {
                perror("malloc : Could not allocate context for main ");
                exit(EXIT_FAILURE);
        }
        
        (*mainTCB)->threadContext   = mainContext;
        
        getcontext(mainContext);
        
        return SUCCESS;
}

static int create_new_thread(struct threadControlBlock **tcb, void* (*function) (void*), void *args) 
{
        /* Create the tcb (thread) */
        *tcb = malloc(sizeof(struct threadControlBlock));
        if (*tcb == NULL) {
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
         * by setting uc_link to schedulerContext. 
         */
        
        ucontext_t *threadWrapperContext = malloc(sizeof(struct ucontext_t));
        if (threadWrapperContext == NULL) {
                perror("malloc : Could not allocate context for threadWrapper ");
                exit(EXIT_FAILURE);
        }
        
        void *threadWrapperStack = malloc(sizeof(STACK_SIZE));
        if (threadWrapperStack == NULL) {
                perror("malloc : Could not allocate stack for threadWrapper ");
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
        int ret;
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
        
        ret = mypthread_enqueue(&readyQueue, newTCB);
        if (ret == FAILURE) {
                fprintf(stderr, "mypthread_enqueue : Failed to enqueue new thread\n");
                exit(EXIT_FAILURE);
        }
        
        raise(SIGALRM);
        
        return 0;
        
}