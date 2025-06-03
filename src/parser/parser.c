#include "parser.h"

enum ParserErr Parser_parse_lookup(FILE *fin, struct App *app) {
    size_t idx = 0;
    if (DArrayIdx_push_back(&app->dictionary_idx, &idx)) {
        return PARSER_ERR_OUT_OF_MEMORY;
    }
    for (char c = fgetc(fin); !feof(fin); c = fgetc(fin)) {
        if (c == '\n') {
            c = 0;
            if (DArrayChar_push_back(&app->dictionary_raw, &c)) {
                return PARSER_ERR_OUT_OF_MEMORY;
            }
            ++idx;
            if (DArrayIdx_push_back(&app->dictionary_idx, &idx)) {
                return PARSER_ERR_OUT_OF_MEMORY;
            }
        } else if (c == '\r') {
            continue;
        } else {
            if (c >= 'a' && c <= 'z') {
                c ^= 0x20;
            }
            if (DArrayChar_push_back(&app->dictionary_raw, &c)) {
                return PARSER_ERR_OUT_OF_MEMORY;
            }
            ++idx;
        }
    }
    char c = 0;
    if (DArrayChar_push_back(&app->dictionary_raw, &c)) {
        return PARSER_ERR_OUT_OF_MEMORY;
    }
    return PARSER_ERR_OK;
}

enum ParserErr Parser_parse_input(FILE *fin, struct App *app) {
    DArrayChar_clear(&app->input_raw);
    DArrayIdx_clear(&app->input_idx);
    size_t idx = 0;
    if (DArrayIdx_push_back(&app->input_idx, &idx)) {
        return PARSER_ERR_OUT_OF_MEMORY;
    }
    for (char c = fgetc(fin); c != '\n'; c = fgetc(fin)) {
        if (feof(fin)) {
            return PARSER_ERR_EOF;
        }
        if (c == ' ') {
            c = 0;
            if (DArrayChar_push_back(&app->input_raw, &c)) {
                return PARSER_ERR_OUT_OF_MEMORY;
            }
            ++idx;
            if (DArrayIdx_push_back(&app->input_idx, &idx)) {
                return PARSER_ERR_OUT_OF_MEMORY;
            }
        } else {
            if (DArrayChar_push_back(&app->input_raw, &c)) {
                return PARSER_ERR_OUT_OF_MEMORY;
            }
            ++idx;
        }
    }
    char c = 0;
    if (DArrayChar_push_back(&app->input_raw, &c)) {
        return PARSER_ERR_OUT_OF_MEMORY;
    }
    return PARSER_ERR_OK;
}
