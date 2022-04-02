#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <stdarg.h>
#include "parser2.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

const Token END_TOKEN = { .value = "", .type = Operator };

int EqualToken(Token* t1, Token* t2)
{
    return strcmp(t1->value, t2->value) == 0;// && t1->type == t2->type;
}

int EqualTokens(Token* tokens, int nbTokens, Token expected[])
{
    if (nbTokens == 0)
        return 0;
    int i = 0;
    for (; i < nbTokens; i++ )
    {
        if (EqualToken(tokens+i, &END_TOKEN))
            return 0;
        else if (!EqualToken(tokens+i, expected+i))
            return 0;            
    }
    return EqualToken(expected+i, &END_TOKEN);
}

int TestToken(const char* name, Token expected[])
{
    int nb;
    Token* tokens = tokenize(name, &nb);
    if (EqualTokens(tokens, nb, expected))
    {
        printf(ANSI_COLOR_GREEN "pass %s" ANSI_COLOR_RESET "\n", name);
        return 1;
    }
    else
    {
        printf(ANSI_COLOR_RED "fail %s" ANSI_COLOR_RESET "\n", name);
        return 0;
    }
}

//int test_parser()

int TestTokenx(const char* name, ...)
{
    va_list valist;
    va_start(valist, name);
    Token expected[100];
    int count = 0;
    while (1)
    {
        if (count == 99)
        {
            expected[count] = END_TOKEN;
            break;
        }
        const char* token = (const char *)va_arg(valist, const char *);
        if (*token == '\0')
        {
            expected[count] = END_TOKEN;
            break;
        }
        else
        {
            expected[count].value = token;
            expected[count].type = Operator;
            count++;
        }
    }
    return TestToken(name, expected);
}


int Testparser(char* name, double expected)
{
    double res = parse_char(name);
    if (res == expected)
        printf(ANSI_COLOR_GREEN "pass %g" ANSI_COLOR_RESET "\n", res);
    else
        printf(ANSI_COLOR_RED "fail %g" ANSI_COLOR_RESET "\n", res);
}


int main() 
{
    
    Testparser("c(98)", 10);
    TestTokenx("c(32)",  "c", "(", "32", ")", "");
    TestTokenx("c(32+27)",  "c", "(", "32", "+", "27", ")", "");
    TestTokenx("c(32)*58",  "c", "(", "32", ")", "*", "58", "");
    TestTokenx("c(32)*c(32)",  "c", "(", "32", ")", "*",  "c", "(", "32", ")", "");
    TestTokenx("s(21)",  "s", "(", "21", ")", "");
    TestTokenx("e(10)",  "e", "(", "10", ")", "");
    Testparser("c(98)*c(98)", 10);
    TestTokenx("3",  "3", "");
    TestTokenx("33",  "33", "");
    TestTokenx("3+33",  "3", "+", "33",  "");
    TestTokenx("3.1+33",  "3.1", "+", "33",  "");
    TestTokenx("(33+3)", "(", "33", "+", "3", ")", "");
    TestTokenx("(33+3)*8/7-5", "(", "33", "+", "3", ")", "*", "8", "/", "7", "-", "5","");
    TestTokenx("(3.3+3)*8/7-5", "(", "3.3", "+", "3", ")", "*", "8", "/", "7", "-", "5","");
    Testparser("3", 3);
    Testparser("3+7-6", 4);
    Testparser("64*10", 640);
    Testparser("6.5*2", 13);
    Testparser("6.5/2", 3.25);
    
    return 0;
}
