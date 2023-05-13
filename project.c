#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

void printAccessRights(mode_t mode) {
    printf("Access Rights:\n");
    printf("User:\n");
    printf("Read - %s\n", (mode & S_IRUSR) ? "yes" : "no");
    printf("Write - %s\n", (mode & S_IWUSR) ? "yes" : "no");
    printf("Exec - %s\n", (mode & S_IXUSR) ? "yes" : "no");
    printf("\n");
    printf("Group:\n");
    printf("Read - %s\n", (mode & S_IRGRP) ? "yes" : "no");
    printf("Write - %s\n", (mode & S_IWGRP) ? "yes" : "no");
    printf("Exec - %s\n", (mode & S_IXGRP) ? "yes" : "no");
    printf("\n");
    printf("Others:\n");
    printf("Read - %s\n", (mode & S_IROTH) ? "yes" : "no");
    printf("Write - %s\n", (mode & S_IWOTH) ? "yes" : "no");
    printf("Exec - %s\n", (mode & S_IXOTH) ? "yes" : "no");
    printf("\n");
}

void regularFileOptions(char* filename, char* options) {
    struct stat fileStat;
    if (stat(filename, &fileStat) < 0) {
        perror("stat");
        return;
    }

    for (int i = 1; i < strlen(options); i++) {
        switch (options[i]) {
            case 'n':
                printf("Name: %s\n", filename);
                break;
            case 'd':
                printf("Size: %ld bytes\n", fileStat.st_size);
                break;
            case 'h':
                printf("Hard Link Count: %ld\n", fileStat.st_nlink);
                break;
            case 'm':
                printf("Last time the file was modified: %s\n", ctime(&fileStat.st_mtime));
            case 'a':
                printAccessRights(fileStat.st_mode);
                break;
            case 'l':
                printf("Enter the name of the symbolic link: ");
                char linkname[100];
                scanf("%s", linkname);
                symlink(filename, linkname);
                printf("Symbolic link created: %s -> %s\n", linkname, filename);
                break;
            default:
                printf("Invalid option: %c\n", options[i]);
                return;
        }
    }
}

void symbolicLinkOptions(char* filename, char* options) {
    struct stat fileStat;
    if (lstat(filename, &fileStat) < 0) {
        perror("lstat");
        return;
    }

    for (int i = 1; i < strlen(options); i++) {
        switch (options[i]) {
            case 'n':
                printf("Name: %s\n", filename);
                break;
            case 'l':
                printf("Deleting symbolic link: %s\n", filename);
                unlink(filename);
                break;
            case 'd':
                printf("Size of Symbolic Link: %ld bytes\n", fileStat.st_size);
                break;
            case 't':
                if (S_ISLNK(fileStat.st_mode)) {
                    char target[100];
                    ssize_t len = readlink(filename, target, sizeof(target) - 1);
                    if (len != -1) {
                        target[len] = '\0';
                        struct stat targetStat;
                        if (stat(target, &targetStat) == 0) {
                            printf("Size of Target File: %ld bytes\n", targetStat.st_size);
                    } else {
                        perror("stat");
                        return;
                    }   
                    } else {
                        perror("readlink");
                        return;
                    }   
                    } else {
                    printf("Invalid option: %c\n", options[i]);
                    return;
                    }
                break;
            case 'a':
                printAccessRights(fileStat.st_mode);
                break;
            default:
                printf("Invalid option: %c\n", options[i]);
                return;
        }
    }
}

void directoryOptions(char* dirname, char* options) {
    struct stat dirStat;
    if (stat(dirname, &dirStat) < 0) {
        perror("stat");
        return;
    }

    for (int i = 1; i < strlen(options); i++) {
        switch (options[i]) {
            case 'n':
                printf("Name: %s\n", dirname);
                break;
            case 'd':
                printf("Size: %ld bytes\n", dirStat.st_size);
                break;
            case 'a':
                printAccessRights(dirStat.st_mode);
                break;
            case 'c': {
                DIR* dir = opendir(dirname);
                if (dir == NULL) {
                    perror("opendir");
                return;
                }

                int count = 0;
                struct dirent* entry;
                while ((entry = readdir(dir)) != NULL) {
                    if (entry->d_type == DT_REG) {
                      if (strstr(entry->d_name, ".c") != NULL)
                            count++;
                    }
                }

                closedir(dir);
                printf("Total Number of Files with .c Extension: %d\n", count);
                break;
            }
            default:
                printf("Invalid option: %c\n", options[i]);
                return;
        }
    }
}

void calculateScore(char* filename) {
    int num_warnings = 0;
    int num_errors = 0;

    // Execute script to compile the file and get the number of warnings and errors
    char command[100];
    sprintf(command, "gcc -o /dev/null -Wall -Wextra -Werror %s 2>&1", filename);

    FILE* script_output = popen(command, "r");
    if (script_output == NULL) {
        printf("Failed to execute script.\n");
        exit(1);
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), script_output) != NULL) {
        if (strstr(buffer, "warning") != NULL) {
            num_warnings++;
        } else if (strstr(buffer, "error") != NULL) {
            num_errors++;
        }
    }

    pclose(script_output);

    // Calculate score based on warnings and errors
    int score;
    if (num_errors == 0 && num_warnings == 0) {
        score = 10;
    } else if (num_errors > 0) {
        score = 1;
    } else {
        int remaining_warnings = num_warnings > 10 ? 10 : num_warnings;
        score = 2 + (8 * (10 - remaining_warnings)) / 10;
    }

    // Write score to grades.txt file
    FILE* grades_file = fopen("grades.txt", "a");
    if (grades_file == NULL) {
        printf("Failed to open grades.txt file.\n");
        exit(1);
    }

    fprintf(grades_file, "%s: %d\n", filename, score);
    fclose(grades_file);
}

void handleArgument(char* arg) {
    struct stat fileStat;
    if (stat(arg, &fileStat) < 0) {
        perror("stat");
        return;
    }

    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("fork");
        return;
    } else if (child_pid == 0) {
        // Child process

        if (S_ISREG(fileStat.st_mode)) {
            // Regular file
            printf("File Type: Regular File\n");
            printf("--MENU--\n");
            printf("Options:\n");
            printf("n:name\n");
            printf("d:size\n");
            printf("h:hardlink count\n");
            printf("m:last modified\n");
            printf("a:access rights\n");
            printf("l:create symbolic link\n");
            printf("----------------------------------------------------------------------------------\n");
            printf("Enter the desired options for the directory: ");
            char options[100];
            printf("Enter the desired options for the regular file: ");
            scanf("%s", options);
            regularFileOptions(arg, options);
            if (strstr(arg, ".c") != NULL) {
                calculateScore(arg); 
            } else {
                // Count the number of lines in the file
                FILE* file = fopen(arg, "r");
                if (file == NULL) {
                    printf("Failed to open file: %s\n", arg);
                    exit(1);
                }

                int line_count = 0;
                char buffer[256];
                while (fgets(buffer, sizeof(buffer), file) != NULL) {
                    line_count++;
                }

                fclose(file);
                printf("Number of lines in file %s: %d\n", arg, line_count);
            }
        } else if (S_ISLNK(fileStat.st_mode)) {
            // Symbolic link
            // Symbolic link
            printf("File Type: Symbolic Link\n");
            char options[100];
            printf("--MENU--\n");
            printf("Options:\n");
            printf("n:name\n");
            printf("l:delete symbolic link\n");
            printf("d:size of symbolic link\n");
            printf("t:size of target file\n");
            printf("a:access rights\n");
            printf("----------------------------------------------------------------------------------\n");
            printf("Enter the desired options for the directory: ");
            printf("Enter the desired options for the symbolic link: ");
            scanf("%s", options);
            symbolicLinkOptions(arg, options);
            // Change permissions of the symbolic link
            if (chmod(arg, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP) < 0) {
                perror("chmod");
                exit(1);
            }
        } else if (S_ISDIR(fileStat.st_mode)) {
            // Directory
            printf("File Type: Directory\n");
            char options[100];
            printf("--MENU--\n");
            printf("Options:\n");
            printf("n:name\n");
            printf("a:access rights\n");
            printf("c:total number of files with .c extenstions\n");
            printf("----------------------------------------------------------------------------------\n");
            printf("Enter the desired options for the directory: ");
            scanf("%s", options);
            directoryOptions(arg, options);
            // Execute command to create a file with .txt extension
            char txt_file_name[100];
            sprintf(txt_file_name, "%s/%s_file.txt", arg, arg);

            if (system(NULL) == 0) {
                printf("Command processor is not available.\n");
                exit(1);
            }

            char command[100];
            sprintf(command, "touch %s", txt_file_name);
            if (system(command) != 0) {
                printf("Failed to create .txt file.\n");
                exit(1);
            }
        }

        // Exit the child process with a success status
        exit(0);
    } else {
        // Parent process
        int status;
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("The process with PID %d has ended with exit code %d.\n", child_pid, exit_code);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf("No arguments provided.\n");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        pid_t child_pid = fork();

        if (child_pid < 0) {
            perror("fork");
            return 1;
        } else if (child_pid == 0) {
            // Child process
            handleArgument(argv[i]);
            exit(0);
        }
    }

    // Wait for all child processes to complete
    for (int i = 1; i < argc; i++) {
        wait(NULL);
    }

    return 0;
}

