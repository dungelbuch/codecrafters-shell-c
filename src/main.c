#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define INPUT_LEN 100
#define CMD_LEN 20
#define BUILTIN_FUNCS 10
#define PATH_MAX_LEN 1024
#define MAX_NUM_INPUT 10


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
 * @brief  This function extracts tokens from a given input string. 
 * It duplicates the input string to avoid modifying the original. Therefore,
 * the caller is responsible for freeing the memory allocated for the tokens.
 * 
 * @param input_str The input string to be tokenized.
 * @param num_args The maximum number of arguments to extract.
 * @return char** An array of pointers to the extracted tokens.
 **/
char **extract_tokens(char *str, const int num_args) {
	char **argv = malloc(num_args * sizeof(char *));
	int agrv_index = 0;

	// Duplicate input string and tokenize it
	char *str_copy = strdup(str);
	char *token = strtok(str_copy, " ");
	while (token != NULL && agrv_index < num_args - 1) {
		argv[agrv_index++] = token;
		token = strtok(NULL, " ");
	}
	// Add NULL terminator to the end of the array
	argv[agrv_index] = NULL;

	return argv;
} 

/**
 * @brief This function forks a new process and executes a binary in it.
 * 
 * @param bin The binary to execute.
 * @param args The arguments to pass to the binary.
 **/
void fork_and_exec(char *bin, char **args) {
	pid_t pid = fork();
	if (pid == 0) {
		// Child process
		execv(bin, args);
		perror("execv failed");
		exit(1); // If execv fails
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
	char builtins[][BUILTIN_FUNCS] = {"exit", "echo", "type"};
 
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
		char **args = extract_tokens(input, MAX_NUM_INPUT);
		char *cmd = args[0];

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
			char *bin_path = find_binary(cmd);
			if (bin_path) {
				fork_and_exec(bin_path, args);
			} else {
				printf("%s: command not found\n", cmd);
			}
		}


		
	}

	return 0;
}
