#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int trim(char *text) {
    // Prve medzery
    size_t count  = 0;
    while(*(text + count) == ' ') {
        ++count;
    }
    memmove(text, text + count, strlen(text + count) + 1);
    // ***************************
    // Posledne medzery
    size_t length = strlen(text);
    char *end = text + length;
    count = 0;
    while (length > 0 && *(end - count - 1) == ' ') {
        ++count;
    }
    memmove(end - count, end, sizeof(char));
    //*********************************
    // komprimuje medzery v strede (rozsireny kod -> neni uz trim)
    // Staci zakomentovat ked to netreba.
    count = 0;
    size_t index = 0;
    while(*(text + index) != '\0') {
        if (*(text + index) == ' ' && *(text + index + 1) == ' ') {
            count = 0;
            while (text[index + count] == ' ') {
                ++count;
            }
            memmove(text + index + 1, text + index + count, strlen(text + index + count) + 1);
        }
        ++index;
    }
    return 0;
}

int main(void) {
    char c = 0;
    size_t chunkSize = 10;

    size_t capacity = 10 * sizeof(char);
    char *text = (char *)calloc(capacity, sizeof(char));

    if (!text) {
        printf("Allocation failed\n");
        return 1;
    }

    size_t count = 0;
    while((c = getchar()) != EOF && c != '\n') {
        ++count;

        if (count >= capacity) {
            capacity += chunkSize;
            char *tmp = realloc(text, capacity);
            if (!tmp) {
                printf("Allocation failed\n");
                free(text);
                return 1;
            }
            text = tmp;
        }

        text[count - 1] = c;
    }
    text[count] = '\0';
    printf("Before trim: %s|\n", text);
    printf("%lu\n", strlen(text));
    if (trim(text)) {
        return 1;
    };
    printf("After trim: |%s|\n", text);
    free(text);
    return 0;
}