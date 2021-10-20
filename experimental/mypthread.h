/*
        File:                          mypthread.c
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

// Used as a semantic type for thread IDs.
typedef unsigned int mypthread_t;

enum status {ready = 1, running = 2, blocked = 3, finished=4};

struct threadControlBlock {

	mypthread_t threadID;           // thread id
	ucontext_t* thread_context;     // thread context
        enum status state;              // thread status
        int quantum_count;              // number of elapsed quanta
        bool restored;                  // indicates that a thread has been restored by the scheduler
};


int mypthread_create(mypthread_t*, pthread_attr_t*, void* (*) (void*), void*);