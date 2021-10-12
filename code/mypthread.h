// File:	mypthread_t.h

// List all group member's name:
// username of iLab:
// iLab Server:

#ifndef MYTHREAD_T_H
#define MYTHREAD_T_H

#define _GNU_SOURCE

/* To use Linux pthread Library in Benchmark, you have to comment the USE_MYTHREAD macro */
#define USE_MYTHREAD 1

/* include lib header files that you need here: */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

typedef uint mypthread_t;

typedef struct thread_strack{

} THREAD_STACK;

enum status {ready = 1, running = 2, blocked = 3};

struct threadControlBlock {

	mypthread_t threadID;           // thread id
	ucontext_t* thread_context;      // thread context
        enum status state;              // thread status
        int quantum_count;              // number of elapsed quanta;
};


typedef struct mypthread_queue{

	struct threadControlBlock* context;
	struct mypthread_queue* next;

}mypthread_queue;
/* mutex struct definition */
typedef struct mypthread_mutex_t {
	/* add something here */
	u_int8_t lock;
	u_int8_t flag;
	mypthread_queue* thread_queue;
	mypthread_t owner_id;
	// YOUR CODE HERE
} mypthread_mutex_t;




/* define your data structures here: */
// Feel free to add your own auxiliary data structures (linked list or queue etc...)

// YOUR CODE HERE


/* Function Declarations: */


/*
	Thread queue functions - START
*/
int mypthread_queue_enqueue(mypthread_queue *front, struct threadControlBlock *pthread_item);
struct threadControlBlock* mypthread_queue_dequeue(mypthread_queue *front);
void queue_cleanup(mypthread_queue*);

/*
	Thread queue functions - END
*/

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void
    *(*function)(void*), void * arg);

/* give CPU pocession to other user level threads voluntarily */
int mypthread_yield();

/* terminate a thread */
void mypthread_exit(void *value_ptr);

/* wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr);

/* initial the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex, const pthread_mutexattr_t
    *mutexattr);

/* aquire the mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex);

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex);

/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex);

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
