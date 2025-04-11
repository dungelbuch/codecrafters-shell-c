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

    // Check for exit command
    if (strcmp(input, "exit 0") == 0) {
      break;
    }

    printf("%s: command not found\n", input);
  }

  return 0;
}
