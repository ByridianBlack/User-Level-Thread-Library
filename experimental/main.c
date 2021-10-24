/*
        File:                          mypthread.c
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/

#include <stdio.h>
#include <stdlib.h>
#include "mypthread.h"

void *test(void *other) {
        int x = 10;
        int y = x * 2;
        //printf("y: %d\n", y);
        return NULL;
}

int main(int argc, char** argv) {
        
        int x = 25;
        
        unsigned threadID = 0;
        
        int ret = mypthread_create(&threadID, NULL, test, NULL);
        if (ret != 0) {
                fprintf(stderr, "mypthread_create : unable to create thread ");
                exit(EXIT_FAILURE);
        }
        
        printf("x: %d\n", x);
        
}