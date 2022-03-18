#ifndef PARSER2_H
#define PARSER2_H

typedef enum TokenType
{
    Number,
    Operator,
    Parenthese
} TokenType;

typedef struct Token {
    const char* value;
    TokenType type;
} Token;

// Create parser structure to keep track of the tokens consumed
typedef struct parser_t {
    Token* tokens;
    int ntokens;
    int pos;
    char type;
} parser_t;


Token* tokenize(const char*, int* nb);

#endif // #define PARSER2_H