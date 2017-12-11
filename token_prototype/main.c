#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <regex.h>

#define MAX_TOKENS 20

struct Token {
    char name[100];
    regex_t *pattern;
    void (*handler)(Job *, char *);
};

typedef struct Token Token;

struct Job {
    char name[100];
    char cmd[100];
    int interval;
};

typedef struct Job Job;

static Token tokens[MAX_TOKENS];
static int nr_of_tokens;

static Token create_token(const char *name, const char *pattern, void (*handler)(Job *, char *));

static void job_handler(Job *current_job, char *token)
{
    strcpy(current_job->name, "");
}

static void interval_handler(Job *current_job, char *token)
{
    puts("Matched 'interval'!");
}

static void command_handler(Job *current_job, char *token)
{
    puts("Matched 'command'!");
}

static void init_tokens(void) 
{
    nr_of_tokens = 0;

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

static Token create_token(const char *name, const char *pattern, void (*handler)(Job *, char *))
{
    Token result;
    
    strcpy(result.name, name);
    regex_t *regex = (regex_t *)malloc(sizeof (regex_t));
    regcomp(regex, pattern, REG_ICASE | REG_NEWLINE | REG_EXTENDED);
    result.pattern = regex;
    result.handler = handler;
    
    nr_of_tokens++;

    return result;
}

int main(void) 
{
    const char lines[] = {"Job \"Test\"\nrun `echo \"hello\"`\nevery 3 minutes\nJob \"Test 2\"\nrun `echo \"hello 2\"\nevery second`"};
    init_tokens();

    char *tok = NULL;

    tok = strtok(lines, "\n");
    
    Job jobs[10];
    Job *current_job = &(jobs[0]);

    while ((tok = strtok(NULL, "\n")) != NULL)
    {
        for (int i = 0; i < nr_of_tokens; i++)
        {
            int return_val = regexec(tokens[i].pattern, lines[i], 0, NULL, 0);
            
            if (return_val == 0) 
            {
                tokens[i].handler(current_job, tok);
            }
            else if (return_val != REG_NOMATCH)
            {
                puts("Error");
            }
        }
    }
    
    return 0;
}
