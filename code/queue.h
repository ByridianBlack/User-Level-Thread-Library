/*
        File:                          queue.h
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/

int mypthread_prior_queue_enqueue(mypthread_queue**, struct threadControlBlock*, int);

int mypthread_queue_enqueue(mypthread_queue**, struct threadControlBlock*);

struct threadControlBlock* mypthread_queue_dequeue(mypthread_queue **);

void queue_cleanup(mypthread_queue*);