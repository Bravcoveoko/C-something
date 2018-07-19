#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void pr(void* file, void* file2){
    char *buffer = (char*) malloc(sizeof(char));
    if (!buffer){
        return NULL;
    }
    int ch;
    int count = 0;
    while ((ch = fgetc(file)) != EOF){
        buffer[count] = ch;
        count++;
        char *tmp = (char *)realloc(buffer, count*sizeof(ch));
        if (!tmp){
            free(buffer);
            return NULL;
        }
        buffer = tmp;
    }
    fprintf(file2, buffer);
    fprintf(file2, "DONE");
    //fprintf(file2, "\n");
    buffer[count] = '\0';
    free(buffer);
}

int main(int argc, char *argv[])
{
    if (argc == 3){
        FILE *file = fopen(argv[1], "rb");
        FILE *file2 = fopen(argv[2], "w");
        pr(file, file2);
        printf("KONEC\n");
        return EXIT_SUCCESS;
    }
    FILE *file = fopen(argv[1], "rb");
    if (!file){
        perror("Bad");
        return EXIT_FAILURE;
    }
    fclose(file);

    printf("Hello World!\n");
    return 0;
}

