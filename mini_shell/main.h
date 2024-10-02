#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include<errno.h>

#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND  3

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct cmd_node{
    char cmd[256];
    pid_t cmd_pid;
    struct cmd_node* link;
}sigstore;

void scan_input();
char *get_command(char* input_string);

void copy_change();

int check_command_type(char* command);
void execute_internal_commands(char* input_string);
void signal_handler(int sig_num);
void execute_external_commands(char* input_string);
int ispipe(char* input_string);
void _delete();
void split_args(char *command, char *args[]);
void execute_n_pipes(int n, char *commands[]);

#endif
