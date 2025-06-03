#ifndef PARSER_PARSER_H_
#define PARSER_PARSER_H_

#include <stdio.h>

#include "../app/app.h"

enum ParserErr {
    PARSER_ERR_OK,
    PARSER_ERR_OUT_OF_MEMORY,
    PARSER_ERR_EOF
};

enum ParserErr Parser_parse_lookup(FILE *fin, struct App *app);
enum ParserErr Parser_parse_input(FILE *fin, struct App *app);

#endif
