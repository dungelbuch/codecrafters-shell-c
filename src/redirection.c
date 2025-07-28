#include "shell.h"

/**
 * @brief Initializes a redirection data structure.
 *
 * @return redirection_t* Pointer to the initialized redirection structure.
 **/
redirection_t *__init_redir_struct() {
    redirection_t *redir = malloc(sizeof(redirection_t));
    redir->output_file = NULL;
    redir->stdout_fd_backup = -1;
    redir->output_fd = -1;
    return redir;
}

/**
 * @brief Configures output redirection by finding the '>' operator in tokens and setting up the redirection.
 *
 * @param tokens Array of command tokens.
 * @param redir Pointer to the redirection structure to be configured.
 **/
void __setup_redirection(char **tokens, redirection_t *redir) {
    // Find and remove redirection operator '>' from tokens
    for (int i = 0; tokens[i] != NULL; i++) {
        if (tokens[i + 1] != NULL && (strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], "1>") == 0)) {
            redir->output_file = strdup(tokens[i + 1]);
            // remove the redirection operator and the file name from tokens and null-terminate the array
            free(tokens[i]);
            free(tokens[i + 1]);
            tokens[i] = NULL;
            break;
        }
    }

    // Redirect if needed
    if (redir->output_file) {
        redir->stdout_fd_backup = dup(STDOUT_FILENO);                                     // backup stdout in case we need to restore it later
        redir->output_fd = open(redir->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);  // Open the output file for writing
        dup2(redir->output_fd, STDOUT_FILENO);                                            // making STDOUT point to the output file
    }
}

/**
 * @brief Restores the original stdout and cleans up the redirection structure.
 *
 * @param redir Pointer to the redirection structure.
 **/
void __restore_redirection(redirection_t *redir) {
    // Restore stdout
    if (redir->output_file) {
        // Restore stdout to its original state
        dup2(redir->stdout_fd_backup, STDOUT_FILENO);
        // Close the backup and output file descriptors
        close(redir->stdout_fd_backup);
        close(redir->output_fd);
        // Free the output file string
        free(redir->output_file);
    }
}