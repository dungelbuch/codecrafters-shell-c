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
#define INPUT_LEN 1024
#define PATH_MAX_LEN 1024

// Tokenizer function (from main.c)
char **__get_tokens(char *str);

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

// Redirection functions (from redirection.c)
typedef struct {
    char *output_file;     // Output file for redirection
    int stdout_fd_backup;  // Backup of original stdout file descriptor
    int output_fd;         // File descriptor for the output file
} redirection_t;
redirection_t *__init_redir_struct(void);
void __setup_redirection(char **tokens, redirection_t *redir);
void __restore_redirection(redirection_t *redir);


#endif // SHELL_H