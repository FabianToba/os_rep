#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>


    void menu(char filename[]){
        char option[10];
        printf("--MENU--\n");
        printf("Options:\n");
        printf("n:name\n");
        printf("m:last modified\n");
        printf("a:access\n");
        printf("----------------------------------------------------------------------------------\n");
        printf("Please enter your option:-");
        scanf("%s",&option);
        printf("\n");
        options(filename,option);
    }

    void options(char filename[],char option[])
    {
        struct stat *buf;
        buf=malloc(sizeof(struct stat));

                if(stat(filename,buf)<0)
                {
                    printf("Error!Cannot read file permissions!\n");
                    exit(1);
                }

        for(int i=0;i<strlen(option);i++)
        {
            switch(option[i])
            {
                case 'n':
                printf("The name is:%s\n",filename);
                break;

                case 'a':
                permissions(buf->st_mode);
                break;

                case 'm':
                printf("Last time the file was modified:%s\n",ctime(&buf->st_mtime));
                break;

                case 'h':
                printf("The file named %s has %d hard links\n",filename,buf->st_nlink-1);
                break;

                case 'l':
                create_SL(filename);
                break;

                default:
                printf("invalid input!\n");
            }   
            printf("----------------------------------------------------------------------------------\n");
        }
    }

    void permissions(unsigned short mode)
    {
        printf("Access rights: \n");
              printf("USER:\n");
              printf( (mode & S_IRUSR) ? "Read:yes\n" : "Read:no\n");
              printf( (mode & S_IWUSR) ? "Write:yes\n" : "Write:no\n");
              printf( (mode & S_IXUSR) ? "Execution:yes\n" : "Execution:no\n");
              printf("GROUP:\n");
              printf( (mode & S_IRGRP) ? "Read:yes\n" : "Read:no\n");
              printf( (mode & S_IWGRP) ? "Write:yes\n" : "Write:no\n");
              printf( (mode & S_IXGRP) ? "Execution:yes\n" : "Execution:no\n");
              printf("OTHER:");
              printf( (mode & S_IROTH) ? "Read:yes\n" : "Read:no\n");
              printf( (mode & S_IWOTH) ? "Write:yes\n" : "Write:no\n");
              printf( (mode & S_IXOTH) ? "Execution:yes\n" : "Execution:no\n");
    }

    void create_SL(char filename[50])
    {
        char SL_name[50];
        printf("Choose a softlink file name:");
        scanf("%s",&SL_name);
        int Sl_test=symlink(filename,SL_name);
        if(Sl_test == 0)
        {
            printf("Soft link added!\n");
        }
    }


int main(int argc,char *argv[])
    {
        if(argc < 2 )
        {
            printf("Too few arguments provided!\n");
        }
        else
        {
            menu(argv[1]);
        }
        return 0;
    }