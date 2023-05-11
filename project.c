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
                printf("Time of Last Modification: %s", ctime(&fileStat.st_mtime));
                break;
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

int main(int argc, char* argv[]){
    if(argc<=1){
        printf("No arguments provided.\n");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        struct stat fileStat;
        if (stat(argv[i], &fileStat) < 0) {
            perror("stat");
            continue;
        }

        printf("File: %s\n", argv[i]);
        
        if (S_ISREG(fileStat.st_mode)) { //REGULAR FILE
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
            regularFileOptions(argv[i], options);
        } 
        else if (S_ISLNK(fileStat.st_mode)) { //SYMBOLIC LINK
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
            symbolicLinkOptions(argv[i], options);
        } 
        else if (S_ISDIR(fileStat.st_mode)) { //DIRECTORY
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
            directoryOptions(argv[i], options);
        }
    printf("\n");
    return 0;
    }
}