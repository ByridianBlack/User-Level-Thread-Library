#include "queue.h"
#include "mypthread.h"
/*
        File:                          queue.c
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/

/*
	Enqueues the pthread into the queue;
	This function was created and used for the mutex locking 
	and unlocking mechanism. Might be repurposed for other cases.

	returns:
		 0: success
		-1: failure
*/

int mypthread_prior_queue_enqueue(mypthread_queue **front, struct threadControlBlock* pthread_item, int priority){

	if(*front == NULL){
		
	}

}
int mypthread_queue_enqueue(mypthread_queue** front, struct threadControlBlock* pthread_item)
{
	if(*front == NULL){
	
		*front = malloc(sizeof(mypthread_queue));
	
		(*front)->next = NULL;
	
		(*front)->context = pthread_item;
	
		return 0;
	}

	mypthread_queue* cursor = front;

	while(cursor->next != NULL){
	
		cursor = cursor->next;
	
	}

	cursor->next = malloc(sizeof(mypthread_queue));
	
	cursor->next->next = NULL;
	
	cursor->next->context = pthread_item;
	
	return 0;
}

struct threadControlBlock* mypthread_queue_dequeue(mypthread_queue **front){
	
	if(*front == NULL){
	
		return NULL;
	
	}
	
	struct threadControlBlock* ret_val = (*front)->context;
	
	*front = (*front)->next;
	return ret_val;
}


/*
	Cleans up the queue if any items remains
*/
void queue_cleanup(mypthread_queue* front)
{
	if(front == NULL)
	{
		return;
	}

	mypthread_queue* cursor = front;
	
	mypthread_queue* temp = NULL;
	
	while(cursor != NULL)
	{
	
		temp = cursor;
	
		cursor = cursor->next;
	
		free(temp->context);
	
		free(temp);
	}
	
	return;
}