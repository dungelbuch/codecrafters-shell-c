#include "shell.h"

/**
 * @brief Initializes a redirection data structure.
 *
 * @return redirection_t* Pointer to the initialized redirection structure.
 **/
redirection_t *__init_redir_struct() {
    redirection_t *redir = malloc(sizeof(redirection_t));
    redir->output_file = NULL;
    redir->std_type = -1;
    redir->std_fd_backup = -1;
    redir->output_fd = -1;
    return redir;
}

/**
 * @brief Parses tokens for redirection operators (>, 1>, 2>) and sets up file redirection.
 * Removes redirection tokens from the array and redirects stdout/stderr to the specified file.
 *
 * @param tokens Array of command tokens (modified in-place to remove redirection operators).
 * @param redir Pointer to the redirection structure to be configured.
 **/
void __setup_redirection(char **tokens, redirection_t *redir) {
    // Find and remove redirection operator '>' from tokens
    for (int i = 0; tokens[i] != NULL; i++) {
        bool redir_stdout = (strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], "1>") == 0);
        bool redir_stderr = (strcmp(tokens[i], "2>") == 0);

        if (tokens[i + 1] != NULL && (redir_stdout || redir_stderr)) {
            // Duplicate the output file name and check for memory allocation failure
            redir->output_file = strdup(tokens[i + 1]);
            if (redir->output_file == NULL) {
                perror("strdup failed");
                exit(EXIT_FAILURE);
            }

            // Set redirection type
            if (redir_stdout) {
                redir->std_type = STDOUT_FILENO;  // Standard output
            } else if (redir_stderr) {
                redir->std_type = STDERR_FILENO;  // Standard error
            }

            // Remove redirection tokens
            tokens[i] = NULL;
            tokens[i + 1] = NULL;
            break;
        }
    }

    // Redirect if needed
    if (redir->output_file) {
        redir->std_fd_backup = dup(redir->std_type);                                      // backup stdout in case we need to restore it later
        redir->output_fd = open(redir->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);  // Open the output file for writing
        dup2(redir->output_fd, redir->std_type);                                          // making STDOUT point to the output file
    }
}

/**
 * @brief Restores the original stdout and cleans up the redirection structure.
 *
 * @param redir Pointer to the redirection structure.
 **/
void __restore_redirection(redirection_t *redir) {
    if (redir->output_file) {
        dup2(redir->std_fd_backup, redir->std_type);  // Restore the original file descriptor
        close(redir->std_fd_backup);                  // Close the backup file descriptor
        close(redir->output_fd);                      // Close the output file descriptor
        free(redir->output_file);                     // Free the output file string
    }
}