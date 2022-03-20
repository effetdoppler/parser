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
#define NUMBER "0123456789."
#define SEPARATOR "()"

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
    if (ind+1 == strlen(in))
        return 2;
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
        int start = i;
        if (strchr(NUMBER, in[i]))
        {
            // Add to number if it is
            // We'll read the consecutive numbers into a character array, and then convert it to a number with atoi
            while (strchr(NUMBER, in[i]) && i-start < MAX_INPUT_SIZE && i < in_len) 
                i++;
            //settup token
            char *number = malloc((i-start+1)*sizeof(char));
            int ind = 0;
            while(start < i){
                number[ind] = in[start];
                start++;
                ind++;
            }
            number[ind] = '\0';
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
                    errx(EXIT_FAILURE, "Invalid syntax: two operator");
                else
                {
                    if(test_operator(in, i) == 2)
                        errx(EXIT_FAILURE, "Invalid syntax: no expression after an operator");
                }
                tok->type = Operator;
            }
            if (strchr(SEPARATOR, in[i]))
                tok->type = Separator;
            char *op = malloc((2)*sizeof(char));
            op[0] = in[i];
            op[1] = '\0';
            tok->value = op;
            tokens[token_pos++] = *tok;
            i++;
        }
        free(tok);
    }
    // Terminate the tokens string with null

    *nbTokens = token_pos;
    return tokens;
}
