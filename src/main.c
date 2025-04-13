#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SH_IN_LEN 100
#define CMD_LEN 20
#define BUILTIN_FUNCS 10
#define PATH_MAX_LEN 1024


/**
 * @brief This function check if a given command exists in the system's PATH.
 * 
 * @param cmd The command to search for.
 * @return char* The full path of the command if found, NULL otherwise.
 **/
char *find_binary(const char *cmd) {
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
 * @brief Main function of the shell.
 * 
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return int Exit status of the shell.
 **/
int main(int argc, char *argv[]) {

    while (1) {
        // Print the prompt and flush stdout
        printf("$ ");
        fflush(stdout);

        // Wait for user input and replace the newline with null terminator
        char input[SH_IN_LEN];
        fgets(input, SH_IN_LEN, stdin);
        input[strlen(input) - 1] = '\0';

		// Check for exit command
		if (!strcmp(input, "exit") || !strcmp(input, "exit 0")) {
			return 0;
		}
		
		// List of builtin commands
		char builtins[][BUILTIN_FUNCS] = {"exit", "echo", "type"};
		// Extract command (first word) from input
		char cmd[CMD_LEN];
		sscanf(input, "%s", cmd);

		// Execute the command
        if ( !strcmp(cmd, "echo") ) {
			printf("%s\n", input + strlen("echo") + 1);
        } else if ( !strcmp(cmd, "type") ) {
			// Check if command is a builtin	
			int is_builtin = 0;
			const char *cmd_type = input + strlen("type") + 1;			
			for (int i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++) {
				if ( !strcmp(cmd_type, builtins[i]) ) {
					printf("%s is a shell builtin\n", builtins[i]);
					is_builtin = 1;
					break;
				}
			}

			// If not, check if it's an executable in PATH
			if (!is_builtin) {
				char *path = find_binary(cmd_type);
				//printf("path: %s\n", path);
				if (path) {
					printf("%s is %s\n", cmd_type, path);
				} else {
					printf("%s: not found\n", cmd_type);
				}
			}
	
		} else {
			printf("%s: command not found\n", cmd);
		}
	}

	return 0;
}
