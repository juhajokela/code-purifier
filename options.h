#ifndef options_h
#define options_h

#include <stdbool.h>

#include "utilities.h"

#define DEFAULT_LOG "code-purifier.log"

struct options {

    bool verbose;
    bool logging;
    bool recursive;
    struct str_array extensions;
    char* log_file_name;
};

void init_options(struct options* options);
int  get_options(struct options* options, struct str_array* args, int argc, char **argv);
void print_options(struct options* options, struct str_array* args);
void free_options(struct options* options, struct str_array* args);

#endif
