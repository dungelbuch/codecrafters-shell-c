#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#define NUM_BUILTINS 10
#define INPUT_LEN 1024
#define PATH_MAX_LEN 1024

/**
 * @brief This function check if a given command exists in the system's PATH.
 *
 * @param cmd The command to search for.
 * @return char* The full path of the command if found, NULL otherwise.
 **/
char *__find_binary(const char *cmd) {
    // Get pointer to PATH environment variable
    char *path = getenv("PATH");
    if (path == NULL) return NULL;

    // Duplicate PATH to avoid modifying the original, get first token
    char *path_copy = strdup(path);
    char *dir = strtok(path_copy, ":");
    static char full_path[PATH_MAX_LEN];

    while (dir != NULL) {
        // Append command to directory to form full path
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
        // Check if the file exists and is executable
        int bin_exist = access(full_path, X_OK);
        if (bin_exist == 0) {
            free(path_copy);
            return full_path;
        }
        // Move to the next token
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return NULL;
}

/**
 * @brief Find builtin functions in the shell and print if found.
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
 * @brief This function extracts tokens from a given string.
 * It duplicates the input string to avoid modifying the original. Therefore,
 * the caller is responsible for freeing the memory allocated for the tokens.
 *
 * @param str The input string to be tokenized.
 * @return char** An array of pointers to the extracted tokens.
 **/
char **__get_tokens(char *str) {
    int capacity = 10;  // Initial capacity for the array of pointers
    char **argv = malloc(capacity * sizeof(char *));
    int idx = 0;
	
    // Skip leading spaces
    char *ptr = str;
    while (*ptr == ' ') ptr++;

    while (*ptr) {
        // allocate memory for a new token
        char *tok = malloc(strlen(str) + 1);
        char *write_ptr = tok;

        // process all consecutive parts as a single token
        while (*ptr && *ptr != ' ') {
            if (*ptr == '\'') {
                // handles single quotes 
                char quote = *ptr++;  
                while (*ptr && *ptr != quote) *write_ptr++ = *ptr++; 
                if (*ptr == quote) ptr++;  
            } else if (*ptr == '\"') {
                // handles double quotes
                char quote = *ptr++;  
                while (*ptr && *ptr != quote) {
                    // skip backslash for escaped \ or "
                    if (*ptr == '\\' && (*(ptr + 1) == '\\' || *(ptr + 1) == '\"')) {
                        ptr++;  
                    }
                    *write_ptr++ = *ptr++;
                }
                if (*ptr == quote) ptr++;  
			} else if (*ptr == '\\') {
                // handles escape char outside quotes -> preserve next char
				ptr++;
				if (*ptr) *write_ptr++ = *ptr++;
            } else {
                // copy normal char
                *write_ptr++ = *ptr++;
            }
        }

        *write_ptr = '\0';  // null-term the token

        // Reallocate if needed
        if (idx == capacity) {
            capacity *= 2;
            argv = realloc(argv, capacity * sizeof(char *));
            if (argv == NULL) {
                perror("Failed to allocate memory for tokens");
                exit(EXIT_FAILURE);
            }
        }

        // Add the token to array and skip trailing spaces until next character
        argv[idx++] = tok;
        while (*ptr == ' ') ptr++;
    }

    // Resize and null-terminate the array of tokens
    argv = realloc(argv, (idx + 1) * sizeof(char *));
    argv[idx] = NULL;

    return argv;
}

/**
 * @brief This function forks a new process and executes a binary in it.
 *
 * @param bin The binary to execute.
 * @param args The arguments to pass to the binary.
 **/
void __fork_and_exec(char *bin, char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execv(bin, args);
        perror("execv failed");
        exit(1);  // If execv fails
    } else if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}

/******************************************************************************/
/* Helper functions to offload specific commands.                             */
/******************************************************************************/

/**
 * @brief This function prints the arguments passed to it.
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
 * @brief This function changes the current working directory to the one
 * specified in the input string.
 *
 * @param str The directory to change to.
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
 * @brief This function handles the 'type' command in the shell.
 * It checks if the command is a builtin or an executable in PATH.
 *
 * @param cmd The command to check.
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
        char *bin = __find_binary(cmd_type);
        bin ? printf("%s is %s\n", cmd_type, bin) : printf("%s: not found\n", cmd_type);
    }
}

/**
 * @brief Handle external commands, by finding the binary in PATH and executing it.
 *
 * @param cmd The command to execute.
 * @param args The arguments for the command.
 */
void __ext_cmd(const char *cmd, char **args) {
    char *bin = __find_binary(cmd);
    bin ? __fork_and_exec(bin, args) : printf("%s: command not found\n", cmd);
}

/******************************************************************************/
/* Main function of the shell.                                                */
/******************************************************************************/

/**
 * @brief Main function of the shell.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return int Exit status of the shell.
 **/
int main(int argc, char *argv[]) {
    // List of builtin commands
    const char *builtins[] = {"exit", "echo", "type", "pwd", "cd", NULL};

    while (1) {
        // Print the prompt and flush stdout
        printf("$ ");
        fflush(stdout);

        // Wait for user input and replace the newline with null terminator
        char input[INPUT_LEN];
        fgets(input, INPUT_LEN, stdin);
        input[strlen(input) - 1] = '\0';

        // Check for exit command
        if (!strcmp(input, "exit") || !strcmp(input, "exit 0")) {
            return 0;
        }

        // Extract tokens from input
        char **toks = __get_tokens(input);
        if (toks[0] == NULL) {
            // If no command is entered, free tokens and continue
            for (int i = 0; toks[i] != NULL; i++) free(toks[i]);
            free(toks);
            continue;
        }
        char *cmd = toks[0];

        if (!strcmp(cmd, "echo")) {
            __echo(toks);
        } else if (!strcmp(cmd, "cd")) {
            __cd(toks);
        } else if (!strcmp(cmd, "pwd")) {
            __pwd();
        } else if (!strcmp(cmd, "type")) {
            __type(toks, builtins);
        } else {
            __ext_cmd(cmd, toks);
        }

        // Free the memory allocated for tokens
        for (int i = 0; toks[i] != NULL; i++) {
            free(toks[i]);
        }
        free(toks);
    }

    return 0;
}
