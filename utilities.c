#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utilities.h"

void clear_stdin() {
    while ( getchar() != '\n' );
}

bool is_main_process() {
    return getpgid(getpid()) == getpid();
}

void init_str_array(struct str_array* array) {
    array->data        = malloc(10000 * sizeof(char*));
    array->count       = 0;
    array->buffer_size = 10000;
}
