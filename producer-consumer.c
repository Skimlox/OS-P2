#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 20
#define TOTAL_ITEMS 20
#define N_CONSUMERS 3

//shared buffer
int buffer[BUFFER_SIZE];
int in = 0, out = 0;
pthread_mutex_t mutex;
sem_t empty, full;
typedef struct {
    int id;
    int count;
} cons_args;
void* producer_thread(void* arg) {
    printf("Start Producer Thread\n");
    for (int i = 0; i<TOTAL_ITEMS; i++) {
        usleep(500000); //simulate production time
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer[in] = i;
        printf("Producer: added item %d at position %d\n", i, in);
        in = (in + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    printf("Producer thread finished\n");
    return NULL;
}
void* consumer_thread(void* arg) {
    cons_args* ca = (cons_args*)arg;
    printf("Consumer %d started (processing %d items)\n", ca->id, ca->count);
    for (int i = 0; i < ca->count; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int item = buffer[out];
        printf("Consumer %d: took item %d from position %d\n", ca->id, item, out);
        out = (out + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        //thread process
        usleep(1000000);
    }
    printf("Consumer %d finished\n", ca->id);
    return NULL;
}
int main() {
    pthread_t producer, consumer[N_CONSUMERS];
    cons_args cargs[N_CONSUMERS];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    //producer start
    pthread_create(&producer, NULL, producer_thread, NULL);
    //consumer work distribution
    int per_consumer = TOTAL_ITEMS / N_CONSUMERS;
    int remainder = TOTAL_ITEMS % N_CONSUMERS;
    for (int i = 0; i < N_CONSUMERS; i++) {
        cargs[i].id = i;
        cargs[i].count = per_consumer + (i == N_CONSUMERS - 1 ? remainder : 0);
        pthread_create(&consumer[i], NULL, consumer_thread, &cargs[i]);
    }
    //completion time
    pthread_join(producer, NULL);
    for (int i = 0; i < N_CONSUMERS; i++) {
        pthread_join(consumer[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    printf("\nAll threads completed\n");
    return 0;
}