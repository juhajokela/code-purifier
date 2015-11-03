#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"
#include "utilities.h"

void parse_extensions(struct options* options, char* optarg) {

    char *token, *ptr;
    struct str_array* extensions = &(options->extensions);

    init_str_array(extensions);

    for (token = strtok_r(optarg, ",", &ptr); token; token = strtok_r(NULL, ",", &ptr)) {

        if (extensions->count == options->extensions.buffer_size) {
            extensions->buffer_size += 10000;
            size_t new_buffer_size = extensions->buffer_size * sizeof(char*);
            extensions->data = (char**) realloc(extensions->data, new_buffer_size);
        }

        extensions->data[extensions->count++] = strdup(token);
    }
}

void init_options(struct options* options) {
    memset(options, 0, sizeof(struct options));
}

int get_options(struct options* options, struct str_array* args, int argc, char **argv) {

    if (argc == 1)
        return -1;

    while (true) {

        static struct option long_options[] = {
            {"verbose",    no_argument,       0, 'v'},
            {"logging",    optional_argument, 0, 'l'},
            {"recursive",  no_argument,       0, 'r'},
            {"extensions", required_argument, 0, 'e'}
        };

        // getopt_long stores the option index here.
        int option_index = 0;

        int c = getopt_long(argc, argv, "vlre:d:", long_options, &option_index);

        // Detect the end of the options.
        if (c == -1)
            break;

        switch (c) {
            case 0:
                // If this option set a flag, do nothing else now.
                break;
            case 'v':
                options->verbose = true;
                break;
            case 'l':
                options->logging = true;
                options->log_file_name = strdup(optarg ? optarg : DEFAULT_LOG);
                break;
            case 'r':
                options->recursive = true;
                break;
            case 'e':
                parse_extensions(options, optarg);
                break;
            case '?':
                exit(EXIT_FAILURE);
            default:
                abort();
                break;
        }
    }

    if (optind < argc) {

        init_str_array(args);

        while (optind < argc) {

            if (args->count == args->buffer_size) {
                args->buffer_size     += 10000;
                size_t new_buffer_size = args->buffer_size * sizeof(char*);
                args->data = (char**) realloc(args->data, new_buffer_size);
            }

            args->data[args->count++] = strdup(argv[optind++]);
        }
    }

    return 0;
}

void print_options(struct options* options, struct str_array* args) {

    putchar('\n');

    printf("Verbose:   %s\n", options->verbose   ? "true" : "false");
    printf("Logging:   %s\n", options->logging   ? "true" : "false");
    printf("Recursive: %s\n", options->recursive ? "true" : "false");

    puts("\nExtensions:\n");

    for (size_t i = 0; i < options->extensions.count; ++i)
        puts(options->extensions.data[i]);

    puts("\nArguments:\n");

    for (size_t i = 0; i < args->count; ++i)
        puts(args->data[i]);

    putchar('\n');
}

void free_options(struct options* options, struct str_array* args) {

    for (size_t i = 0; i < options->extensions.count; ++i)
        free(options->extensions.data[i]);
    for (size_t i = 0; i < args->count; ++i)
        free(args->data[i]);

    if (options->extensions.count > 0)
        free(options->extensions.data);
    if (args->count > 0)
        free(args->data);

    free(options->log_file_name);
}
