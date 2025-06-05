#include <stdio.h>
#include <unistd.h>

#include "app/app.h"

int main(void) {
    struct App app;
    enum AppErr err = APP_ERR_OK;
    String dictionary_path;
    int ret = DArrayChar_initialize(&dictionary_path, 1000);
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
    // if ((err = App_log_dictionary(&app, stdout))) {
    //     printf("%d\n", err);
    //     App_finalize(&app);
    //     DArrayChar_finalize(&dictionary_path);
    //     return 2;
    // }
    puts("Ready");
    for (;;) {
        printf("%s", "> ");
        fflush(stdout);
        err = App_parse_input(&app, stdin);
        if (err == APP_ERR_EOF) {
            break;
        } else if (err) {
            printf("Parse %d\n", err);
            App_finalize(&app);
            DArrayChar_finalize(&dictionary_path);
            return 1;
        }
        err = App_log_input(&app, stdout);
        if (err) {
            printf("Parse Log %d\n", err);
            App_finalize(&app);
            DArrayChar_finalize(&dictionary_path);
            return 1;
        }
        err = App_solve(&app);
        if (err && err != APP_ERR_NOT_FOUND) {
            printf("Solve %d\n", err);
            App_finalize(&app);
            DArrayChar_finalize(&dictionary_path);
            return 1;
        } else if (err == APP_ERR_NOT_FOUND) {
            puts("Not found");
            continue;
        }
        err = App_log_output(&app, stdout);
        if (err) {
            printf("Output Log %d\n", err);
            App_finalize(&app);
            DArrayChar_finalize(&dictionary_path);
            return 1;
        }
    }
    App_finalize(&app);
    DArrayChar_finalize(&dictionary_path);
    return 0;
}
