#include <unistd.h>          
#include <stdio.h>           
#include <stdlib.h>          
#include <pthread.h>         
#include <semaphore.h>       
#include <sys/time.h>
#include <assert.h>


#define MAX_WAIT_CHAIRS 4       
#define NUM_ASSISTANTS 2           
#define MY_CUSTOMERS 12         
sem_t customers;                 
sem_t assistants;                   
sem_t tooManyLock;                     
sem_t simulateLock;  
double test, testTime, finishTime, openTime;
int numberOfFreeSeats = MAX_WAIT_CHAIRS;   
int wait[MAX_WAIT_CHAIRS];           
int customerTracker = 0;                  
int assistantTracker = 0;
static int cid = 0;  

// Assignment Three Table Variables         
int arrivalTime[MY_CUSTOMERS]  =  {3,7,8,9,11,12,14,16,19,22,34,39};
int serviceTime[MY_CUSTOMERS]  =  {15,10,8,5,12,4,8,14,7,2,9,3};

// Track Indices for all of the timer intervals
int waitArray[MY_CUSTOMERS]    =  {0,0,0,0,0,0,0,0,0,0,0,0};
int finishArray[MY_CUSTOMERS]  =  {0,0,0,0,0,0,0,0,0,0,0,0};
int t[MY_CUSTOMERS]            =  {0,0,0,0,0,0,0,0,0,0,0,0};

// P3 API
void assistantThread(void *args);        
void customerThread(void *args);       
double GetTime();
void Spin(int howlong);

double GetTime() {
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double)t.tv_sec + (double)t.tv_usec/1e6;
}

void Spin(int howlong) {
    double t = GetTime();
    while ((GetTime() - t) < (double)howlong)
	; // Just waiting here
}
int main()
{   
    pthread_t assistant_t[NUM_ASSISTANTS];
    pthread_t customer[MY_CUSTOMERS];
    //A customer semaphore that is initially empty
    sem_init(&customers,0,0);
    //Assistants also is initially empty
    sem_init(&assistants,0,0);
    //My lock semaphore
    sem_init(&simulateLock,0,1);   
    //Resource protection semaphore
    sem_init(&tooManyLock,0,6);   
    openTime = GetTime();
    int rc = 0;

    //Let us open the service center
    for(int i = 0; i < NUM_ASSISTANTS; i++)  
       rc = pthread_create(&assistant_t[i],NULL,(void *)assistantThread,(void*)&i);

    //Allow customers into service center
    for(int i = 0; i < MY_CUSTOMERS; i++){   
       if(i == 0)usleep(arrivalTime[i] * 1000000);
       if(i > 0) usleep((arrivalTime[i] - arrivalTime[i-1]) * 1000000);
       rc = pthread_create(&customer[i],NULL,(void *)customerThread,(void*)&i);      
    }   

    //Wait until done
    for(int i= 0; i < NUM_ASSISTANTS; i++)        
        pthread_join(&assistant_t[i],NULL);

    //Wait until done
    for(int i = 0; i < MY_CUSTOMERS; i++)        
        pthread_join(customer[i],NULL); 
}
void customerThread(void *args)  
{   
    int customerArgs = *(int *) args;
    int tempA, tempB;   
    
    sem_wait(&simulateLock);  
    cid++;           
    testTime = GetTime() - openTime;
    t[cid -1] = testTime;
    printf("Time[%d]: Customer %d \t arrives\n", arrivalTime[cid - 1], cid);
    if(numberOfFreeSeats > 0){   
        sem_wait(&tooManyLock);
        numberOfFreeSeats -= 1;    
        assistantTracker = (assistantTracker + 1) % MAX_WAIT_CHAIRS;  // Sir, you can sit here
        tempA= assistantTracker;
        wait[tempA] = cid;
        /* Gentlemen's Chair Rules
           Disregard breaks and bother assistant
           Customer Sleep   
           Protect mah seat                   */
        sem_post(&simulateLock);                   
        sem_post(&assistants);              
        sem_wait(&customers);              
        sem_wait(&simulateLock);                  
          numberOfFreeSeats++;
          tempB = wait[tempB]; 
        sem_post(&simulateLock);  
    } 
    else{
       sem_post(&simulateLock);
       printf("Time[%d]: Customer %d \t \t \t \t  \t leaves\n", arrivalTime[cid - 1], cid);
         }
    pthread_exit(0);
}
void assistantThread(void *args)        
{     
    int tempC, trackCid;
    double t_adjust;
    char a[10] = "TRUE";

    //Super stylish infinite loop
    while(a[0] = 'T' || a[1] == 'R' || a[2] == 'U' || a[3] == 'E')
    {   
        
        sem_wait(&assistants);          
        sem_wait(&simulateLock); 

        customerTracker = (customerTracker + 1) % MAX_WAIT_CHAIRS;  
        tempC= customerTracker;
        trackCid = wait[tempC];  

        sem_post(&customers);        
        sem_post(&simulateLock);

        test = GetTime();
        t_adjust = test - openTime;
        waitArray[trackCid-1] = t_adjust;

        printf("Time[%d]: Customer %d \t \t starts \t \t \n",waitArray[trackCid-1], trackCid);

        Spin(serviceTime[trackCid-1]);
        sem_post(&tooManyLock);

        finishTime = GetTime();
        t_adjust = finishTime - openTime;
        finishArray[trackCid-1] = t_adjust;
        
        printf("Time[%d]: Customer %d \t \t \t \tdone \t \n",finishArray[trackCid-1], trackCid);     
    }
}

