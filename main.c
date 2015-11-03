#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"
#include "options.h"
#include "io.h"
#include "main.h"

static volatile sig_atomic_t sigint_count = 0;

static void sigint_handler(int sig) {

    if (is_main_process()) {
        ++sigint_count;
        if (sigint_count == 1) {
            puts("Press CTRL-C again to quit.");
        } else {
            running = false;
        }
    }
}

void print_usage(char* program) {
    printf("\nUsage: %s [files] [directories] [options]\n", program);
    puts("\nOptions:\n");
    puts("  --verbose    [-v]");
    puts("  --recursive  [-r]");
    puts("  --extensions [-e] .c,.py,...");
    puts("  --logging=[logfile](optional) / -l (default logfile)");
    putchar('\n');
}

int main(int argc, char *argv[]) {

    // initialize signal handler
    running = true;
    if (signal(SIGINT, sigint_handler) == SIG_ERR)
        perror("Initializing SIGINT signal handler failed.");

    struct str_array args;

    init_options(&options);
    int rc = get_options(&options, &args, argc, argv);

    if (rc < 0) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (options.verbose)
        print_options(&options, &args);

    clean_files(&args);
    free_options(&options, &args);

    return EXIT_SUCCESS;
}
