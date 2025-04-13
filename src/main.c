#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SH_IN_LEN 100
#define CMD_LEN 20
#define BUILTIN_FUNCS 10
#define PATH_MAX_LEN 1024


char *find_binary(const char *cmd) {
	// Get pointer to PATH environment variable
	char *path = getenv("PATH");
	if (path == NULL) {
		return NULL;
	}

	// Duplicate PATH to avoid modifying the original, get the first directory
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

		// Move to the next directory in PATH
		dir = strtok(NULL, ":");
	}
	
	free(path_copy);
	return NULL;
}
	


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
		char builtins[][BUILTIN_FUNCS] = {"exit", "exit 0", "echo", "type"};
		// Extract command (first word) from input
		char cmd[CMD_LEN];
		sscanf(input, "%s", cmd);

		// Execute the command
        if ( !strcmp(cmd, "echo") ) {
			// Echo command
			printf("%s\n", input + strlen("echo") + 1);
        } else if ( !strcmp(cmd, "type") ) {
			// Builtin command check		
			char *cmd_type = input + strlen("type") + 1;
			int builtin_arr_size = sizeof(builtins) / sizeof(builtins[0]);			
			for (int i = 0; i < builtin_arr_size; i++) {
				if ( !strcmp(cmd_type, builtins[i]) ) {
					printf("%s is a shell builtin\n", builtins[i]);
					break;
				} else if (i == builtin_arr_size - 1) {
					printf("%s: not found\n", cmd_type);
				}
			}
		} else {
			// Check if command exists in PATH
			char *path = find_binary(cmd);
			// printf("path: %s\n", path);
			if (path) {
				printf("%s is %s\n", cmd, path);
			} else {
				printf("%s: command not found\n", cmd);
			}
		}
	}

	return 0;
}
