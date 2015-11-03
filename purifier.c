#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "main.h"
#include "utilities.h"
#include "lib/filelock.h"

void write_log(char* file_path, double diff_t) {

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    struct flock lock;
    FILE* log = fopen_with_lock(options.log_file_name, "a", &lock);

    fprintf(log, "%d-%d-%d %d:%d:%d %s cleaned in %.1f seconds.\n",
        tm.tm_mday, tm.tm_mon + 1, (tm.tm_year + 1900) % 100,
        tm.tm_hour, tm.tm_min, tm.tm_sec, file_path, diff_t
    );

    file_unlock(log, &lock);
    fclose(log);
}

bool is_valid_file(char* file_path) {
    /*
        Check whether file has defined extension.
        If any extension aren't defined, all files are valid.
    */
    if (options.extensions.count == 0)
        return true;

    for (int i = 0; i < options.extensions.count; ++i) {
        if (strstr(file_path, options.extensions.data[i]) != NULL) {
            return true;
        }
    }

    return false;
}

void trim_trailing_whitespace(char* destination, char* source) {

    char* end = source + strlen(source) - 1;
    while (end > source && isspace(*end)) end--;
    *(end+1) = 0;
    sprintf(destination, "%s%c", source, '\n');
}

void _clean_file(FILE* destination, FILE* source) {
 
    char*  line   = NULL;
    size_t length = 0, newlines = 0;

    while (getline(&line, &length, source) != -1) {
 
        char tmp[length+1];
        trim_trailing_whitespace(tmp, line);
 
        if (tmp[0] == '\n') {
            ++newlines;
            continue;
        }

        while (newlines > 0) {
            fputc('\n', destination);
            --newlines;
        }

        fputs(tmp, destination);
    }

    free(line);
}

void clean_file(char* file_path) {

    if (!is_valid_file(file_path))
        return;

    // initialize timer
    time_t start_t = time(NULL), end_t;

    if (options.verbose)
        printf("%i: %s\n", getpid(), file_path);

    // open source file
    struct flock source_lock;
    FILE* source = fopen_with_lock(file_path, "r", &source_lock);

    // open temporary destination file
    char temp_file[strlen(file_path)+5];
    sprintf(temp_file, "%s%s", file_path, ".tmp");

    struct flock destination_lock;
    FILE* destination = fopen_with_lock(temp_file, "w", &destination_lock);

    _clean_file(destination, source);

    // unlock and close files
    file_unlock(destination, &destination_lock);
    fclose(destination);

    file_unlock(source, &source_lock);
    fclose(source);

    // replace original file with the cleaned temp file
    remove(file_path);
    rename(temp_file, file_path);

    if (options.logging) {
        time(&end_t);
        double diff_t = difftime(end_t, start_t);
        write_log(file_path, diff_t);
    }

}
