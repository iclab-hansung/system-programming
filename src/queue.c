#include <stdlib.h>

#include "queue.h"

void queue_init(Queue *q, char **image_paths, int n_images, int n_jobs)
{
    q->items = malloc(sizeof(char *) * n_jobs);
    for (int i = 0; i < n_jobs; i++)
        q->items[i] = image_paths[i % n_images];
    q->size = n_jobs;
    q->head = 0;
    pthread_mutex_init(&q->lock, NULL);
}

char *queue_pop(Queue *q)
{
    char *item = NULL;
    pthread_mutex_lock(&q->lock);
    if (q->head < q->size) item = q->items[q->head++];
    pthread_mutex_unlock(&q->lock);
    return item;
}

void queue_destroy(Queue *q)
{
    free(q->items);
    pthread_mutex_destroy(&q->lock);
}
