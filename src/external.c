#include "shell.h" 

/**
 * @brief This function checks if a given command exists in the system's PATH.
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
 * @brief Forks a new process and executes a binary in it, waiting for completion.
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

/**
 * @brief Handles external commands, by finding the binary in PATH and executing it.
 *
 * @param cmd The command to execute.
 * @param args The arguments for the command.
 */
void __ext_cmd(const char *cmd, char **args) {
    char *bin = __find_binary(cmd);
    bin ? __fork_and_exec(bin, args) : printf("%s: command not found\n", cmd);
}