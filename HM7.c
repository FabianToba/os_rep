#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>


void menu(char dirname[]){
    char option[10];
    printf("--MENU--\n");
    printf("Options:\n");
    printf("n:name\n");
    printf("a:access\n");
    printf("c:total number of files with .c extenstions\n");
    printf("----------------------------------------------------------------------------------\n");
    printf("Please enter your option:-");
    scanf("%s",&option);
    printf("\n");
    options(dirname,option);
}

void options(char dirname[],char option[])
{
    struct stat *buf;
    buf=malloc(sizeof(struct stat));

    if(stat(dirname,buf)<0)
    {
        printf("Error!Cannot read file permissions!\n");
        exit(1);
    }

    for(int i=0;i<strlen(option);i++)
    {
        switch(option[i])
        {
            case 'n':
            printf("The name is:%s\n",dirname);
            break;

            case 'a':
            permissions(buf->st_mode);
            break;

            case 'c':
            if(S_ISDIR(buf->st_mode))
		    {
		        DIR *tmp2 = opendir(dirname);
		        if(tmp2 == NULL)
		        {
			        printf("Error: opendir!\n");
			        exit(6);
		        }

		        int contr = 0;
		        char *c;
		        struct dirent *parse;
		        parse = readdir(tmp2);
		        while(parse != NULL)
		        {
			        if(parse->d_type == DT_REG)
			        {
			            c = strrchr(parse->d_name, '.');
			            if((c != NULL) && (strcmp(c, ".c") == 0))
			            {
				            contr++;
			            }
			        }
			        parse = readdir(tmp2);
		        }

		        if(closedir(tmp2) == -1)
		        {
			    printf("Error: closedir!\n");
			    exit(7);
		        }

		        printf("Total number of files with the .c extension: %d\n", contr);
		    }
		    else
		    {
		        printf("Not available!\n");
		    }
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
    printf("OTHER:\n");
    printf( (mode & S_IROTH) ? "Read:yes\n" : "Read:no\n");
    printf( (mode & S_IWOTH) ? "Write:yes\n" : "Write:no\n");
    printf( (mode & S_IXOTH) ? "Execution:yes\n" : "Execution:no\n");
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