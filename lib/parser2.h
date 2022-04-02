#ifndef PARSER2_H
#define PARSER2_H

typedef enum TokenType
{
    Number,
    Operator,
    Separator,
    Specialterm
} TokenType;

typedef struct Token {
    const char* value;
    TokenType type;
} Token;

Token* tokenize(const char*, int* nb);
double parse_char(const char* input);
#endif // #define PARSER2_H