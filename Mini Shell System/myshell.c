#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include "argparse.h"
#include "builtin.h"

#define BUFFER_SIZE 1024

/*
    This file will run our shell system. The methods from builtin.c and argparse.c are also included. 
*/

void processLine(char *line);
ssize_t getInput(char **line, size_t *size);

int main(){

    //Keep running our program until the user chooses to exit 
    while(1){

        //Here we simply create a pointer to our buffer size. I went ahead and defined the buffer size at the top. (Since getline will require a pointer) 
        size_t buffer_size = BUFFER_SIZE;
        size_t *buffer_size_pointer = &buffer_size;
        char **line = malloc(sizeof(char) * buffer_size); 
        getInput(line, buffer_size_pointer); 
        processLine(*line); 
        free(line);
    }
}

//This method simply accepts user input 
ssize_t getInput(char **line, size_t *size){

    printf(">>");
    ssize_t len = getline(line, size, stdin);
    return len;
}

//This method processes user input and runs our commands 
void processLine(char *line){

    //If line is empty, simply return 
    if(strlen(line) <= 1){

        printf("Line is empty.\n");
        return;
    }

    //Parse arguments and return a string array. Then, check to see if command is built in 
    pid_t childProcess;
    int status, argCount;
    char **arguments = parseArgs(line, &argCount);
    status = builtIn(arguments, argCount);

    //If we find our command is not built in 
    if(status == 0){

        //We will create a child process to run our command
        childProcess = fork();

        //For our child process, replace the process image with a new process and run that command 
        if(childProcess == 0){
            if(execvp(arguments[0], arguments) < 0)
                printf("Invalid command.\n"); 

            exit(EXIT_SUCCESS); 
        }

        else
            wait(NULL); //Wait on child process 
    }

    //Then, we simply free our data 
    for(int i = 0; i < argCount; i++)
        free(arguments[i]);

  free(arguments);
}

