#ifndef utilities_h
#define utilities_h

#include <stdbool.h>

struct str_array {
    char** data;
    size_t count;
    size_t buffer_size;
};

void clear_stdin();
bool is_main_process();
void init_str_array(struct str_array* array);

#endif
