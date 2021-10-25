/*
        File:                          mypthread.h
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/

#ifndef MYTHREAD_T_H
#define MYTHREAD_T_H

#define _GNU_SOURCE

/* To use Linux pthread Library in Benchmark, you have to comment the USE_MYTHREAD macro */
#define USE_MYTHREAD 1

#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <ucontext.h>

void getID();
// Used as a semantic type for thread IDs.
typedef unsigned int mypthread_t;

enum status {ready = 1, running = 2, blocked = 3, finished=4};

struct threadControlBlock {
        
	ucontext_t* threadContext;      // pointer to thread context
        
        mypthread_t threadID;           // thread id
        enum status state;              // thread status
        int quantumCount;               // number of elapsed quanta
};

struct mypthread_queue {
        struct threadControlBlock *tcb;         // Pointer to the tcb.
        struct mypthread_queue    *next;        // Pointer to the next node in the queue.
};

typedef struct mypthread_mutex_t {
        
	u_int8_t lock;
	u_int8_t flag;
	struct mypthread_queue *thread_queue;
	mypthread_t owner_id;
        
} mypthread_mutex_t;



int mypthread_create(mypthread_t*, pthread_attr_t*, void* (*) (void*), void*);

int mypthread_yield();

void mypthread_exit(void*);

int mypthread_join(mypthread_t, void **);

int mypthread_mutex_init(mypthread_mutex_t*, const pthread_mutexattr_t*);

int mypthread_mutex_lock(mypthread_mutex_t*);

int mypthread_mutex_unlock(mypthread_mutex_t*);

int mypthread_mutex_destroy(mypthread_mutex_t*);


#ifdef USE_MYTHREAD
#define pthread_t mypthread_t
#define pthread_mutex_t mypthread_mutex_t
#define pthread_create mypthread_create
#define pthread_exit mypthread_exit
#define pthread_join mypthread_join
#define pthread_mutex_init mypthread_mutex_init
#define pthread_mutex_lock mypthread_mutex_lock
#define pthread_mutex_unlock mypthread_mutex_unlock
#define pthread_mutex_destroy mypthread_mutex_destroy
#endif

#endif
