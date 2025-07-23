#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define INPUT_LEN 100
#define CMD_LEN 20
#define NUM_BUILTINS 10
#define PATH_MAX_LEN 1024
#define MAX_NUM_INPUT 10


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
 * @param num_builtins The number of builtin functions.
 * 
 * @return int 1 if the command is a builtin, 0 otherwise.
 **/
int __builtin_func(const char *cmd_type, char builtins[][NUM_BUILTINS], int num_builtins) {
	// Search for the command in the list of builtins and print if found
	for (int i = 0; i < num_builtins; i++) {
		if ( !strcmp(cmd_type, builtins[i]) ) {
			printf("%s is a shell builtin\n", builtins[i]);
			return 1;
			break;
		}
	}

	return 0;
}

/**
 * @brief  This function extracts tokens from a given input string. 
 * It duplicates the input string to avoid modifying the original. Therefore,
 * the caller is responsible for freeing the memory allocated for the tokens.
 * 
 * @param input_str The input string to be tokenized.
 * @param num_args The maximum number of arguments to extract.
 * @param str_copy A pointer to the string copy that will be used for tokenization.
 * @return char** An array of pointers to the extracted tokens.
 **/
char **__extract_tokens(char *str, const int num_args, char **str_copy) {
	char **argv = malloc(num_args * sizeof(char *));
	int agrv_index = 0;

	// Duplicate input string and tokenize it
	*str_copy = strdup(str);
	char *token = strtok(*str_copy, " ");
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
void __fork_and_exec(char *bin, char **args) {
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
/* Helper functions to offload specific commands.                             */
/******************************************************************************/ 

/**
 * @brief This function prints the string passed to it after the echo command.
 * 
 * @param str The string to print.
 **/
void __echo(const char *str) {
	// Extract the string after "echo "
	str = str + strlen("echo ");
	printf("%s\n", str);
}

/**
 * @brief This function changes the current working directory to the one 
 * specified in the input string. 
 * 
 * @param str The directory to change to.
 **/
void __cd(const char *str) {
	// Extract the directory from the input string
	str = str + strlen("cd ");
	if (chdir(str) != 0) {
		fprintf(stderr, "cd: %s: No such file or directory\n", str);
	}
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
 * @param num_builtins The number of builtin functions.
 **/
void __handle_type_cmd(const char *cmd, char builtins[][NUM_BUILTINS], int num_builtins){
	// Check if command is a builtin	
	const char *cmd_type = cmd + strlen("type") + 1;	// +1 to skip the space after 'type'		
	int is_builtin = __builtin_func(cmd_type, builtins, NUM_BUILTINS);

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
void __handle_ext_cmd(const char *cmd, char **args){
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
	char builtins[][NUM_BUILTINS] = {"exit", "echo", "type", "pwd", "cd"};
 
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
		char *input_copy = NULL;
		char **args = __extract_tokens(input, MAX_NUM_INPUT, &input_copy);
		char *cmd = args[0];
		
        if ( !strcmp(cmd, "echo") ) {
			__echo(input);
		} else if ( !strcmp(cmd, "cd") ) {
			__cd(input);
		} else if ( !strcmp(cmd, "pwd") ) {
			__pwd();
        } else if ( !strcmp(cmd, "type") ) {
			__handle_type_cmd(input, builtins, NUM_BUILTINS);
		} else {
			__handle_ext_cmd(cmd, args);
		}
		
		// Free the memory allocated for the input string's copy
		free(input_copy);
		free(args);
	}

	return 0;
}
