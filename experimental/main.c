/*
        File:                          mypthread.c
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/

#include <stdio.h>
#include <stdlib.h>
#include "mypthread.h"

mypthread_mutex_t locker;
int counter = 0;

void *test(void *other) {
        int x = 10;
        int y = x * 2;
        // printf("first:%d\n", locker.lock);
        mypthread_mutex_lock(&locker);
        counter++;
        // printf("%d\n", locker.lock);
        getID();
        mypthread_mutex_unlock(&locker);
        // printf("y: %d\n", y);

        return NULL;
}

int main(int argc, char** argv) {
        
        int x = 25;
        
        unsigned threadID = 0;
        
        mypthread_mutex_init(&locker, NULL);
        
        for(int i = 0; i < 1000; i++){
            
            int ret = mypthread_create(&threadID, NULL, test, NULL);

        }
        
        printf("x: %d\n", counter);
        
}
