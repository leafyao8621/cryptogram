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
    APP_ERR_INVALID_PATH
};

enum AppErr App_initialize(struct App *app, char *path);
void App_finalize(struct App *app);
enum AppErr App_log(struct App *app, FILE *fout);

#endif
