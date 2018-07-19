#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

/* Adov vzorovy priklad je zly. Sifrovany string pri slove CONGRATULATION je kratky oproti originalu.
 * a znak kodovany do cisla 17 by mal byt 'R' ale podla Adovho algoritmu to vychadza na 'Q'.
 * Tu su vstupy, kt. reprezentuju vzorovy vystup:
 * 13|25|24|17|28|11|30|6|22|11|30|19|25|24
 * 23|10
 * 16|3|19|15|24|14
 * 10|25|31
 * 14|19|14
 * 19|205
 * Ak korenovy znak je 'P' (15).*/

/*********************Read one line in File.**********************************/
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
/*************************************************************************/
/********************Number of lines in file.*****************************/
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

int strToint(char *str){
    /* "Pretypovanie"
     * Vyuzivam ASCII hodnotu cisla.*/
    int result = 0;
    for (size_t i = 0; i < strlen(str); i++){
        result = result * 10 + (str[i] - '0');
    }
    return result;
}

/***************************************************************************/
/*******************************Encoding************************************/
void encode(char **buffer, int count, int root_patt, char *letters){
    for (int i = 0; i <= count; i++){
        // Funkcia meni stringy na cisla.
        int num = strToint(buffer[i]);
        int index = num + root_patt;
        int mod = index % 25;
        // Samotne vypisovanie.
        if (index > 25){
            printf("%c", letters[mod-1]);
            continue;
        }
        printf("%c", letters[index - 1]);
    }
    printf("\n");
}
/***************************************************************************/
/********************************Parsing************************************/
void parsing_and_encoding(char *arr, char pattern, int root_patt, char *letters){
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
                return;
            }
            // Priradim pointer
            buffer[count] = (arr + i + 1);
        }
    }
    /*buffer pri takomto vstupe: 25|12|5 vyzera:
     * 25(\0)12(\0)5*/
    // Funkcia desifruje a vypisuje vysledok.
    encode(buffer, count, root_patt, letters);
}

int root(char *argv[], char *letters){
    // Keby prvy parameter bol dlhsi ako 1.
    if (strlen(argv[1]) > 2){
        printf("ZLE");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 26; i++){
        if (*argv[1] == letters[i]){
            return i;
        }
    }
    // Alebo keby sa ziadne pismeno nenaslo.
    printf("Invalid char\n");
    return EXIT_FAILURE;
}


int main(int argc, char *argv[])
{
    char letters[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                     'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    if (argc != 3){
        printf("No paramater or invalid input\n");
        return EXIT_FAILURE;
    }
    // Zistim si index korenoveho znaku podla ktoreho sa bude vstup desifrovat.
    int root_patt = root(argv, letters);
    if (root_patt == EXIT_FAILURE){
        return root_patt;
    }

    FILE *file = fopen(argv[2], "r");
    if (!file){
        printf("No file\n");
        return EXIT_FAILURE;
    }
    // Premenna pre 1 nacitany riadok zo suboru.
    char *line;
    // Pocet riadkov v subore.
    int len;
    len = lineCount(file);
    /* Funkcia lineCount mi nastavi pointer na koniec suboru.
     * preto ho musim vratit na zaciatok suboru.*/
    fseek(file, 0, SEEK_SET);
    for(int i = 0; i < len; i++){
        line = readFullLine(file);
        if (!line){
            printf("NULL\n");
            free(line);
            return EXIT_FAILURE;
        }
        // Funkcia rozparsuje a "pretypuje" vstup a zzaroven vypise vystup.
        parsing_and_encoding(line, '|', root_patt, letters);
        free(line);
    }
    fclose(file);
    return EXIT_SUCCESS;
}

