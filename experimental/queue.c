/*
        File:                          queue.c
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/
#include <stdio.h> 
#include <stdlib.h>
#include "mypthread.h"

#define SUCCESS 0
#define FAILURE 1

/*
struct mypthread_queue {
        struct threadControlBlock *tcb;         // Pointer to the tcb.
        struct mypthread_queue    *next;        // Pointer to the next node in the queue.
};*/

int mypthread_enqueue(struct mypthread_queue **front,
                      struct threadControlBlock* pthread_item)
{
        // Case where there are no items in the queue.
        if (*front == NULL) {
                
                *front = malloc(sizeof(struct mypthread_queue));
                if (*front == NULL) {
                        perror("Malloc : Unable to allocate space for mypthread_queue node ");
                        exit(EXIT_FAILURE);
                }
                
                (*front)->tcb  = pthread_item;
                (*front)->next = NULL;
                
                return SUCCESS;
        }
        
        // Create the mypthread_queue node.
        struct mypthread_queue *temp = malloc(sizeof(struct mypthread_queue));
        if (temp == NULL) {
                perror("Malloc : Unable to allocate space for mypthread_queue node ");
                exit(EXIT_FAILURE);
        }
        temp->tcb  = pthread_item;
        
        struct mypthread_queue* cursor = *front;
        struct mypthread_queue* prev   = NULL;
        
        // Case where the new thread has the highest priority (added to the front of the queue).
        if (pthread_item->quantumCount <= (*front)->tcb->quantumCount) {
                
                temp->next = (*front);
                (*front)   = temp;
                
                return SUCCESS;
        }
        
        // All other cases.
        while (cursor != NULL) {
                if (pthread_item->quantumCount <= cursor->tcb->quantumCount) {
                        prev->next = temp;
                        temp->next = cursor;
                        
                        return SUCCESS;
                }
                
                prev   = cursor;
                cursor = cursor->next;
        }
        
        prev->next = temp;
        
        return EXIT_SUCCESS;
}

int mypthread_dequeue(struct mypthread_queue** front,
                      struct threadControlBlock** pthread_item)
{
        if (front == NULL) {
                return FAILURE;
        }
        
        // temp pointer to the front of the queue
        struct mypthread_queue *temp = *front;
        
        *pthread_item = temp->tcb;
        *front = (*front)->next;
        free(temp);
        
        return SUCCESS;
        
}