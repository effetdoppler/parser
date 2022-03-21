#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "parser2.h"

#define VALID_TOKENS "+-*/0123456789()"
#define OPERATOR "+-*/"
#define NUMBER "0123456789."
#define SEPARATOR "()"

#define MAX_INPUT_SIZE 100

// Important function declarations


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

/* Parse tokens into expression tree based on grammar */
exprtree* parse(Token* tokens) {

    // Allocate memory for the parse structure
    parser_t* parser = malloc(sizeof(parser_t));

    // Set initial values for the parser
    parser->tokens = tokens;
    parser->ntokens = strlen(tokens);
    parser->pos = 0;

    // Calculate the expression starting by parsing the tokens starting with the lowest priority
    //exprtree* expression = parse_add_expression(parser);

    // Free allocated memory
    free(parser->tokens);
    free(parser);
    exprtree* expression = NULL;
    return expression;
}

int calculate(exprtree* expr) {

    if (expr->type == 'n')
        return expr->value;

    int left = calculate(expr->left);
    int right = calculate(expr->right);
    
    if (expr->type == '+')
        return left + right;
    else if (expr->type == '-')
        return left - right;
    else if (expr->type == '*')
        return left * right;
    else if (expr->type == '/')
        return left / right ? right : 0;

    return 0;
}

exprtree* parse_add_expression(parser_t* parser) {

    /* add_expression := mult_expression (('+' | '-') mult_expression) */

    // An add_expression is composed firstly of a mult_expression, so we parse one right away
    exprtree* expr = parse_mult_expression(parser);
    
    // After the mult_expression, add_expression can find 0 or more (+ or -) followed by another mult_expression
    while (parser->pos < parser->ntokens && (parser->tokens[parser->pos].value == '+' || parser->tokens[parser->pos].value == '-')) {

        // Set expression type as either addition or subtraction
        char type = parser->tokens[parser->pos].value;

        // Consume the addition or subtraction token
        parser->pos++;

        // Parse a mult_expression that should come right after (+ or -)
        exprtree* right_expr = parse_mult_expression(parser);

        // We create a new expression with the ... //TODO
        expr = create_exprtree(type, 0, expr, right_expr); // value is set to 0 because operations don't use it
    }
    
    return expr;

} 

exprtree* parse_mult_expression(parser_t* parser) {


    /* mult_expression := atomic_expression (('*' | '/') atomic_expression) */

    // a mult_expression is composed firstly of an atomic_expression, so we parse one right away
    exprtree* expr = parse_atomic_expression(parser);
    
    // mult_expression can find 0 or more (* or /) followed by another atomic_expression
    while (parser->pos < parser->ntokens && (parser->tokens[parser->pos].value == '*' || parser->tokens[parser->pos].value == '/')) {

        // set expression type as either multiplication or division
        char type = parser->tokens[parser->pos].value;

        // consume the multiplication or division token
        parser->pos++;

        // parse an atomic_expression that should come right after (* or /)
        exprtree* right_expr = parse_atomic_expression(parser);

        // we create a new expression with the //TODO
        expr = create_exprtree(type, 0, expr, right_expr); // value is set to 0 because operations don't use it
    }
    
    return expr;

}

exprtree* parse_atomic_expression(parser_t* parser) {

    /* atomic_expression := number | left_parenthesis add_expression right_parenthesis */

    exprtree* expr;

    // If we find parenthesis, then we read an add_expression as an atomic one
    if (parser->tokens[parser->pos].value == '(') {

        parser->pos++; // Consume parenthesis

        // Parse add_expression that should come between parenthesis
        expr = parse_add_expression(parser);

        // Consume the closing parenthesis
        if (parser->tokens[parser->pos].value == ')')
            parser->pos++;
        else {
            
            // Error if there aren't any
            fprintf(stderr, "Invalid input\n");
            exit(1);
        }
        
    } else {
 
        // This is the alternative production rule - an atomic expression can be just a number
        expr = parse_number(parser);
    }
    
    return expr;

}

exprtree* parse_number(parser_t* parser) {

    /* number := (0-9)+ */

    int negatif = 0;
    if (strchr("-", parser->tokens[parser->pos].value))
    {
        //number[numberlen] = parser->tokens[parser->pos];
        negatif = 1;
        parser->pos++;
    }

    int value;
    // Convert the number characters array to an int
    if (negatif == 1)
    {
        value = -atoi(parser->tokens[parser->pos].value);
    }
    else
    {
        value = atoi(parser->tokens[parser->pos].value);
    }
    
    // Create an expression of type 'n' with the value set as the number value
    exprtree* number_expr = create_exprtree('n', value, NULL, NULL);

    return number_expr;

}

static exprtree* create_exprtree(char type, int value, exprtree* left, exprtree* right) {

    // Allocate memory for the expression
    exprtree* expr = malloc(sizeof(exprtree));

    // Set values for the expression
    expr->type = type;
    expr->value = value;
    expr->left = left;
    expr->right = right;

    return expr;

}

static void free_exprtree(exprtree* expr) {

    // Free the expression recursively

    if (expr) {

        if (expr->left)
            free_exprtree(expr->left);
        if (expr->right)
            free_exprtree(expr->right);

        free(expr);

    }
}
int parse_char(char* input)
{
        // 2. Get tokens from the input string
        int nbtoken;
        Token* tokens = tokenize(input, &nbtoken);

        // 3. Create expression tree from tokens
        exprtree* expression = parse(tokens);

        // 4. Calculate the value of the expression
        int value = calculate(expression);


        // 6. Free the memory allocated for the expression
        free_exprtree(expression);

        return value;
}

int main(int argc, char* argv[]) 
{
    if (argc == 2)
    {
        int res = parse_char(argv[1]);
        printf("%d\n", res);
    }
    return 0;
}