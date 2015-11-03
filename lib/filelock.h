#ifndef filelock_h
#define filelock_h

#include <fcntl.h>
#include <stdbool.h>

/*
    A library for easy file locking.

    Version: 0.1
    Author:  Juha Jokela
*/

bool file_lock(FILE* file, struct flock* lock, short lock_type);
bool file_unlock(FILE* file, struct flock* lock);
FILE* fopen_with_lock(const char* filename, const char* mode, struct flock* lock);

#endif
