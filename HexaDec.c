#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

char *readFullLine(FILE *file){
    char *buffer = (char*) malloc(sizeof(char));
    if (!buffer){
        return NULL;
    }
    int ch;
    int count = 0;
    while ((ch = fgetc(file)) != '\n' && ch != EOF){
        buffer[count] = ch;
        count++;
        char *tmp = (char *)realloc(buffer, count*sizeof(ch));
        if (!tmp){
            free(buffer);
            return NULL;
        }
        buffer = tmp;
    }
    buffer[count] = '\0';
    return buffer;
}

int lineCount(FILE *file){
    int count = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF){
        if (ch == '\n'){
            count++;
        }
    }
    return count;
}


void printChar(char *word){
    unsigned int a;
    sscanf(word, "%x", &a);
    printf("%c\n", a);
}


void readHexa(char *line){
    char word[3];
    word[2] = '\0';
    int count;
    size_t line_length = strlen(line);
    for (size_t i = 0; i < line_length; i++){
        if (line[i] != ' '){
            word[0] = line[i];
            if (line[i+1] != ' '){
                word[1] = line[i+1];
                i++;
                printChar(word);
                continue;
            }
            word[1] = '\0';
            i++;
            printChar(word);
        }

    }

}


int main(int argc, char* argv[])
{
    if (argc != 2){
        perror("Invalid input");
        return EXIT_FAILURE;
    }
    FILE *file = fopen(argv[1], "rb");
    if (!file){
        perror("No file");
        return EXIT_FAILURE;
    }
    char *line;
    int count;
    count = lineCount(file);
    // Seek_set nastavi pointer na zaciatok suboru
    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < count; i++){
        line = readFullLine(file);
        readHexa(line);
        //printf("%s\n", line);
    }



    printf("Hello World!\n");
    return 0;
}

