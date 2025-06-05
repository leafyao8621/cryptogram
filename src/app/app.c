#include <stdio.h>

#include "app.h"
#include "../parser/parser.h"

enum AppErr App_initialize(struct App *app, char *path) {
    if (!app || !path) {
        return APP_ERR_NULL_PTR;
    }

    if (DArrayChar_initialize(&app->dictionary_raw, 1024)) {
        return APP_ERR_OUT_OF_MEMORY;
    }

    if (DArrayIdx_initialize(&app->dictionary_idx, 1024)) {
        return APP_ERR_OUT_OF_MEMORY;
    }

    if (DArrayChar_initialize(&app->input_raw, 1024)) {
        return APP_ERR_OUT_OF_MEMORY;
    }

    if (DArrayIdx_initialize(&app->input_idx, 1024)) {
        return APP_ERR_OUT_OF_MEMORY;
    }

    if (DArrayChar_initialize(&app->lookup, 26000)) {
        return APP_ERR_OUT_OF_MEMORY;
    }

    if (DArrayIdx_initialize(&app->output, 1024)) {
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
    DArrayChar_finalize(&app->dictionary_raw);
    DArrayIdx_finalize(&app->dictionary_idx);
    DArrayChar_finalize(&app->input_raw);
    DArrayIdx_finalize(&app->input_idx);
    DArrayChar_finalize(&app->lookup);
    DArrayIdx_finalize(&app->output);
}

enum AppErr App_log_dictionary(struct App *app, FILE *fout) {
    if (!app || !fout) {
        return APP_ERR_NULL_PTR;
    }
    for (
        Idx i = 0, *ii = app->dictionary_idx.data;
        i < app->dictionary_idx.size;
        ++i, ++ii) {
        fprintf(
            fout,
            "%010lu %010lu %s\n",
            i,
            *ii,
            app->dictionary_raw.data + *ii
        );
    }
    return APP_ERR_OK;
}

enum AppErr App_parse_input(struct App *app, FILE *fin) {
    enum ParserErr err = Parser_parse_input(fin, app);
    switch (err) {
    case PARSER_ERR_EOF:
        return APP_ERR_EOF;
    case PARSER_ERR_OUT_OF_MEMORY:
        return APP_ERR_OUT_OF_MEMORY;
    default:
        return APP_ERR_OK;
    }
}

enum AppErr App_log_input(struct App *app, FILE *fout) {
    if (!app || !fout) {
        return APP_ERR_NULL_PTR;
    }
    fputs("Input:\n", fout);
    for (
        Idx i = 0, *ii = app->input_idx.data;
        i < app->input_idx.size;
        ++i, ++ii) {
        fprintf(
            fout,
            "%010lu %010lu %s\n",
            i,
            *ii,
            app->input_raw.data + *ii
        );
    }
    return APP_ERR_OK;
}

static int find(struct App *app, char advance) {
    char lookup[26];
    for (
        ++app->output.data[app->output.size - 1];
        app->output.data[app->output.size - 1] <
        app->dictionary_idx.size;
        ++app->output.data[app->output.size - 1]) {
        memcpy(
            lookup,
            app->lookup.data + app->lookup.size - (advance ? 26 : 52),
            26
        );
        char *iter_input =
            app->input_raw.data +
            app->input_idx.data[app->output.size - 1];
        char *iter_dictionary =
            app->dictionary_raw.data +
            app->dictionary_idx.data[app->output.data[app->output.size - 1]];
        for (
            ;
            *iter_input && *iter_dictionary;
            ++iter_input, ++iter_dictionary) {
            if (*iter_input >= 'a' && *iter_input <= 'z') {
                if (*iter_dictionary < 'A' || *iter_dictionary > 'Z') {
                    break;
                }
                if (lookup[*iter_input - 'a']) {
                    if (lookup[*iter_input - 'a'] != *iter_dictionary) {
                        break;
                    }
                } else {
                    char *iter_lookup = lookup;
                    for (
                        ;
                        iter_lookup < lookup + 26 &&
                        *iter_lookup != *iter_dictionary;
                        ++iter_lookup);
                    if (iter_lookup != lookup + 26) {
                        break;
                    }
                    lookup[*iter_input - 'a'] = *iter_dictionary;
                }
            } else {
                if (*iter_input != *iter_dictionary) {
                    break;
                }
            }
        }
        if (!*iter_input && !*iter_dictionary) {
            break;
        }
    }
    if (advance) {
        if (DArrayChar_push_back_batch(&app->lookup, lookup, 26)) {
            return 1;
        }
    } else {
        memcpy(app->lookup.data + app->lookup.size - 26, lookup, 26);
    }
    return 0;
}



enum AppErr App_solve(struct App *app) {
    DArrayIdx_clear(&app->output);
    DArrayChar_clear(&app->lookup);
    char lookup[26] = {0};
    if (DArrayChar_push_back_batch(&app->lookup, lookup, 26)) {
        return APP_ERR_OUT_OF_MEMORY;
    }
    size_t negative_one = -1;
    char advance = 1;
    for (; app->output.size < app->input_idx.size;) {
        if (advance) {
            if (DArrayIdx_push_back(&app->output, &negative_one)) {
                return APP_ERR_OUT_OF_MEMORY;
            }
        }
        int ret = find(app, advance);
        if (ret) {
            return APP_ERR_OUT_OF_MEMORY;
        }
        if (
            app->output.data[app->output.size - 1] ==
            app->dictionary_idx.size) {
            DArrayIdx_pop_back(&app->output);
            DArrayChar_pop_back_batch(&app->lookup, 26);
            advance = 0;
        } else {
            advance = 1;
        }
        if (!app->output.size) {
            return APP_ERR_NOT_FOUND;
        }
        App_log_output(app, stdout);
    }
    return APP_ERR_OK;
}

enum AppErr App_log_output(struct App *app, FILE *fout) {
    if (!app || !fout) {
        return APP_ERR_NULL_PTR;
    }
    fputs("Output:\n", fout);
    for (
        Idx i = 0, *ii = app->output.data;
        i < app->output.size;
        ++i, ++ii) {
        fprintf(
            fout,
            "%010lu %010lu %010lu %s\n",
            i,
            *ii,
            app->dictionary_idx.data[*ii],
            app->dictionary_raw.data + app->dictionary_idx.data[*ii]
        );
    }
    return APP_ERR_OK;
}
