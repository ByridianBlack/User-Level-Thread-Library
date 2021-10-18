#include <stdio.h>
#include <stdlib.h>
#include "mypthread.h"


int mypthread_prior_queue_enqueue(mypthread_queue **front, struct threadControlBlock* pthread_item){

	if(*front == NULL){
		*front = malloc(sizeof(mypthread_queue));
		(*front)->context = pthread_item;
		(*front)->next = NULL;
		return 0;
	}

    mypthread_queue* cursor = *front;
    mypthread_queue* prev = NULL;

    if(pthread_item->quantum_count <= (*front)->context->quantum_count){
        mypthread_queue *temp = malloc(sizeof(mypthread_queue));
        temp->context = (*front)->context;
        temp->next = (*front)->next;
        (*front)->next = temp;
        (*front)->context = pthread_item;
        return 0;
    }

    while(cursor != NULL){
        if(pthread_item->quantum_count <= cursor->context->quantum_count){
            mypthread_queue *temp = malloc(sizeof(mypthread_queue));
            temp->context = pthread_item;
            prev->next = temp;
            temp->next = cursor;
            return 0;
        }   
        prev = cursor;
        cursor = cursor->next;
    }

	return -1;
}

// mypthread_queue *cursor = *front;
	// if((*front)->context->quantum_count < cursor->priority){
	// 	mypthread_queue *temp = malloc(sizeof(mypthread_queue));
	// 	temp->priority = priority;
	// 	temp->next = cursor;
	// 	(*front) = temp;
	// 	return 0;
	// }
	
	// while(cursor != NULL){
	// 	if(priority < cursor->priority){
	// 		mypthread_queue *temp = malloc(sizeof(mypthread_queue));
	// 		temp->priority = priority;
	// 		temp->next = cursor;
	// 		(*front) = temp;
	// 		return 0;
	// 	}
	// 	cursor = cursor->next;
	// }

void print_queue(mypthread_queue* queue){
    if(queue == NULL){
        return;
    }

    mypthread_queue* cursor = queue;

    while(cursor != NULL){
        printf("%d\n", cursor->context->quantum_count);
        cursor = cursor->next;
    }
}
int main(){
    mypthread_queue* front = NULL;
    struct threadControlBlock* item_1 = malloc(sizeof(struct threadControlBlock));
    item_1->quantum_count = 3;
    struct threadControlBlock* item_2 = malloc(sizeof(struct threadControlBlock));
    item_2->quantum_count = 2;
    struct threadControlBlock* item_3 = malloc(sizeof(struct threadControlBlock));
    item_3->quantum_count = 4;

    struct threadControlBlock* item_4 = malloc(sizeof(struct threadControlBlock));
    item_4->quantum_count = 0;
    struct threadControlBlock* item_5 = malloc(sizeof(struct threadControlBlock));
    item_4->quantum_count = 0;
    mypthread_prior_queue_enqueue(&front, item_1);
    mypthread_prior_queue_enqueue(&front, item_2);
    mypthread_prior_queue_enqueue(&front, item_3);

    mypthread_prior_queue_enqueue(&front, item_4);
    mypthread_prior_queue_enqueue(&front, item_5);
    print_queue(front);
    return 0;
}