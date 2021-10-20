/*
        File:                          mypthread.c
        List all group member's name:  Philip C. Okoh, Paul Kotys
        Username of iLab:              pco23, pjk151
        iLab Server:                   ilab1
*/

#include <stdio.h>
#include <stdlib.h>
#include "mypthread.h"

int test() {
        int x = 10;
        int y = x * 2;
        return y;
}

int main(int argc, char** argv) {
        
        
        
        int x = 25;
        
        int threadID = 0;
        
        int ret = mypthread_create(&threadID, NULL, (void*) &test, NULL);
        
}