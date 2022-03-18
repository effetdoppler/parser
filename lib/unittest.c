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


int main() 
{
    TestTokenx("3",  "3", "");
    TestTokenx("33",  "33", "");
    TestTokenx("3+3",  "3", "+", "3", "");

    return 0;
}