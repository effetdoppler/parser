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

typedef struct Result {
    double value;
    const char* err;
} Result;

Token* tokenize(const char*, int* nb);
double parse_char(const char* input);
Result calculate_char(const char* input);
#endif // #define PARSER2_H