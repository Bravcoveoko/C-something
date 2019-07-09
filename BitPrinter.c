#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define CHAR_BYTE 0x80
#define INT_BYTE 0x10

// Toto vypisuje 8 bitov
// Pre ASCII hodnoty a cisla ktore sa zmestia do 1 bajtu :D 
void print_bits_seconf(unsigned int x) {
	short int result;
	for (size_t i = 0; i < 8; ++i) {
    	result = (x & (0x80 >> i)) ? 1 : 0;
    	printf("%d", result);
    }
  	printf("\n");
}

// Toto vypisuje 32 bitov
void print_bits_first(unsigned int x)
{
  short int result;
  for(int i = 8 * sizeof(x) - 1; i >= 0; i--) {
    result = (x & (1 << i)) ? 1 : 0;
    printf("%d", result);
  }
    printf("\n");
}

int main(void) {
	char *word = "AHOJ";
	size_t len = strlen(word);
	printf("PRVE:\n");
	for (size_t i = 0; i < len; ++i) {
		print_bits_seconf(*(word + i));
	}
	printf("DRUHE\n");
	for (size_t j = 0; j < len; ++j) {
		print_bits_first(*(word + j));
	}
	return 0;
}