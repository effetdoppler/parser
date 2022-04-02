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

// Create parser structure to keep track of the tokens consumed
typedef struct parser_t {
    Token* tokens;
    int ntokens;
    int pos;
    char type;
} parser_t;


typedef struct exprtree {
    char* type;
    double value;
    struct exprtree* left;
    struct exprtree* right;
} exprtree;

Token* tokenize(const char*, int* nb);
exprtree* parse(Token* tokens, int nbtok);
double calculate(exprtree*);
exprtree* parse_number(parser);
exprtree* parse_add_expression(parser);
exprtree* parse_mult_expression(parser);
exprtree* parse_atomic_expression(parser);
static exprtree* create_exprtree(char type, double value, exprtree* left, exprtree* right);
double parse_char(char* input);
#endif // #define PARSER2_H