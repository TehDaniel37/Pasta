#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <regex.h>

#define MAX_TOKENS 20

struct Token {
    char name[100];
    regex_t *pattern;
    void (*handler)(void);
};

typedef struct Token Token;

static Token tokens[MAX_TOKENS];

static Token create_token(const char *name, const char *pattern, void (*handler)(void));

static void job_handler(void)
{
    puts("Matched 'Job'!");
}

static void interval_handler(void)
{
    puts("Matched 'interval'!");
}

static void command_handler(void)
{
    puts("Matched 'command'!");
}

static void init_tokens(void) 
{
    tokens[0] = create_token("Job",
            "Job\\s+\".*\"",
            job_handler);
            
    tokens[1] = create_token("command",
            "run\\s+`.*`",
            command_handler);  
            
    tokens[2] = create_token("interval",
            "every\\s+[0-9]+\\s+(s|sec|second|seconds)|(m|min|minute|minutes)|(h|hour|hours)",
            interval_handler);
}

static Token create_token(const char *name, const char *pattern, void (*handler)(void))
{
    Token result;
    
    strcpy(result.name, name);
    regex_t *regex = (regex_t *)malloc(sizeof (regex_t));
    regcomp(regex, pattern, REG_ICASE | REG_NEWLINE | REG_EXTENDED);
    result.pattern = regex;
    result.handler = handler;
    
    return result;
}

int main(void) 
{
    const char *lines[3] = {"Job \"Test\"", "run `echo \"hello\"`", "every 3 minutes"};
    init_tokens();
    
    for (int i = 0; i < 3; i++)
    {
        int return_val = regexec(tokens[i].pattern, lines[i], 0, NULL, 0);
        
        if (!return_val) 
        {
            printf("Token '%s' matches line '%s'\n", tokens[i].name, lines[i]);
        }
        else if (return_val == REG_NOMATCH)
        {
            printf("Token '%s' does not match line '%s'\n", tokens[i].name, lines[i]);
        }
        else
        {
            puts("error");
        }
    }
    
    return 0;
}
