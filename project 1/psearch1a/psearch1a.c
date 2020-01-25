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
    char* output_file_name = argv[3 + atoi(number_of_inputs)];

  

    int pid;
    char *file_name_to_read;
    for (int i = 0; i < number_of_inputs_int; i++)
    {
        
        pid = fork();
        if (pid == 0)
        {
            
            file_name_to_read = argv[3 + i];
            char file[6];
            sprintf(file, "%d.txt", i);
            FILE *temp_output = fopen(file, "w");
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
                    fprintf(temp_output, "%s:%d:%s", file_name_to_read, k, temp);
                }

                k++;
            }
            fclose(fp);
            
            exit(0);
        }
        else if(pid > 0){
            int status;
            pid = wait(&status);
        }
        else{
            printf("Fork failed");
        }
    }
     FILE *output = fopen(output_file_name, "w");

    for (int i = 0; i < number_of_inputs_int; i++)
    {

        char file_name[2];
        sprintf(file_name, "%d", i);
        strcat(file_name, ".txt");
        
        FILE *input = fopen(file_name, "r");

        char c;

        while ((c = fgetc(input)) != EOF)
            fputc(c, output);

        fclose(input);
       
    }
    fclose(output);
    return 0;
}
