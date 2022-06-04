#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include "builtin.h" 

#define BUFFER_SIZE 1024

/*
    This file checks to see if the commands from user input are built into our program. 
    If we find none of these commands exist, we will then run the linux command
*/

//Simple prototypes that are implemented into our shell 
static void runCommand_exit(char** args, int argcp);
static void runCommand_cd(char** args, int argpcp);
static void runCommand_pwd(char** args, int argcp);
static void runCommand_ls(char** args, int argcp); 

//This method checks to see if the command given is built into our program. If so, we will run our command. 
int builtIn(char **args, int numArgs){

    int builtIn = 1; 

    if((strcmp(args[0], "pwd") == 0) && (numArgs == 1))
        runCommand_pwd(args, numArgs);

    else if((strcmp(args[0], "exit") == 0) && ((numArgs == 1) || (numArgs == 2)))
        runCommand_exit(args, numArgs);

    else if((strcmp(args[0], "cd") == 0) && (numArgs == 2))
        runCommand_cd(args, numArgs);

    else if((strcmp(args[0], "ls") == 0) && (numArgs == 1))
        runCommand_ls(args, numArgs);
    
    else
        builtIn = 0;

    return builtIn;
}

//Simple method used to exit the program. If a second command is entered, we will return that value. Else, simply exit the program. 
static void runCommand_exit(char **args, int argcp){

    if(args[1] != NULL){

        int value = atoi(args[1]);
        exit(value);
    }

    else{

        exit(EXIT_SUCCESS);
    }
}

//This method will run the cd command. 
static void runCommand_cd(char **args, int argcp){

    if(chdir(args[1]) < 0)
        printf("Error!\n");
}

//Prints the current working directory. 
static void runCommand_pwd(char **args, int argcp){

    char getDirectory[BUFFER_SIZE];
    
    if(getcwd(getDirectory, sizeof(getDirectory)) == NULL)
        printf("Error!\n");

    else
        printf("%s\n", getDirectory);
}

//Runs the ls command 
static void runCommand_ls(char **args, int argcp){

    struct dirent *getDirectory;
    DIR *directory; int index = 0;

    if((directory = opendir(".")) == NULL)
        printf("Error!\n");

    //If we find the directory is valid, we simply list four files before appending a newline 
    if(directory){
        
        while((getDirectory = readdir(directory)) != NULL){

            printf("%s\t", getDirectory -> d_name);

            if(index > 2){

                printf("\n");
                index = 0; 
            }

            else{

                index++;
            }
        }

        printf("\n");
        closedir(directory);
    }
}



