#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "darknet.h"
#include "util.h"
#include "queue.h"

static Config g_config;
static Queue g_queue;
static char **g_class_names;
static char **g_synsets;
static pthread_mutex_t g_load_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_print_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_barrier_t g_load_barrier;

static pthread_mutex_t g_timing_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct timespec g_inference_start_time;
static struct timespec g_last_inference_time;
static int g_completed = 0;
static int g_correct_top1 = 0;
static int g_correct_top5 = 0;
static int g_correct_top10 = 0;

static void *worker(void *arg)
{
    long tid = (long)arg;

    pthread_mutex_lock(&g_load_mutex);
    network *net = load_network(g_config.model_cfg, g_config.model_weight, 0);
    set_batch_network(net, 1);
    pthread_mutex_unlock(&g_load_mutex);

    int barrier_rc = pthread_barrier_wait(&g_load_barrier);
    if (barrier_rc == PTHREAD_BARRIER_SERIAL_THREAD)
        clock_gettime(CLOCK_MONOTONIC, &g_inference_start_time);

    int indexes[10];
    char *path;

    while ((path = queue_pop(&g_queue)) != NULL) {
        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);

        image im = load_image_color(path, 0, 0);
        image r = letterbox_image(im, net->w, net->h);

        float *predictions = network_predict(net, r.data);
        if (net->hierarchy) hierarchy_predictions(predictions, net->outputs, net->hierarchy, 1, 1);
        top_k(predictions, net->outputs, 10, indexes);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

        const char *pred_name = g_class_names[indexes[0]];
        int true_idx = true_class_index(path, g_synsets, net->outputs);
        const char *result = (true_idx < 0) ? "라벨없음" : (true_idx == indexes[0]) ? "정답" : "오답";

        pthread_mutex_lock(&g_print_mutex);
        printf("[thread %ld] %s -> class %d '%s' (%.4f), %s, %.4fs\n",
               tid, path, indexes[0], pred_name, predictions[indexes[0]],
               result, elapsed);
        pthread_mutex_unlock(&g_print_mutex);

        int in_top5 = 0, in_top10 = 0;
        for (int i = 0; i < 10; i++) {
            if (indexes[i] == true_idx) {
                if (i < 5) in_top5 = 1;
                in_top10 = 1;
            }
        }

        pthread_mutex_lock(&g_timing_mutex);
        g_completed++;
        if (true_idx >= 0) {
            if (indexes[0] == true_idx) g_correct_top1++;
            if (in_top5) g_correct_top5++;
            if (in_top10) g_correct_top10++;
        }
        if (g_completed == g_config.num_inferences)
            clock_gettime(CLOCK_MONOTONIC, &g_last_inference_time);
        pthread_mutex_unlock(&g_timing_mutex);

        free_image(im);
        free_image(r);
    }

    free_network(net);
    return NULL;
}

int main(int argc, char **argv)
{
    const char *config_path = (argc > 1) ? argv[1] : "config.yaml";

    gpu_index = -1;

    load_config(config_path, &g_config);
    g_class_names = get_labels(g_config.labels);
    g_synsets = get_labels(g_config.synsets);

    char **image_paths;
    int n_images = list_images(g_config.data_dir, &image_paths);

    queue_init(&g_queue, image_paths, n_images, g_config.num_inferences);
    pthread_barrier_init(&g_load_barrier, NULL, g_config.num_threads);

    pthread_t *threads = malloc(sizeof(pthread_t) * g_config.num_threads);
    for (int i = 0; i < g_config.num_threads; i++)
        pthread_create(&threads[i], NULL, worker, (void *)(long)i);
    for (int i = 0; i < g_config.num_threads; i++)
        pthread_join(threads[i], NULL);
    free(threads);
    pthread_barrier_destroy(&g_load_barrier);

    double total = (g_last_inference_time.tv_sec - g_inference_start_time.tv_sec) +
                   (g_last_inference_time.tv_nsec - g_inference_start_time.tv_nsec) / 1e9;
    printf("추론시작~추론끝: %.4fs\n", total);
    printf("Top-1: %d/%d\n", g_correct_top1, g_config.num_inferences);
    printf("Top-5: %d/%d\n", g_correct_top5, g_config.num_inferences);
    printf("Top-10: %d/%d\n", g_correct_top10, g_config.num_inferences);

    queue_destroy(&g_queue);

    for (int i = 0; i < n_images; i++) free(image_paths[i]);
    free(image_paths);

    return 0;
}
