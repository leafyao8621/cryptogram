#include <stdio.h>
#include <unistd.h>

#include "app/app.h"

static const char *msg =
    "Usage: cryptogram [OPTIONS]\n"
    "Options:\n"
    "-s\n"
    "-v\n"
    "-d\n";

int main(int argc, char **argv) {
    struct App app;
    enum AppErr err = APP_ERR_OK;
    int ret = 0;
    bool verbose = false;
    bool silent = false;
    bool dictionary = false;
    for (; (ret = getopt(argc, argv, "vsd")) != -1;) {
        switch (ret) {
        case 'v':
            verbose = true;
            break;
        case 's':
            silent = true;
            break;
        case 'd':
            dictionary = true;
            break;
        case ':':
            perror(msg);
            return -1;
        case '?':
            perror(msg);
            return -1;
        }
    }
    String dictionary_path;
    ret = DArrayChar_initialize(&dictionary_path, 1000);
    if (ret) {
        puts("Out of Memory");
        return 1;
    }
    char *home = getenv("HOME");
    ret = DArrayChar_push_back_batch(&dictionary_path, home, strlen(home));
    if (ret) {
        puts("Out of Memory");
        return 1;
    }
    char *path = "/.cryptogram/words.txt";
    ret = DArrayChar_push_back_batch(&dictionary_path, path, strlen(path) + 1);
    if (ret) {
        puts("Out of Memory");
        return 1;
    }
    if ((err = App_initialize(&app, dictionary_path.data))) {
        printf("Initialize %d\n", err);
        App_finalize(&app);
        DArrayChar_finalize(&dictionary_path);
        return 1;
    }
    if (dictionary) {
        if ((err = App_log_dictionary(&app, stdout))) {
            printf("%d\n", err);
            App_finalize(&app);
            DArrayChar_finalize(&dictionary_path);
            return 2;
        }
    }
    if (!silent) {
        puts("Ready");
    }
    for (;;) {
        if (!silent) {
            printf("%s", "> ");
            fflush(stdout);
        }
        err = App_parse_input(&app, stdin);
        if (err == APP_ERR_EOF) {
            break;
        } else if (err) {
            fprintf(stderr, "Parse %d\n", err);
            App_finalize(&app);
            DArrayChar_finalize(&dictionary_path);
            return 1;
        }
        if (verbose) {
            err = App_log_input(&app, stdout);
            if (err) {
                fprintf(stderr, "Parse Log %d\n", err);
                App_finalize(&app);
                DArrayChar_finalize(&dictionary_path);
                return 1;
            }
        }
        err = App_solve(&app, verbose);
        if (err && err != APP_ERR_NOT_FOUND) {
            fprintf(stderr, "Solve %d\n", err);
            App_finalize(&app);
            DArrayChar_finalize(&dictionary_path);
            return 1;
        } else if (err == APP_ERR_NOT_FOUND) {
            puts("Not found");
            continue;
        }
        err = App_log_output(&app, stdout);
        if (err) {
            fprintf(stderr, "Output Log %d\n", err);
            App_finalize(&app);
            DArrayChar_finalize(&dictionary_path);
            return 1;
        }
    }
    App_finalize(&app);
    DArrayChar_finalize(&dictionary_path);
    return 0;
}
