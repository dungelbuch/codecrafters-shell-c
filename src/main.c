#include "shell.h"

/**
 * @brief Extracts and parses tokens from a given string, handling quotes and escapes.
 * Allocates memory for each token. The caller is responsible for freeing the memory.
 *
 * @param str The input string to be tokenized.
 * @return char** An array of pointers to the extracted tokens, NULL-terminated.
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

    // Resize token array and null-terminate it
    argv = realloc(argv, (idx + 1) * sizeof(char *));
    argv[idx] = NULL;

    return argv;
}

/**
 * @brief Main function of the shell. Implements a REPL loop for command processing.
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
        if (!strcmp(input, "exit") || !strcmp(input, "exit 0")) return 0;

        // Extract tokens from input
        char **toks = __get_tokens(input);
        if (toks[0] == NULL) {
            // If no command is entered, free tokens and continue
            for (int i = 0; toks[i] != NULL; i++) free(toks[i]);
            free(toks);
            continue;
        }

        // Redirection handling
        redirection_t *redir = __init_redir_struct();
        __setup_redirection(toks, redir);

        // Execute the command based on the first token
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

        // Restore redirection if it was setup
        __restore_redirection(redir);

        // Free the memory allocated for tokens
        for (int i = 0; toks[i] != NULL; i++) free(toks[i]);
        free(toks);
    }

    return 0;
}
