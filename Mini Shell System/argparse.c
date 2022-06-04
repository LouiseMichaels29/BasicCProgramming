#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "argparse.h"

/*
  This file will parse the arguments from user input
*/

//Here we simply count the number of arguments. 
static int countArgs(char *line){

  //Create a static array (since strtok cannot be used on string literals).
  //Clear memory, copy data, and then use strtok and strchr to find the first and second arguments. 
  char staticArray[strlen(line)];
  bzero(staticArray, sizeof(staticArray));
  memcpy(staticArray, line, strlen(line));
  char *firstCommand = strtok(staticArray, " "); 
  char *secondCommand = strchr(line, ' '); 

  //Then, we check if there is one or two arguments. 
  if(secondCommand == NULL)
    return 1;

  else
    return 2;
}

//This method will parse the arguments and return a 2D array of strings 
char **parseArgs(char *line, int *argcp){

  //Convert to static array and copy data from line. 
  char staticArray[strlen(line)];
  int numArgs = countArgs(line); *argcp = numArgs;
  char **stringArray = malloc(sizeof(char *) * (numArgs));
  bzero(staticArray, sizeof(staticArray));
  bzero(stringArray, sizeof(stringArray));
  memcpy(staticArray, line, strlen(line)); 

  //Same method as before. We find the first and second command entered by the user. 
  char *firstCommand = strtok(staticArray, " "); 
  char *secondCommand = strchr(line, ' '); 
  stringArray[0] = malloc(sizeof(char) * strlen(firstCommand));
  bzero(stringArray[0], sizeof(stringArray[0]));
  strcpy(stringArray[0], firstCommand); 

  //If we have only one argument, we will null terminate the newline character and return our array
  if(numArgs == 1){
    
    stringArray[0][strlen(firstCommand) - 1] = '\0';
  }

  //Else, copy the second command into the second index (minus the space) and null terminate the newline character. Then, we simply return our array. 
  else if(numArgs == 2){

    stringArray[1] = malloc(sizeof(char) * strlen(secondCommand + 1));
    bzero(stringArray[1], sizeof(stringArray[1]));
    strcpy(stringArray[1], (secondCommand + 1));
    stringArray[1][strlen(secondCommand) - 2] = '\0';
  }  

  return stringArray;
}



