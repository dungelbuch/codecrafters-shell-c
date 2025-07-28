#include "shell.h"

/**************************************************************************************************/
/* Helper functions to offload specific commands.                                                 */
/**************************************************************************************************/

/**
 * @brief Prints the arguments passed to it, separated by spaces.
 *
 * @param args The arguments to print.
 */
void __echo(char **args) {
    // skip the "echo" command at args[0]
    for (int i = 1; args[i] != NULL; i++) {
        if (i > 1) printf(" ");
        printf("%s", args[i]);
    }
    putchar('\n');
}

/**
 * @brief Changes the current working directory. Supports '~' expansion and defaults to HOME.
 *
 * @param args The arguments array where args[1] is the target directory.
 **/
void __cd(char **args) {
    const char *dir = args[1];  // skip the command itself

    // If no directory is specified, change to home directory
    if (dir == NULL) {
        const char *home = getenv("HOME");
        if (chdir(home) != 0) perror("cd");
        return;
    }

    // If the directory starts with '~', replace it with the home directory
    if (dir[0] == '~') {
        const char *home = getenv("HOME");
        int len = strlen(home) + strlen(dir) + 1;  // +1 for null terminator
        char *home_dir = malloc(len * sizeof(char));
        snprintf(home_dir, len, "%s%s", home, dir + 1);
        if (chdir(home_dir) != 0) {  // Change the directory
            fprintf(stderr, "cd: %s: No such file or directory\n", home_dir);
        }
        free(home_dir);
    } else {
        // Change the directory
        if (chdir(dir) != 0) {
            fprintf(stderr, "cd: %s: No such file or directory\n", dir);
        }
    }
    return;
}

/**
 * @brief Print current working directory.
 *
 **/
void __pwd() {
    char cwd[PATH_MAX_LEN];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd() error");
    }
}

/**
 * @brief Checks if a command is a builtin function and prints the result.
 *
 * @param cmd_type The command to check.
 * @param builtins The list of builtin functions.
 *
 * @return int 1 if the command is a builtin, 0 otherwise.
 **/
int __builtin_func(const char *cmd_type, const char *builtins[]) {
    // Search for the command in the list of builtins and print if found
    for (int i = 0; builtins[i] != NULL; i++) {
        if (!strcmp(cmd_type, builtins[i])) {
            printf("%s is a shell builtin\n", builtins[i]);
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Handles the 'type' command, checking if a command is builtin or executable in PATH.
 *
 * @param args The command arguments where args[1] is the command to check.
 * @param builtins The list of builtin functions.
 **/
void __type(char **args, const char *builtins[]) {
    // Check if there's an argument after "type"
    if (args[1] == NULL) {
        printf("type: missing argument\n");
        return;
    }

    // Check if command is a builtin
    char *cmd_type = args[1];
    int is_builtin = __builtin_func(cmd_type, builtins);

    // If not, check if it's an executable in PATH
    if (!is_builtin) {
        extern char *__find_binary(const char *cmd);  // Forward declaration
        char *bin = __find_binary(cmd_type);
        bin ? printf("%s is %s\n", cmd_type, bin) : printf("%s: not found\n", cmd_type);
    }
}