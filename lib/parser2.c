#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "parser2.h"

// Create expression tree structure to hold our expression
typedef struct exprtree {
    char* type;
    double value;
    struct exprtree* left;
    struct exprtree* right;
} exprtree;

#define VALID_TOKENS "+-*/0123456789()"
#define OPERATOR "+-*/"
#define NUMBER "0123456789"

#define MAX_INPUT_SIZE 100

// Important function declarations
exprtree* parse(char*);
int calculate(exprtree*);

// Helper parsing functions
//static void free_exprtree(exprtree*);
//static exprtree* create_exprtree(char, int, exprtree*, exprtree*);
//exprtree* parse_add_expression(parser_t* parser);
//exprtree* parse_mult_expression(parser_t* parser);
//exprtree* parse_atomic_expression(parser_t* parser);
//exprtree* parse_number(parser_t* parser);



int test_operator(char* in, int ind)
{
    if (strchr(OPERATOR, in[ind+1]))
        return 1;
    return 0;
}

/* Convert input string into tokens */
Token* tokenize(const char* in, int* nbTokens) {

    // Allocate space for the tokens (each token is a character)
    Token* tokens = malloc(sizeof(Token) * MAX_INPUT_SIZE);

    int token_pos = 0; // token_pos will track the amount of tokens in the token string

    // Iterate over the input string and everytime a token is found, add it to the tokens
    int in_len = strlen(in);
    int i = 0;
    while (i < in_len)
    {
        // Check if input character is a valid token
        Token *tok = malloc(sizeof(Token));
        if (strchr(NUMBER, in[i]))
        {
            // Add to number if it is
            char number[MAX_INPUT_SIZE];
            int numberlen = 0;
            // We'll read the consecutive numbers into a character array, and then convert it to a number with atoi
            while (strchr("0123456789", in[i]) && numberlen < MAX_INPUT_SIZE) {

                number[numberlen++] = in[i];
                i++;
            }
            number[numberlen] = '\0';
            //settup token
            tok->value = number;
            tok->type = Number;
            tokens[token_pos++] = *tok;
        }
        // Check if input character is a valid token
        else
        {
            if (strchr(OPERATOR, in[i]))
            {
                // Add to tokens if it is
                if (test_operator(in, i) == 1)
                    err(EXIT_FAILURE, "Invalid syntax: two operator");
                tok->value = &in[i];
                tok->type = Operator;
                tokens[token_pos++] = *tok;
                i++;
            }
        }
    }
    // Terminate the tokens string with null

    *nbTokens = token_pos;
    return tokens;
}
