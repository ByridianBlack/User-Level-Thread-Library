/*
        File:                          mypthread.h
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

// Used as a semantic type for thread IDs.
typedef unsigned int mypthread_t;

enum status {ready = 1, running = 2, blocked = 3, finished=4};

struct threadControlBlock {
        
	ucontext_t* threadContext;      // pointer to thread context
        
        mypthread_t threadID;           // thread id
        enum status state;              // thread status
        int quantumCount;               // number of elapsed quanta
};


int mypthread_create(mypthread_t*, pthread_attr_t*, void* (*) (void*), void*);