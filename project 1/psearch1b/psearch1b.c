#define _GNU_SOURCE
#include <semaphore.h>
#include <sys/mman.h>
#include <linux/mman.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

sem_t *semaphores;

int main(int argc, char *argv[])
{

    char *word_to_be_search = argv[1];
    char *number_of_inputs = argv[2];
    int number_of_inputs_int = atoi(number_of_inputs);
    char *output_file_name = argv[3 + atoi(number_of_inputs)];

    semaphores = mmap(NULL, (number_of_inputs_int + 1) * sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    FILE *output = fopen(output_file_name, "w");

    if (semaphores == MAP_FAILED)
    {
        printf("Map failed");
        exit(0);
    }

    int pid;
    char *file_name_to_read;
    char *child_result;

    for (int i = 0; i <= number_of_inputs_int; i++)
    {
        sem_init(&semaphores[i], 2, 0);
    }

    sem_post(&semaphores[0]);

    for (int i = 0; i < number_of_inputs_int; i++)
    {
        pid = fork();
        file_name_to_read = argv[3 + i];

        if (pid == 0)
        {
            sem_wait(&semaphores[i]);
            char temp[1000];
            FILE *fp = fopen(file_name_to_read, "r");
            int k = 1;
            while (!feof(fp))
            {
                fgets(temp, 100000, fp);
                char word[200];
                sprintf(word, " %s ", word_to_be_search);
                char line[1000];
                sprintf(line, " %s ", temp);
                if (strstr(line, word))
                {
                    fprintf(output, "%s:%d:%s", file_name_to_read, k, temp);
                }

                k++;
            }
            fclose(fp);

            sem_post(&semaphores[i + 1]);

            exit(0);
        }
        else if (pid < 0)
        {
            perror("fork failed");
            abort();
        }
    }

    sem_wait(&semaphores[number_of_inputs_int]);

    fclose(output);

    for (int i = 0; i <= number_of_inputs_int; i++)
    {
        sem_destroy(&semaphores[i]);
    }

    return 0;
}
