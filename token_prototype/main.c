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
    void (*handler)(Job **, char *, regmatch_t *);
};

typedef struct Token Token;

static Job jobs[10];
int jobs_found = 0;

static Token tokens[MAX_TOKENS];
static int nr_of_tokens;

static Token create_token(const char *name, const char *pattern, void (*handler)(Job **, char *, regmatch_t *));
static void run_regex(char *lines, Job **current_job);

static void job_handler(Job **current_job, char *text, regmatch_t matches[])
{
    //printf("Matched 'Job'!\n");    
    
    // Set job name 
    int job_name_len = matches[1].rm_eo - matches[1].rm_so;
    char *job_name = (char *)malloc(job_name_len + 1);
    strncpy(job_name, text + matches[1].rm_so, job_name_len);
    job_name[job_name_len] = '\0';
    //puts(job_name);
    strcpy((*current_job)->name, job_name);
    
    int job_text_len = matches[2].rm_eo - matches[2].rm_so;
    char *job_text = (char *)malloc(job_text_len + 1);
    strncpy(job_text, text + matches[2].rm_so, job_text_len);
    job_text[job_text_len] = '\0';
    //puts(job_text);
    run_regex(job_text, current_job);
    free(job_text);

    jobs_found = jobs_found + 1;
    *current_job = &(jobs[jobs_found]);
}

static void interval_handler(Job **current_job, char *text, regmatch_t matches[])
{
    //printf("Matched 'interval'!\n");

    int interval_raw_len = matches[1].rm_eo - matches[1].rm_so;
    char *interval_raw = (char *)malloc(interval_raw_len + 1);
    strncpy(interval_raw, text + matches[1].rm_so, interval_raw_len);
    interval_raw[interval_raw_len] = '\0';
    //puts(interval_raw);
    int interval = atoi(interval_raw);
    if (interval == 0) 
    {
        interval = 1;
    }
    free(interval_raw);

    int unit_len = matches[2].rm_eo - matches[2].rm_so;
    char *unit = (char *)malloc(unit_len + 1);
    strncpy(unit, text + matches[2].rm_so, unit_len);
    unit[unit_len] = '\0';
    //puts(unit);

    if (strncmp(unit, "m", 1) == 0) 
    {
        interval = interval * 60;
    } 
    else if (strncmp(unit, "h", 1) == 0) 
    {
        interval = interval * 3600;
    }

    free(unit);

    (*current_job)->interval = interval;
}

static void command_handler(Job **current_job, char *text, regmatch_t matches[])
{
    //printf("Matched 'command'!\n");

    int command_len = matches[1].rm_eo - matches[1].rm_so;
    char *command = (char *)malloc(command_len + 1);
    strncpy(command, text + matches[1].rm_so, command_len);
    command[command_len] = '\0';
    //puts(command);

    strcpy((*current_job)->cmd, command);
}

static void reaction_handler(Job **current_job, char *text, regmatch_t matches[])
{
}

static void init_tokens(void) 
{
    nr_of_tokens = 0;

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

static Token create_token(const char *name, const char *pattern, void (*handler)(Job **, char *, regmatch_t *))
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

static void run_regex(char *lines, Job **current_job)
{
    int lines_len = strlen(lines);
    //printf("%d characters\n\n%s\n\n", lines_len, lines);
    int MAX_GROUPS = 20;
    long offset = 0;

    for (int i = 0; i < nr_of_tokens; i++)
    {
        regmatch_t matches[MAX_GROUPS]; 

        while (regexec(tokens[i].pattern, lines + offset, MAX_GROUPS, matches, 0) == 0)
        {
            tokens[i].handler(current_job, lines + offset, matches);
            offset += matches[0].rm_eo;
        }
    }

    //puts("end of run_regex");
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

    Job *current_job = &(jobs[0]);

    run_regex(lines, &current_job);

    for (int i = 0; i < jobs_found; i++) 
    {
        printf("Job nr %d:\n", i + 1);
        printf("Name: '%s'\nCmd: '%s'\ninterval: %d\n\n", jobs[i].name, jobs[i].cmd, jobs[i].interval);
    }

    return 0;
}
