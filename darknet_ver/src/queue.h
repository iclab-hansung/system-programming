#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

typedef struct {
    char **items;
    int size;
    int head;
    pthread_mutex_t lock;
} Queue;

void queue_init(Queue *q, char **image_paths, int n_images, int n_jobs);
char *queue_pop(Queue *q);
void queue_destroy(Queue *q);

#endif
