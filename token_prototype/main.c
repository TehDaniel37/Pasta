#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <regex.h>

#define MAX_TOKENS 20

struct Job {
    char name[100];
    char cmd[100];
    int interval;
};

typedef struct Job Job;

struct Token {
    char name[100];
    regex_t *pattern;
    void (*handler)(Job *, char *);
};

typedef struct Token Token;

static Token tokens[MAX_TOKENS];
static int nr_of_tokens;

static Token create_token(const char *name, const char *pattern, void (*handler)(Job *, char *));

static void job_handler(Job *current_job, char *token)
{
    printf("Matched 'Job'!\t");
}

static void interval_handler(Job *current_job, char *token)
{
    printf("Matched 'interval'!\t");
}

static void command_handler(Job *current_job, char *token)
{
    printf("Matched 'command'!\t");
}

static void reaction_handler(Job *current_job, char *token)
{
    printf("Matched 'reaction'!\t");
}

static void init_tokens(void) 
{
    nr_of_tokens = 0;

    // ^Job\\s+\"(.*)\"\\s+(.*?)(?:\\r*\\n){2}

    tokens[0] = create_token("job",
            "^Job\\s+\"([^\"]*)\"\\s+((\n?.)*)",
            job_handler);

    tokens[1] = create_token("command",
            "run\\s+`(.*)`",
            command_handler);

    tokens[2] = create_token("interval",
            "every\\s*([0-9]+)?\\s+(hours|hour|h|minutes|minute|min|m|seconds|second|sec|s)",
            interval_handler);

    tokens[3] = create_token("reaction",
            "when\\s+`(.*)`\\s*((outputs)\\s+\"(.*)\")?",
            reaction_handler);
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
    char lines[] = {
"Job \"Test\" \n\
    run `this command`\n\
    every 5 seconds\n\
\n\
Job \"Test 2\" run `that command` every second\n\
\n\
Job \"Test\" run `echo \"hello\"` every 3 minutes\n\
\n\
Job \"Test 2\"\n\
run `echo \"hello 2\"`\n\
every second\n\n\
\
Job \"Test 3\"\n\
run `echo \"every 3 minutes\"`\n\
when `bspc subscribe desktop`\n\n\
\
Job \"Test 4\"\n\
run `whoami`\n\
when `usb_connected.sh` outputs \"true\""};

    init_tokens();

    int jobs_found = 0;
    Job jobs[10];
    Job *current_job = &(jobs[0]);

    printf("%s\n", lines);
    int MAX_GROUPS = 5;

    for (int i = 0; i < nr_of_tokens; i++)
    {
        regmatch_t matches[MAX_GROUPS]; 
        long offset = 0;

        while (regexec(tokens[i].pattern, lines + offset, MAX_GROUPS, matches, 0) == 0)
        {
            tokens[i].handler(current_job, lines);
            printf("%d - %d\n", offset + matches[2].rm_so, offset + matches[2].rm_eo);
            offset += matches[0].rm_eo;
        }
    }

    return 0;
}
