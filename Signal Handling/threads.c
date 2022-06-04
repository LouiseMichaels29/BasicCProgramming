#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sched.h>
#include <errno.h>

#define THREAD_COUNT        3
#define TASK_COUNT          3

//Initialize mutexes 
pthread_mutex_t             displayMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t             lockMutex = PTHREAD_MUTEX_INITIALIZER;

//Structure for threads 
typedef struct{

    int                     threadNumber;
    int                     signalNumber;
    int                     timerPeriod;
    int                     missedSignals;

    int64_t                 timeStampStart[TASK_COUNT];
    int64_t                 timeStampEnd[TASK_COUNT];

    pthread_t               threadMemory;
    sigset_t                threadSigset;

    timer_t                 timerMemory;

    time_t                  startTime;
    time_t                  endTime;

    struct sched_param      param;

}currentThread;

//Create an array for the maximum number of threads 
currentThread threadArray[THREAD_COUNT];

//Simply display the threads and the time taken to run 
void displayThreads(currentThread* threadInformation){

    pthread_mutex_lock(&displayMutex);

    for(int i = 0; i < TASK_COUNT; i++){

        printf("Thread [%d]  Delta[%"PRId64"]  Jitter[%"PRId64"]\n", threadInformation -> threadNumber, threadInformation -> timeStampEnd[i]
               - threadInformation -> timeStampStart[i], threadInformation -> timeStampEnd[i]
               - threadInformation -> timeStampStart[i] - threadInformation -> timerPeriod);
    }

    pthread_mutex_unlock(&displayMutex);
}

//Create and arm the timer for each thread 
void CreateAndArmTimer(unsigned int period, currentThread* threadInformation){

    //We start at the lowest signal possible. Signals will only affect the order in which we expect them to run (using sigprocmask)
    static int signalNumber = 35;
    threadInformation -> signalNumber = signalNumber;
    signalNumber++;

    //Empty set and add signals 
    if(sigemptyset(&threadInformation -> threadSigset) != 0) { printf("Signal Empty Set Error!\n"); }
    if(sigaddset(&threadInformation -> threadSigset, threadInformation -> signalNumber) != 0) { printf("Signal Add Set Error!\n"); }

    //Structure for signal events. Assign the timer to the thread structure and create timer 
    struct sigevent mySignalEvent;
    mySignalEvent.sigev_notify = SIGEV_SIGNAL;
    mySignalEvent.sigev_signo = threadInformation -> signalNumber;
    mySignalEvent.sigev_value.sival_ptr = (void *) (&threadInformation -> timerMemory);
    if(timer_create(CLOCK_MONOTONIC, &mySignalEvent, &threadInformation -> timerMemory) != 0){ printf("Timer Create Error!\n"); }

    //Here we set the variables for the timer period 
    struct itimerspec timerSpec;
    int seconds = period / 1000000;
    long nanoSeconds = ((period - (seconds * 1000000)) * 1000);
    timerSpec.it_interval.tv_sec = seconds;
    timerSpec.it_interval.tv_nsec = nanoSeconds;
    timerSpec.it_value.tv_sec = seconds;
    timerSpec.it_value.tv_nsec = nanoSeconds;

    //Assign the correct time for each thread 
    if(timer_settime(threadInformation -> timerMemory, 0, &timerSpec, NULL) != 0) { printf("Timer Set Time Error!\n"); }
}

//Simple method to wait for the correct signal for each thread. Also counts timer over runs (blocks until signal is received)
void waitPeriod(currentThread* threadInformation){

    if(sigwait(&threadInformation -> threadSigset, &threadInformation -> signalNumber) != 0) { printf("Sig Wait Error!\n"); }
    if((threadInformation -> missedSignals = timer_getoverrun(threadInformation -> timerMemory)) == -1){ printf("Timer Over Run Error!\n"); };
}

//Thread method assigned to our threads (when creating pthreads, we must have a thread function)
void* threadFunction(void* arg){

    //Lock our thread and cast thread structure from void argument 
    pthread_mutex_lock(&lockMutex);
    currentThread* threadInformation = (currentThread *) arg;

    struct timespec timeMilliSeconds;

    //Check thread arguments. Then, arm our timer right before we start our process. 
    if(threadInformation -> threadMemory != pthread_self()){ printf("Mismatched Thread IDs. Exiting...\n"); pthread_exit(arg); }
    if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &threadInformation -> param) != 0){ printf("Set Scheduling Paramaters Error.\n"); }
    CreateAndArmTimer(threadInformation -> timerPeriod, threadInformation);

    //When our process starts, we will record the actual time it takes for our process to run and the time assigned to each thread variable. 
    for(int i = 0; i < TASK_COUNT; i++){

        //Record time, set timestamp and block until signal is recieved. 
        clock_gettime(CLOCK_REALTIME, &timeMilliSeconds);
        threadInformation -> timeStampStart[i] = timeMilliSeconds.tv_sec * 1000000;
        threadInformation -> timeStampStart[i] += timeMilliSeconds.tv_nsec / 1000;
        if(timeMilliSeconds.tv_nsec % 1000 >= 500) threadInformation -> timeStampStart[i] += 1000;

        waitPeriod(threadInformation);

        //Get the ending time for the thread and record the total time taken to run 
        clock_gettime(CLOCK_REALTIME, &timeMilliSeconds);
        threadInformation -> timeStampEnd[i] = timeMilliSeconds.tv_sec * 1000000;
        threadInformation -> timeStampEnd[i] += timeMilliSeconds.tv_nsec / 1000;
        if(timeMilliSeconds.tv_nsec % 1000 >= 500) threadInformation -> timeStampEnd[i] += 1000;
    }

    //Display threads, unlock and exit 
    displayThreads(threadInformation);

    pthread_mutex_unlock(&lockMutex);
    pthread_exit(NULL);
}

int main(){

    //Priority value starts as lowest value. (This value is really not needed, but can be changed if two threads require different priorities)
    int priority = 1;
    sigset_t    signalSet;

    //Here we will empty our signal set, add all signals in the correct range and block (one block for each signal value)
    if(sigemptyset(&signalSet) != 0) { printf("Sig Empty Set Error!\n"); }
    for(int i = SIGRTMIN; i <= SIGRTMAX; i++){ if(sigaddset(&signalSet, i) != 0) { printf("Sig Add Set Error!\n"); }}
    if(sigprocmask(SIG_BLOCK, &signalSet, NULL) != 0){ printf("Sig Proc Mask Error!\n"); }

    //Create our threads here and assign values
    for(int i = 0; i < THREAD_COUNT; i++){

        threadArray[i].threadNumber = i;
        threadArray[i].param.sched_priority = priority++;
        threadArray[i].timerPeriod = (1 << i) * 1000000;
        if(pthread_create(&threadArray[i].threadMemory, NULL, threadFunction, &threadArray[i]) != 0) { printf("Pthread Create Error!\n"); }
    }

    //Here we simply join all threads before our program ends 
    for(int i = 0; i < THREAD_COUNT; i++){

        pthread_join(threadArray[i].threadMemory, NULL);
    }

    return 0;
}

