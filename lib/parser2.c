#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "parser2.h"
#include <math.h>
#include <ctype.h>

#define VALID_TOKENS "+-*/0123456789()"
#define OPERATOR "+-*/^"
#define NUMBER "0123456789.,"
#define SPECIALNUMBER "π"
#define SEPARATOR "()"
#define SPECIALTERM "ecsl"

#define MAX_INPUT_SIZE 100

//static const char* global_error = NULL;
static Result result;

// Create parser structure to keep track of the tokens consumed
typedef struct parser_t {
    Token* tokens;
    int ntokens;
    int pos;
    char type;
} parser_t;


typedef struct exprtree {
    char type;
    double value;
    struct exprtree* left;
    struct exprtree* right;
} exprtree;


// Helper parsing functions

exprtree* parse(Token* tokens, int nbtok);
double calculate(exprtree*);
exprtree* parse_number(parser_t* parser);
exprtree* parse_add_expression(parser_t* parser);
exprtree* parse_mult_expression(parser_t* parser);
exprtree* parse_power_expression(parser_t* parser);
exprtree* parse_atomic_expression(parser_t* parser);


size_t test_operator(const char* in, size_t ind)
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
    size_t in_len = strlen(in);
    size_t i = 0;
    while (i < in_len)
    {
        // Check if input character is a valid token
        Token *tok = malloc(sizeof(Token));
        size_t start = i;
        if (strchr(NUMBER, in[i]))
        {
            size_t wrong_number = 0;
            // Add to number if it is
            // We'll read the consecutive numbers into a character array, and then convert it to a number with atoi
            while (strchr(NUMBER, in[i]) && i-start < MAX_INPUT_SIZE && i < in_len) 
            {
                if(in[i] == '.' || in[i] == ',')
                    wrong_number++;
                i++;
            }
            if (wrong_number > 1)
            {
                 result.err = "Invalid syntax: not a number";                 
                 return tokens;
            }
            //settup token
            char *number = malloc((i-start+1)*sizeof(char));
            size_t ind = 0;
            while(start < i){
                if (in[start] == ',')
                    number[ind] = '.';
                else
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
        else if (strchr(SPECIALNUMBER, in[i]))
        {
            tok->type = Number;
            tok->value = "3.14159265358979323846";
            tokens[token_pos++] = *tok;
            i = i+2;
        }
        else if (isalpha((int)in[i]))
        {
            if (strchr((SPECIALTERM), in[i]))
            {
                i++;
                while (isalpha((int)in[i]) && i < in_len) 
                {
                    i++;
                }
                char* func = malloc(sizeof(char) * (i-start+1));
                for (size_t x = start; x < i; x++)
                    func[x-start] = in[x];
                func[i-start] = '\0';
                if ( strcmp(func, "cos") != 0 &&
                        strcmp(func, "sin") != 0 &&
                        strcmp(func, "exp") != 0 &&
                        strcmp(func, "log") != 0)
                {
                    result.err = "Invalid syntax: invalid function";
                    free(func);
                    return tokens;                        
                }
                free(func);
                if(i == in_len || in[i] != '(')
                {
                    result.err = "Invalid syntax: function must be followed by '('";
                    return tokens;
                }  
                char *op = malloc((2)*sizeof(char));
                op[0] = in[start];
                op[1] = '\0';
                tok->value = op;
                tok->type = Specialterm;
                tokens[token_pos++] = *tok;            
            }
            else
            {
                result.err = "Invalid syntax: invalid function";
                return tokens;
            }
        }
        else
        {
            if (strchr(OPERATOR, in[i]))
            {
                // Add to tokens if it is
                if (test_operator(in, i) == 1)
                {
                    result.err = "Invalid syntax: two consecutive operators";
                    return tokens;
                }
                if(test_operator(in, i) == 2)
                {
                    result.err = "Invalid syntax: no expression after an operator";
                    return tokens;
                }
                tok->type = Operator;
            }
            else if (strchr(SEPARATOR, in[i]))
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


static exprtree* create_exprtree(char type, double value, exprtree* left, exprtree* right) {

    // Allocate memory for the expression
    exprtree* expr = malloc(sizeof(exprtree));

    // Set values for the expression
    expr->type = type;
    expr->value = value;
    expr->left = left;
    expr->right = right;

    return expr;

}


/* Parse tokens into expression tree based on grammar */
exprtree* parse(Token* tokens, int nbtok) {

    // Allocate memory for the parse structure
    parser_t* parser = malloc(sizeof(parser_t));

    // Set initial values for the parser
    parser->tokens = tokens;
    parser->ntokens = nbtok;
    parser->pos = 0;

    // Calculate the expression starting by parsing the tokens starting with the lowest priority
    exprtree* expression = parse_add_expression(parser);

    // Free allocated memory
    free(parser);
    return expression;
}

double calculate(exprtree* expr) {
    if (expr->type == 'n')
        return expr->value;

    double left = 0;
    if(expr->left)
        left = calculate(expr->left);
    double right = calculate(expr->right);
    
    if (expr->type == '+')
        return left + right;
    else if (expr->type == '-')
        return left - right;
    else if (expr->type == '*')
        return left * right;
    else if (expr->type == '/')
        return left / right;
    else if (expr->type == '^')
        return pow(left, right);
    else if (expr->type == 'c')
        return cos(right);
    else if (expr->type == 's')
        return sin(right);
    else if (expr->type == 'e')
        return exp(right);  
    else if (expr->type == 'l')
        return log(right);  

    return 0;
}
exprtree* parse_add_expression(parser_t* parser) {

    /* add_expression := mult_expression (('+' | '-') mult_expression) */

    // An add_expression is composed firstly of a mult_expression, so we parse one right away
    exprtree* expr = parse_mult_expression(parser);
    
    // After the mult_expression, add_expression can find 0 or more (+ or -) followed by another mult_expression
    while (parser->pos < parser->ntokens && (*parser->tokens[parser->pos].value == '+' || *parser->tokens[parser->pos].value == '-')) {

        // Set expression type as either addition or subtraction
        char type = *parser->tokens[parser->pos].value;

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
    exprtree* expr = parse_power_expression(parser);
    
    // mult_expression can find 0 or more (* or /) followed by another atomic_expression
    while (parser->pos < parser->ntokens && (*parser->tokens[parser->pos].value == '*' || *parser->tokens[parser->pos].value == '/')) {

        // set expression type as either multiplication or division
        char type = *parser->tokens[parser->pos].value;

        // consume the multiplication or division token
        parser->pos++;

        // parse an atomic_expression that should come right after (* or /)
        exprtree* right_expr = parse_power_expression(parser);

        // we create a new expression with the //TODO
        expr = create_exprtree(type, 0, expr, right_expr); // value is set to 0 because operations don't use it
    }
    
    return expr;

}
exprtree* parse_power_expression(parser_t* parser) {

    /* add_expression := mult_expression (('+' | '-') mult_expression) */

    // An add_expression is composed firstly of a mult_expression, so we parse one right away
    exprtree* expr = parse_atomic_expression(parser);
    
    // After the mult_expression, add_expression can find 0 or more (+ or -) followed by another mult_expression
    while (parser->pos < parser->ntokens && (*parser->tokens[parser->pos].value == '^')) {

        // Set expression type as either addition or subtraction
        char type = *parser->tokens[parser->pos].value;

        // Consume the addition or subtraction token
        parser->pos++;

        // Parse a mult_expression that should come right after (+ or -)
        exprtree* right_expr = parse_atomic_expression(parser);

        // We create a new expression with the ... //TODO
        expr = create_exprtree(type, 0, expr, right_expr); // value is set to 0 because operations don't use it
    }
    
    return expr;

} 
exprtree* parse_atomic_expression(parser_t* parser) {

    /* atomic_expression := number | left_parenthesis add_expression right_parenthesis */

    exprtree* expr = NULL;
    char specialterm = 0;
    if (parser->pos < parser->ntokens && (*parser->tokens[parser->pos].value == 'c' || *parser->tokens[parser->pos].value == 's' || *parser->tokens[parser->pos].value == 'e' || *parser->tokens[parser->pos].value == 'l'))
    {
        specialterm = *parser->tokens[parser->pos].value;
        parser->pos++;
    }
    // If we find parenthesis, then we read an add_expression as an atomic one
    if (*parser->tokens[parser->pos].value == '(') {

        parser->pos++; // Consume parenthesis

        // Parse add_expression that should come between parenthesis
        expr = parse_add_expression(parser);

        // Consume the closing parenthesis
        if (*parser->tokens[parser->pos].value == ')')
            parser->pos++;
        else {
            
            // Error if there aren't any
            result.err = "Invalid input: mismatched (";
            return expr;
        }
        
    } else {
 
        // This is the alternative production rule - an atomic expression can be just a number
        expr = parse_number(parser);
    }
    if (specialterm)
        expr = create_exprtree(specialterm, 0, NULL, expr);
    
    return expr;

}

exprtree* parse_number(parser_t* parser) {

    /* number := (0-9)+ */

    int negatif = 0;
    if (strchr("-", *parser->tokens[parser->pos].value))
    {
        //number[numberlen] = parser->tokens[parser->pos];
        negatif = 1;
        parser->pos++;
    }
    double value;
    // Convert the number characters array to an int
    if (negatif == 1)
    {
        value = -atof(parser->tokens[parser->pos].value);
    }
    else
    {
        value = atof(parser->tokens[parser->pos].value);
    }

    parser->pos++;
    // Create an expression of type 'n' with the value set as the number value
    exprtree* number_expr = create_exprtree('n', value, NULL, NULL);

    return number_expr;

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

Result calculate_char(const char* input)
{
    // 2. Get tokens from the input string
    int nbtoken;
    result.value = nan("");
    result.err = NULL;
    Token* tokens = tokenize(input, &nbtoken);

    // 3. Create expression tree from tokens
    if (result.err == NULL)
    {
        exprtree* expression = parse(tokens, nbtoken);

        // 4. Calculate the value of the expression
        if (result.err == NULL)
            result.value = calculate(expression);

        // 6. Free the memory allocated for the expression
        free_exprtree(expression);
    }
    free(tokens);

    return result;
}

double parse_char(const char* input)
{
    Result v = calculate_char(input);
    return v.value;
}