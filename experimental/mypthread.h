/*
        File:                          mypthread.c
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/

// Used as a semantic type for thread IDs.
typedef unsigned int mypthread_t;

int mypthread_create(mypthread_t*, pthread_attr_t*, void* (*) (void*), void*);