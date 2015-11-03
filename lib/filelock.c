#include <stdio.h>
#include <unistd.h>
#include "filelock.h"

/*
    A library for easy file locking.

    Version: 0.1
    Author:  Juha Jokela
*/

bool file_lock(FILE* file, struct flock* lock, short lock_type) {

    /*
        Acquire file lock. Function will block until able to acquire the lock.

        Lock types:
            Read  lock: F_RDLCK
            Write lock: F_WRLCK
    */

    lock->l_type   = lock_type;  // F_RDLCK, F_WRLCK, F_UNLCK
    lock->l_whence = SEEK_SET;   // SEEK_SET, SEEK_CUR, SEEK_END
    lock->l_start  = 0;          // Offset from l_whence
    lock->l_len    = 0;          // length, 0 = to EOF
    lock->l_pid    = getpid();   // PID

    int fd = fileno(file);

    if (fcntl(fd, F_SETLKW, lock) == -1) {
        perror("Acquiring lock failed.");
        return false;
    }

    return true;
}

bool file_unlock(FILE* file, struct flock* lock) {

    /*
        Release a lock. Non-blocking function.
    */

    lock->l_type = F_UNLCK;
    int fd = fileno(file);

    if (fcntl(fd, F_SETLK, lock) == -1) {
        perror("Releasing lock for file failed.");
        return false;
    }

    return true;
}

FILE* fopen_with_lock(const char* filename, const char* mode, struct flock* lock) {

    if (!mode || (mode[0] != 'w' && mode[0] != 'r' && mode[0] != 'a')) {
        perror("fopen_with_lock: invalid mode");
        return NULL;
    }

    FILE* file = fopen(filename, mode);
    file_lock(file, lock, mode[0] == 'r' ? F_RDLCK : F_WRLCK);
    return file;
}
