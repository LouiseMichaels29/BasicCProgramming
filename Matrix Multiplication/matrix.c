#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

#define  DIM_X  6
#define  DIM_Y  9
#define  DIM_Z  2

#define MAX_THREADS 7

struct matrixThreadData{

  int start;
  int work;
  int end;
  int threadID;
  int *A;
  int *B;
  int *C;
};

struct matrixThreadData threadData[MAX_THREADS];

void *threadFunction(void *arg) {
  
  struct matrixThreadData *threadInfo = (struct matrixThreadData *) arg;
  int start = threadInfo -> start;
  int end = threadInfo -> end;
  int *A = threadInfo -> A; 
  int *B = threadInfo -> B; 
  int *C = threadInfo -> C;  

  C += (threadInfo -> start);
  int j = (threadInfo -> start % DIM_Z); 
  for(int i = start; i < end; i++){
    
    for(int k = 0; k < DIM_Y; k++){ 

      int getValueA = (*(A + (k + ((i / DIM_Z) * DIM_Y)))); 
      int getValueB = (*(B + ((j % DIM_Z)  + k * DIM_Z)));
      *C += (getValueA * getValueB); 
    }
    
    j++;
    C++; 
  }

  pthread_exit(NULL);
}

void multiplyMatrices (int *A, int *B, int *C) { 

  pthread_t threads[MAX_THREADS]; 
  for(int i = 0; i < MAX_THREADS; i++){

    int work = (int) ((DIM_X * DIM_Z) / MAX_THREADS);
    threadData[i].start = (int) (i * work);
    threadData[i].end = (int) (threadData[i].start + work);
    threadData[i].A = A;
    threadData[i].B = B;
    threadData[i].C = C;

    if(i == (MAX_THREADS - 1))
      threadData[i].end = (DIM_X * DIM_Z);

    if(pthread_create(&threads[i], NULL, threadFunction, &threadData[i]) != 0)
      printf("Error while creating threads.\n");
  }

  for(int i = 0; i < MAX_THREADS; i++)
    if(pthread_join(threads[i], NULL) != 0)
      printf("pthread_join error!\n");
}

void printMatrix (int *inputMatrix, int x, int y) {

  for(int i = 0; i < x; i++){

    printf("Row %d: ", i);
    for(int j = 0; j < y; j++){

      printf("%d  ", *inputMatrix);
      inputMatrix++;
    }

    printf("\n");
  }

  printf("\n");
}

void generateMatrix (int *inputMatrix, int x, int y) {

  int size = x * y; srand(time(0)); 
  
  for(int i = 0; i < size; i++){

    int randomNumber = ((rand() % 100) + 1);
    inputMatrix[i] = randomNumber;
  }

  sleep(1);
}
  
int main (int argc, char ** argv) { 
  
  int *A, *B, *C; 
  A = (int *) malloc(DIM_X * DIM_Y * sizeof(int *));
  B = (int *) malloc(DIM_Y * DIM_Z * sizeof(int *));
  C = (int *) malloc(DIM_X * DIM_Z * sizeof(int *));

  double elapsedTime = 0.0;
  struct timeval start, end;
	if(gettimeofday(&start, NULL) != 0)
		printf("Error setting start time.\n");

  generateMatrix(A, DIM_X, DIM_Y); 
  generateMatrix(B, DIM_Y, DIM_Z);
  multiplyMatrices(A, B, C);

  printf ("-------------- orignal A matrix ------------------\n");
  printMatrix(A, DIM_X, DIM_Y);

  printf ("-------------- orignal B matrix ------------------\n");
  printMatrix(B, DIM_Y, DIM_Z);

  printf ("--------------  result C matrix ------------------\n");
  printMatrix(C, DIM_X, DIM_Z);
  
  if(gettimeofday(&end, NULL) != 0)
		printf("Error setting end time.\n");
	
  elapsedTime = (end.tv_sec - start.tv_sec) * 1e6;
	elapsedTime = (elapsedTime + (end.tv_usec - start.tv_usec)) * 1e-6;
  printf("Elapsed Time: %.3f\n", elapsedTime);

  free(A);
  free(B);
  free(C); 
  
  return 0;
}

