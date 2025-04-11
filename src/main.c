#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  while (1) {
    // Flush after every printf
    fflush(stdout);

    // Uncomment this block to pass the first stage
    printf("$ ");

    // Wait for user input and replace the newline character with a null terminator
    char input[100];
    fgets(input, 100, stdin);
    input[strlen(input) - 1] = '\0';

    // Extract first string of input
    char command[100];
    sscanf(input, "%s", command);

    // Check for exit command
    if (strcmp(command, "echo") == 0) {
      printf("%s\n", input + strlen("echo") + 1);
    } else if (strcmp(command, "exit") == 0) {
      break; 
    } else {
      printf("%s: command not found\n", input);
    }

  }

  return 0;
}
