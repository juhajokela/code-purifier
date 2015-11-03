#ifndef io_h
#define io_h

#include "utilities.h"

void  clean_files(struct str_array* args);
int   scan_directory(const char* path);
pid_t fork_and_scan_directory(char* dir_path);

#endif
