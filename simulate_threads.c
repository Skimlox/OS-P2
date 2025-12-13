#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int pid;
    int arrival;
    int burst;
    int priority;
} process_t;

