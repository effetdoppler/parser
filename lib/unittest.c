#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <stdarg.h>
#include <math.h>
#include "parser2.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

const Token END_TOKEN = { .value = "", .type = Operator };

int EqualToken(const Token* t1, const Token* t2)
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
        free(tokens);
        return 1;
    }
    else
    {
        printf(ANSI_COLOR_RED "fail %s" ANSI_COLOR_RESET "\n", name);
        free(tokens);
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

int TestparserFail(char* name)
{
    Result res = calculate_char(name);
    if (res.err != NULL)
    {
        printf(ANSI_COLOR_GREEN "pass %s" ANSI_COLOR_RESET "\n", name);
        return 1;
    }
    else
    {
        printf(ANSI_COLOR_RED "fail %s=%.6g (expected input error)" ANSI_COLOR_RESET "\n", name, res.value);
        return 0;
    }
}

int Testparser(char* name, double expected)
{
    Result res = calculate_char(name);
    if (res.err != NULL)
    {
        printf(ANSI_COLOR_RED "fail %s=%s (expected %.6g)" ANSI_COLOR_RESET "\n", name, res.err, expected);
        return 0;
    }
    else if (fabs(res.value - expected) < 1.e-10)
    {
        printf(ANSI_COLOR_GREEN "pass %s" ANSI_COLOR_RESET "\n", name);
        return 1;
    }
    else
    {
        printf(ANSI_COLOR_RED "fail %s=%.6g (expected %.6g)" ANSI_COLOR_RESET "\n", name, res.value, expected);
        return 0;
    }
}


int main() 
{
    TestTokenx("c(32)",  "c", "(", "32", ")", "");
    TestTokenx("c(32+27)",  "c", "(", "32", "+", "27", ")", "");
    TestTokenx("c(32)*58",  "c", "(", "32", ")", "*", "58", "");
    TestTokenx("c(32)*c(32)",  "c", "(", "32", ")", "*",  "c", "(", "32", ")", "");
    TestTokenx("s(21)",  "s", "(", "21", ")", "");
    TestTokenx("e(10)",  "e", "(", "10", ")", "");
    TestTokenx("3",  "3", "");
    TestTokenx("33",  "33", "");
    TestTokenx("3+33",  "3", "+", "33",  "");
    TestTokenx("3.1+33",  "3.1", "+", "33",  "");
    TestTokenx("(33+3)", "(", "33", "+", "3", ")", "");
    TestTokenx("(33+3)*8/7-5", "(", "33", "+", "3", ")", "*", "8", "/", "7", "-", "5","");
    TestTokenx("(3.3+3)*8/7-5", "(", "3.3", "+", "3", ")", "*", "8", "/", "7", "-", "5","");
    printf("========parser=========\n");
    Testparser("3", 3);
    TestparserFail("c3");
    Testparser("c(98)*c(98)", cos(98)*cos(98));
    Testparser("c(98)", cos(98));
    Testparser("3.1+7", 3.1+7);
    Testparser("3+7-6", 3+7-6);
    Testparser("64*10", 64*10);
    Testparser("6.5*2", 6.5*2);
    Testparser("6.5/2", 6.5/2);
    Testparser("3-3^5+3", 3-pow(3, 5)+3);
    Testparser("-2^5", -pow(2, 5));
    Testparser("c(π)", cos(M_PI));
    Testparser("s(π)", sin(M_PI));
    Testparser("l(1)", log(1));
    Testparser("c(98)", cos(98));
    Testparser("e(1)", exp(1));
    
    return 0;
}
