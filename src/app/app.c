#include <stdio.h>

#include "app.h"
#include "../parser/parser.h"

enum AppErr App_initialize(struct App *app, char *path) {
    if (!app || !path) {
        return APP_ERR_NULL_PTR;
    }

    if (DArrayChar_initialize(&app->lookup_raw, 1024)) {
        return APP_ERR_OUT_OF_MEMORY;
    }

    if (DArrayIdx_initialize(&app->lookup_idx, 1024)) {
        return APP_ERR_OUT_OF_MEMORY;
    }

    FILE *fin = fopen(path, "r");
    if (!fin) {
        return APP_ERR_INVALID_PATH;
    }

    enum ParserErr err = Parser_parse_lookup(fin, app);
    fclose(fin);

    return err ? APP_ERR_OUT_OF_MEMORY : APP_ERR_OK;
}

void App_finalize(struct App *app) {
    DArrayChar_finalize(&app->lookup_raw);
    DArrayIdx_finalize(&app->lookup_idx);
}

enum AppErr App_log(struct App *app, FILE *fout) {
    if (!app || !fout) {
        return APP_ERR_NULL_PTR;
    }
    for (
        Idx i = 0, *ii = app->lookup_idx.data;
        i < app->lookup_idx.size;
        ++i, ++ii) {
        fprintf(
            fout,
            "%04lu %010lu %s\n",
            i,
            *ii,
            app->lookup_raw.data + *ii
        );
    }
    return APP_ERR_OK;
}
