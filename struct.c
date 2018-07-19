/*EOF, FILE, fclose, fread*/
#include <stdio.h>
/* true/false*/
#include <stdbool.h>
/* Following are the functions defined in the header string.h
 * memchr, memcmp, memcpy, memmove, strcat*/
#include <string.h>
/* The stdlib.h header defines four variable types, several macros,
 * and various functions for performing general functions. ( size_t, Macros: NULL, EXIT_FAILURE ... */
#include <stdlib.h>

struct Person{
    char Id[30];
    char name[100];
    char lastName[100];
    char birthDate[20];
    int age;
    char **friends;
    int count_friends;
};

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

char **parsing(char *arr, char pattern){
    // Pole pointrov
    char **buffer = (char**) malloc(sizeof(char*));
    // dlzka zadaneho riadku
    size_t length = strlen(arr);
    int count = 0;
    buffer[count] = arr;
    for (size_t i = 0; i < length; i++){
        if (arr[i] == pattern){
            count++;
            // Na indexe patternu dam '\0' a tym sa mi cely riadok rozdeli na sub-stringy.
            arr[i] = '\0';
            char **pOld = buffer;
            // Naalokujem si dalsie miesto pre dalsi pointer.
            buffer = (char **) realloc(buffer, (count+1)*sizeof(char*));
            if (!buffer){
                free(pOld);
                return NULL;
            }
            // Priradim pointer
            buffer[count] = (arr + i + 1);
        }
    }
    return buffer;
    /*vypis:
    for (int i = 0; i <= count; i++){
        printf("%s\n", *(buffer + i));
    }*/
}

int main(int argc, char *argv[])
{
    if (argc != 2){
        printf("No parameter or too many parameters\n");
        return EXIT_FAILURE;
    }
    FILE *file = fopen(argv[1], "r");
    if (!file){
        printf("No file\n");
        return EXIT_FAILURE;
    }
    // Jeden riadok suboru
    char *line;
    // Pole ktore obsahuje rozparsovane stringy
    char **buff;
    char **ln;
    char **parr;
    // Pocet riadkov v subore
    int len;
    char *occurence;
    // Pocet osob
    int num_per = 0;
    struct Person **arr = malloc(sizeof(struct Person*));

    len = lineCount(file);
    fseek(file, 0, SEEK_SET);

    for (int i = 0; i < len; i++){
        line = readFullLine(file);
        if (!line){
            printf("NULL\n");
            return EXIT_FAILURE;
        }
        /********************************/
        buff = parsing(line, ':');
        if (!buff){
            return EXIT_FAILURE;
        }
        occurence = strchr(buff[0], '.');
        if (!occurence){
            //printf("NIC\n");
            arr[num_per] = malloc(sizeof(struct Person));
            if (!arr[num_per]){
                free(arr);
                return EXIT_FAILURE;
            }
            strcpy(arr[num_per]->Id, buff[0]);
            arr[num_per]->friends = (char**)malloc(sizeof(char*));
            arr[num_per]->count_friends = 1;
            ln = parsing(buff[1], ' ');
            strcpy(arr[num_per]->name, ln[0]);
            strcpy(arr[num_per]->lastName, ln[1]);
            free(ln);
            struct Person **pOld = arr;
            num_per++;
            arr = (struct Person **)realloc(arr, (num_per + 1)*sizeof(struct Person*));
            if (!arr){
                free(pOld);
                return EXIT_FAILURE;
            }
            continue;
        }
        /****************Priradovanie datumov************************/
        // ID a atribut (birthDate)
        parr = parsing(buff[0], '.');
        if (strcmp(occurence + 1, "birthDate") == 0){
            //printf("ID: %s\n", parr[0]);
            for (int i = 0; i < num_per; i++){
                if (strcmp(parr[0], arr[i]->Id) == 0 ){
                    strcpy(arr[i]->birthDate, buff[1]);
                    break;
                }
            }
        }
        /************Priradovanie kamaratov**************************/
        if (strcmp(occurence + 1, "friend") == 0){
            for (int i = 0; i < num_per; i++){
                if (strcmp(parr[0], arr[i]->Id) == 0){
                    arr[i]->friends[i] = malloc(strlen(buff[1] + 1));
                    strcpy(arr[i]->friends[i], buff[1]);
                    (arr[i]->count_friends)++;
                    arr[i]->friends = (char**)realloc(arr[i]->friends, (arr[i]->count_friends+1)*sizeof(char*));
                }
            }
        }
        /************************************************************/
        for (int k = 0; k < num_per; k++){
            for (int o = 0; o <= arr[k]->count_friends; o++){
                printf("%s ", arr[k]->friends[o]);
            }
            printf("\n");
        }


        free(buff);
        free(parr);
        free(line);
    }

    struct Person ***p = &arr;

    for (int i = 0; i < num_per; i++){
        printf("ID: %s MENO: %s PRIEZVISKO: %s BR: %s\n",(*p)[i]->Id, (*p)[i]->name, (*p)[i]->lastName, (*p)[i]->birthDate);
    }




    fclose(file);
    return 0;
}

