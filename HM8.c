#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int choice;

    printf("Enter 1 to run the first child process, 2 to run the second child process, or 0 to exit: ");
    scanf("%d", &choice);

    if (choice == 1 || choice == 2) {
        pid_t pid1, pid2;

        if (choice == 1) {
            pid1 = fork();
            if (pid1 == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (pid1 == 0) {
                // First child process
                printf("First child process is running.\n");
                // Add your code for the first child process here
                // ...
                printf("First child process completed.\n");
                exit(EXIT_SUCCESS);
            }
        } else if (choice == 2) {
            pid2 = fork();
            if (pid2 == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (pid2 == 0) {
                // Second child process
                printf("Second child process is running.\n");
                // Add your code for the second child process here
                // ...
                printf("Second child process completed.\n");
                exit(EXIT_SUCCESS);
            }
        }

        // Parent process
        int status;
        if (choice == 1) {
            waitpid(pid1, &status, 0);
            if (WIFEXITED(status)) {
                printf("First child process exited with status %d.\n", WEXITSTATUS(status));
            } else {
                printf("First child process exited abnormally.\n");
            }
        } else if (choice == 2) {
            waitpid(pid2, &status, 0);
            if (WIFEXITED(status)) {
                printf("Second child process exited with status %d.\n", WEXITSTATUS(status));
            } else {
                printf("Second child process exited abnormally.\n");
            }
        }
    }

    printf("Program finished.\n");
    return 0;
}