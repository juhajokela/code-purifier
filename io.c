#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "main.h"
#include "utilities.h"
#include "purifier.h"
#include "io.h"

int is_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int is_directory(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

char* generate_file_path(const char* path, const char* filename) {
    size_t path_length = strlen(path);
    char*  file_path   = malloc(path_length + strlen(filename) + 2);
    sprintf(file_path, "%s%s%s", path, path[path_length-1] == '/' ? "" : "/", filename);
    return file_path;
}

// filter "files" . and ..
int filter_files(const struct dirent* file) {
    return (strcmp(file->d_name, ".") && strcmp(file->d_name, ".."));
}

int scan_directory(const char* path) {

    // scan files in the directory
    struct dirent **files = NULL;
    int n = scandir(path, &files, &filter_files, NULL);

    char* file_paths[n];
    for (int i = 0; i < n; ++i) {
        file_paths[i] = generate_file_path(path, files[i]->d_name);
    }

    // process files
    size_t child_pid_n = 0;
    pid_t  child_pids[n];

    for (int i = 0; i < n && running; ++i) {

        char* file_path = file_paths[i];

        if (is_file(file_path))
            clean_file(file_path);

        if (is_directory(file_path) && options.recursive) {
            pid_t pid = fork_and_scan_directory(file_path);
            if (pid == 0) break;
            if (pid >= 0) child_pids[child_pid_n++] = pid;
        }

        //free(file_path);
    }

    // wait for all the child processes to exit
    for (size_t i = 0; i < child_pid_n; ++i) {
        waitpid(child_pids[i], NULL, 0);
    }

    // free memory
    while (n--) {
        free(files[n]);
        free(file_paths[n]);
    }
    free(files);

    return n;
}

pid_t fork_and_scan_directory(char* dir_path) {

    pid_t pid = fork();

    if (pid < 0)
        fprintf(stderr, "Forking process failed (status: %i). Directory %s not processed.\n", pid, dir_path);

    if (pid == 0)
        scan_directory(dir_path);

    return pid;
}

void clean_files(struct str_array* args) {

    size_t child_pid_n = 0;
    pid_t  child_pids[args->count];

    for (size_t i = 0; i < args->count; ++i) {

        if (is_file(args->data[i])) {
            clean_file(args->data[i]);
        }

        if (is_directory(args->data[i])) {
            pid_t pid = fork_and_scan_directory(args->data[i]);
            if (pid == 0) break;
            if (pid >= 0) child_pids[child_pid_n++] = pid;
        }
    }

    // wait for all the child processes to exit
    for (size_t i = 0; i < child_pid_n; ++i) {
        waitpid(child_pids[i], NULL, 0);
    }
}
