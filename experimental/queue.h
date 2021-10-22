/*
        File:                          queue.h
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/


struct mypthread_queue {
        struct threadControlBlock *tcb;         // Pointer to the tcb.
        struct mypthread_queue    *next;        // Pointer to the next node in the queue.
};

// Adds the passed in threadControlBlock to the queue.
int mypthread_enqueue(struct mypthread_queue**, struct threadControlBlock*);

// Writes the highest priority threadControlBlock to passed in argument.
int mypthread_dequeue(struct mypthread_queue**, struct threadControlBlock**);