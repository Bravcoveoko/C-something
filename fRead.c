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

#define SIZE 40

int main(int argc, char *argv[])
{

    char buffer[SIZE];

    FILE *file = fopen(argv[1], "r");

    if (!file){
        perror("No file");
        return EXIT_FAILURE;
    }

    size_t len;
    size_t sum = 0;

    while (len = fread(buffer, sizeof(char), SIZE, file) > 0){
        printf("%s\n", buffer);
        printf("%zu\n", len);
    }


    return 0;
}

