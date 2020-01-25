#include <math.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>
#include <unistd.h>
#include <string.h>

int* state; 
int* philosopher_ids; 
int max_thinking_time;
int min_thinking_time;
int min_eating_time;
int max_eating_time;
int eat_num;
char* distribution_mode;

int* hungry_times;

sem_t mutex; 
sem_t* philosopher_semaphore;
  
void check_chopstick(int philosopher_index, int philosopher_number) 
{ 
    int LEFT = (philosopher_index + philosopher_number -1) % philosopher_number;
    int RIGHT = (philosopher_index + 1) % philosopher_number;
    if (state[philosopher_index] == 1 
        && state[LEFT] != 2 
        && state[RIGHT] != 2) { 
        state[philosopher_index] = 2; 
        sem_post(&philosopher_semaphore[philosopher_index]); 
    } 
} 
  
void take_chopstick(int philosopher_index, int philosopher_number) 
{  
    sem_wait(&mutex); 
  
    state[philosopher_index] = 1; 
  
    check_chopstick(philosopher_index, philosopher_number); 
  
    sem_post(&mutex); 
 
    sem_wait(&philosopher_semaphore[philosopher_index]); 

} 
  
void down_chopstick(int philosopher_index, int philosopher_number, int thinking_time) 
{ 
    printf("PHILOSOPHER %d FINISHED EATING\n", philosopher_index + 1); 

    int LEFT = (philosopher_index + philosopher_number -1) % philosopher_number;
    int RIGHT = (philosopher_index + 1) % philosopher_number;
  
    sem_wait(&mutex); 

    state[philosopher_index] = 0; 
  
    printf("PHILOSOPHER %d WAS THINKING FOR %d ms\n", philosopher_index + 1, thinking_time); 
  
    check_chopstick(LEFT, philosopher_number); 
    check_chopstick(RIGHT, philosopher_number); 
  
    sem_post(&mutex); 
} 
  
void* philospher(void* args) 
{ 
  
    for(int i = 0; i< eat_num; i++) { 
        int* arr = args; 
        int philosopher_number = arr[1];
        int thinking_time = 0;
        if(strcmp(distribution_mode, "uniform")){
            if(max_thinking_time != min_thinking_time)
            thinking_time = ((rand() % (max_thinking_time - min_thinking_time)) + min_thinking_time); 
            else
            thinking_time = max_thinking_time;
            printf("PHILOSOPHER %d WAS THINKING FOR %d ms\n", arr[0]+1, thinking_time);  
            usleep(thinking_time * 1000);
        }else{
            if(max_thinking_time != min_thinking_time)
            thinking_time = (sqrt((rand() % ((max_thinking_time*max_thinking_time) - (min_thinking_time*min_thinking_time)) + min_thinking_time*min_thinking_time)));
            else
            thinking_time = max_thinking_time;
            printf("PHILOSOPHER %d WAS THINKING FOR %d ms\n", arr[0]+1, thinking_time);  
            usleep(thinking_time * 1000);
        }

        clock_t start_seeking = clock();

        take_chopstick(*arr, philosopher_number); 

        clock_t difference_clock = clock() - start_seeking;
        int difference = (int) (double) difference_clock * 1000 / (CLOCKS_PER_SEC/1000);
        printf("PHILOSOPHER %d WAS HUNGRY FOR %d ms\n", arr[0]+1, difference); 
        hungry_times[arr[0]] += difference;
        
        if(strcmp(distribution_mode, "uniform")){
            int eating_time;
            if(max_eating_time != min_eating_time)
            eating_time = ((rand() % (max_eating_time - min_eating_time)) + min_eating_time); 
            else
            eating_time = max_eating_time;
            printf("PHILOSOPHER %d WAS EATING FOR %d ms\n", arr[0]+1, eating_time);  
            usleep(eating_time * 1000);
        }else{
            int eating_time;
            if(max_eating_time != min_eating_time)
            eating_time = (sqrt((rand() % ((max_eating_time*max_eating_time) - (min_eating_time*min_eating_time)) + min_eating_time*min_eating_time)));
            else
            eating_time = max_eating_time;
            printf("PHILOSOPHER %d WAS EATING FOR %d ms\n", arr[0]+1, eating_time);  
            usleep(eating_time * 1000);
        }
        
        down_chopstick(*arr, philosopher_number, thinking_time); 
    } 
} 
  
int main(int argc, char **argv) 
{ 
    srand(time(NULL));
    int philosopher_number = atoi(argv[1]);
    min_thinking_time = atoi(argv[2]);
    max_thinking_time = atoi(argv[3]);
    min_eating_time = atoi(argv[4]);
    max_eating_time = atoi(argv[5]);
    distribution_mode = argv[6]; 
    eat_num = atoi(argv[7]);

    pthread_t* thread_id = malloc(philosopher_number * sizeof(pthread_t));
    philosopher_semaphore = malloc(philosopher_number * sizeof(sem_t));
    philosopher_ids = malloc(philosopher_number * sizeof(int));
    hungry_times = malloc(philosopher_number * sizeof(int));
    state = malloc(philosopher_number * sizeof(int));
    sem_init(&mutex, 0, 1); 
    
    for (int i=0; i<philosopher_number; i++){
        philosopher_ids[i] = i;
        sem_init(&philosopher_semaphore[i], 0, 0); 
    }

    for (int i=0; i<philosopher_number; i++) { 
        int* args = malloc(2*sizeof(int));
        args[0] = philosopher_ids[i];
        args[1] = philosopher_number;
        pthread_create(&thread_id[i], NULL, 
                       philospher, args); 
    } 
  
    for (int i=0; i<philosopher_number; i++){
        pthread_join(thread_id[i], NULL); 
    }
    int total_hungry_state = 0;
    for(int i=0; i<philosopher_number; i++){
    printf("TOTAL HUNGRY TIME FOR PHILOSOPHER %d IS                    :%d ms\n", i+1, hungry_times[i]);
    printf("AVARAGE HUNGRY TIME FOR PHILOSOPHER %d IS                  :%f ms\n", i+1, (double)hungry_times[i]/eat_num);
        total_hungry_state += hungry_times[i] / eat_num;
    }
    double mean = (double) total_hungry_state / philosopher_number;
    printf("AVERAGE HUNGRY STATE FOR ALL PHILOSOPHERS IS               :%f ms\n", mean);
    double std_dev = 0;
    for (int i=0; i<philosopher_number; i++){
        std_dev += pow(hungry_times[i]/eat_num - mean, 2);
    }
    std_dev = sqrt(std_dev / philosopher_number);
    printf("STANDARD DEVIATION OF HUNGRY STATE FOR ALL PHILOSOPHERS IS :%f ms\n", std_dev);
}