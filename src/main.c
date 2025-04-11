#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    while (1) {
        // Flush after every printf
        fflush(stdout);

        // Uncomment this block to pass the first stage
        printf("$ ");

        // Wait for user input and replace the newline with null terminator
        char input[100];
        fgets(input, 100, stdin);
        input[strlen(input) - 1] = '\0';

		// Check for exit command
		if (!strcmp(input, "exit") | !strcmp(input, "exit 0")) {
			return 0;
		}
		
		// List of builtin commands
		char builtins[][16] = {"exit", "exit 0", "echo", "type"};
		// Extract command from input
		char command[100];
		sscanf(input, "%s", command);

		// Execute the command
        if (strcmp(command, "echo") == 0) {
			printf("%s\n", input + strlen("echo") + 1);

        } else if (strcmp(command, "type") == 0) {
			// Check if the command is a builtin			
			char *command_type = input + strlen("type") + 1;
			int builtin_arr_size = sizeof(builtins) / sizeof(builtins[0]);			
			for (int i = 0; i < builtin_arr_size; i++) {
				if (strcmp(command_type, builtins[i]) == 0) {
					printf("%s is a shell builtin\n", builtins[i]);
					break;
				} else if (i == builtin_arr_size - 1) {
					printf("%s: not found\n", command_type);
				}
			}

		} else {
			printf("%s: command not found\n", input);
		}
    }

    return 0;
}
