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

mypthread_t threadCounter    = 0;               // Assigns thread IDs
ucontext_t  schedulerContext = {0};             // Zeroes out the schedulerContext struct

bool schedulerInitialized = false;

static void scheduler()
{
        return;
        
}

int initializeSchedulerContext()
{
        getcontext(&schedulerContext);
        
        void *stack = malloc(STACK_SIZE);
        if (stack == NULL) {
                perror("Malloc : Could not allocate stack for the scheduler");
                exit(EXIT_FAILURE);
        }
        
        schedulerContext.uc_link            = 0;          // Set successor stack to null
        schedulerContext.uc_stack.ss_sp     = stack;      // Set the starting address of the stack
        schedulerContext.uc_stack.ss_size   = STACK_SIZE; // Set the size of the stack
        schedulerContext.uc_stack.ss_flags  = 0;          // Might be necessary
        
        makecontext(&schedulerContext, (void *) &scheduler, 0);
        
        return SUCCESS;
}

int mypthread_create(mypthread_t *thread, pthread_attr_t *attr,
                     void* (*function) (void*), void *arg)
{
        *thread = threadCounter;
        threadCounter++;
        
        if (schedulerInitialized == false) {
                initializeSchedulerContext();
                schedulerInitialized = true;
        }
        
        return 0;
        
}