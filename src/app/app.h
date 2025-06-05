#ifndef APP_APP_H_
#define APP_APP_H_

#include "../util/containers.h"

struct App {
    DArrayChar dictionary_raw;
    DArrayIdx dictionary_idx;
    DArrayChar input_raw;
    DArrayIdx input_idx;
    DArrayChar lookup;
    DArrayIdx output;
};

enum AppErr {
    APP_ERR_OK,
    APP_ERR_NULL_PTR,
    APP_ERR_OUT_OF_MEMORY,
    APP_ERR_INVALID_PATH,
    APP_ERR_EOF,
    APP_ERR_NOT_FOUND
};

enum AppErr App_initialize(struct App *app, char *path);
void App_finalize(struct App *app);
enum AppErr App_log_dictionary(struct App *app, FILE *fout);
enum AppErr App_parse_input(struct App *app, FILE *fin);
enum AppErr App_log_input(struct App *app, FILE *fout);
enum AppErr App_solve(struct App *app, bool verbose);
enum AppErr App_log_output(struct App *app, FILE *fout);

#endif
