#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#include "util.h"

#define MAX_LINE_LEN 512

static void trim(char *s)
{
    char *start = s;
    while (isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) s[--len] = '\0';

    len = strlen(s);
    if (len >= 2 && (s[0] == '"' || s[0] == '\'') && s[len - 1] == s[0]) {
        memmove(s, s + 1, len - 2);
        s[len - 2] = '\0';
    }
}

void load_config(const char *path, Config *cfg)
{
    memset(cfg, 0, sizeof(*cfg));

    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "config open fail: %s\n", path);
        exit(1);
    }

    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), fp)) {
        char *hash = strchr(line, '#');
        if (hash) *hash = '\0';

        char *colon = strchr(line, ':');
        if (!colon) continue;

        *colon = '\0';
        char *key = line;
        char *value = colon + 1;
        trim(key);
        trim(value);
        if (strlen(key) == 0 || strlen(value) == 0) continue;

        if (strcmp(key, "num_inferences") == 0) cfg->num_inferences = atoi(value);
        else if (strcmp(key, "num_threads") == 0) cfg->num_threads = atoi(value);
        else if (strcmp(key, "model_cfg") == 0) snprintf(cfg->model_cfg, MAX_PATH_LEN, "%s", value);
        else if (strcmp(key, "model_weight") == 0) snprintf(cfg->model_weight, MAX_PATH_LEN, "%s", value);
        else if (strcmp(key, "data_dir") == 0) snprintf(cfg->data_dir, MAX_PATH_LEN, "%s", value);
        else if (strcmp(key, "labels") == 0) snprintf(cfg->labels, MAX_PATH_LEN, "%s", value);
        else if (strcmp(key, "synsets") == 0) snprintf(cfg->synsets, MAX_PATH_LEN, "%s", value);
    }
    fclose(fp);

    if (cfg->num_inferences <= 0 || cfg->num_threads <= 0 ||
        !cfg->model_cfg[0] || !cfg->model_weight[0] || !cfg->data_dir[0] ||
        !cfg->labels[0] || !cfg->synsets[0]) {
        fprintf(stderr, "config invalid/incomplete: %s\n", path);
        exit(1);
    }
}

static int has_image_ext(const char *name)
{
    const char *dot = strrchr(name, '.');
    if (!dot) return 0;

    char ext[16];
    size_t i;
    for (i = 0; i < sizeof(ext) - 1 && dot[1 + i]; i++)
        ext[i] = (char)tolower((unsigned char)dot[1 + i]);
    ext[i] = '\0';

    return !strcmp(ext, "jpg") || !strcmp(ext, "jpeg") ||
           !strcmp(ext, "png") || !strcmp(ext, "bmp");
}

int list_images(const char *dir, char ***out_paths)
{
    DIR *d = opendir(dir);
    if (!d) {
        fprintf(stderr, "data_dir open fail: %s\n", dir);
        exit(1);
    }

    int cap = 16, n = 0;
    char **paths = malloc(sizeof(char *) * cap);

    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (!has_image_ext(ent->d_name)) continue;
        if (n >= cap) {
            cap *= 2;
            paths = realloc(paths, sizeof(char *) * cap);
        }
        char full[MAX_PATH_LEN];
        snprintf(full, MAX_PATH_LEN, "%s/%s", dir, ent->d_name);
        paths[n++] = strdup(full);
    }
    closedir(d);

    if (n == 0) {
        fprintf(stderr, "no images found in %s\n", dir);
        exit(1);
    }

    *out_paths = paths;
    return n;
}

static void basename_noext(const char *path, char *out, size_t outsize)
{
    const char *slash = strrchr(path, '/');
    const char *base = slash ? slash + 1 : path;
    const char *dot = strrchr(base, '.');
    size_t len = dot ? (size_t)(dot - base) : strlen(base);
    if (len >= outsize) len = outsize - 1;
    memcpy(out, base, len);
    out[len] = '\0';
}

int true_class_index(const char *image_path, char **synsets, int n_synsets)
{
    char base[MAX_PATH_LEN];
    basename_noext(image_path, base, sizeof(base));

    char *underscore = strchr(base, '_');
    if (underscore) *underscore = '\0';

    for (int i = 0; i < n_synsets; i++)
        if (strcmp(base, synsets[i]) == 0) return i;

    return -1;
}
