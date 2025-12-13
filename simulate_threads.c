#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_PROCESSES 100

typedef struct {
    int pid;
    int arrival;
    int burst;
    int priority;
} process_t;
void* process_run(void* arg) {
    process_t* p=(process_t*)arg;
    printf("Process %d started (burst=%d)\n", p->pid, p->burst);
    sleep(p->burst);
    printf("Process %d finished\n",p->pid);
    return NULL;
}
int main(void) {
    FILE* fp=fopen("processes.txt","r");
    char line[256];
    fgets(line,sizeof(line),fp);
    process_t data[MAX_PROCESSES];
    int x = 0;
    //read data
    while (fscanf(fp,"%d %d %d %d", 
                  &data[x].pid, 
                  &data[x].arrival, 
                  &data[x].burst, 
                  &data[x].priority) == 4) {
        x++;
    }
    fclose(fp);
    printf("%d process threads created...\n", x);
    pthread_t threads[MAX_PROCESSES];
    for (int i = 0; i < x; i++) {
        if (pthread_create(&threads[i], NULL,process_run,&data[i]) != 0) {
            return 1;
        }
    }
    //finish
    for (int i = 0; i<x; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("\nAll %d processes completed.\n",x);
    return 0;
}