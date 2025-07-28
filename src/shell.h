#ifndef SHELL_H
#define SHELL_H

// All includes in one place
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// All constants in one place
#define NUM_BUILTINS 10
#define INPUT_LEN 1024
#define PATH_MAX_LEN 1024

// Builtin functions (from builtins.c)
void __echo(char **args);
void __cd(char **args);
void __pwd(void);
int __builtin_func(const char *cmd_type, const char *builtins[]);
void __type(char **args, const char *builtins[]);

// External command functions (from external.c)
char *__find_binary(const char *cmd);
void __fork_and_exec(char *bin, char **args);
void __ext_cmd(const char *cmd, char **args);

// Tokenizer function (from main.c)
char **__get_tokens(char *str);

#endif // SHELL_H