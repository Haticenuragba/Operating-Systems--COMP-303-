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

int main(int argc, char *argv[])
{

    char *word_to_be_search = argv[1];
    char *number_of_inputs = argv[2];
    int number_of_inputs_int = atoi(number_of_inputs);
    char *output_file_name = argv[3 + atoi(number_of_inputs)];

    int fd[number_of_inputs_int][2], nbytes;
    pid_t childpid;
    char string[] = "Hello, world!\n";
    char readbuffer[1000000];

    FILE *output = fopen(output_file_name, "w");
    char *file_name_to_read;

    for (int i = 0; i < number_of_inputs_int; i++)
    {
        pipe(fd[i]);
        file_name_to_read = argv[3 + i];
        int pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(1);
        }

        if (pid == 0)
        {
            /* Child process closes up input side of pipe */
            close(fd[i][0]);

            char temp[200];
            char result[100000];
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
                    strcat(result, file_name_to_read);
                    strcat(result, ": ");
                    char *str;
                    int size = asprintf(&str, "%d: ", k);
                    strcat(str, temp);
                    strcat(result, str);
                }


                k++;
            }
            fclose(fp);

            write(fd[i][1], result, (strlen(result) + 1));
            exit(0);
        }
        else
        {
            close(fd[i][1]);

            
        }
    }
    for(int i=0; i<number_of_inputs_int; i++){
            nbytes = read(fd[i][0], readbuffer, sizeof(readbuffer));
            if(strlen(readbuffer) > 0)
                 fprintf(output, "%s", readbuffer);
    }
    
    return (0);
}