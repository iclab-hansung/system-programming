#ifndef UTIL_H
#define UTIL_H

#define MAX_PATH_LEN 512

typedef struct {
    char model_cfg[MAX_PATH_LEN];
    char model_weight[MAX_PATH_LEN];
    char data_dir[MAX_PATH_LEN];
    int num_threads;
    int num_inferences;
} Config;

void load_config(const char *path, Config *cfg);
int list_images(const char *dir, char ***out_paths);

#endif
