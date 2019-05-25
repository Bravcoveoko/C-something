#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
// Nazov suboru bez cisiel
#define TEXT_LENGTH 5


int main(void) {
	// subor001, subor002, ...subor999
	char *suborText = "subor";
	char *result = calloc(TEXT_LENGTH + 4, sizeof(char));
  strcpy(result, suborText);
  // zaciname : subor00
  result[5] = '0';
  result[6] = '0';
  for (int i = 0; i < 1000; ++i) {
    result[7] = (i % 10) + '0';
    if (i % 10 == 0 && i > 0) {
      result[6] = (((result[6] - '0') + 1) % 10) + '0';
    }
    if (i % 100 == 0 && i > 0) {
      result[5] = (((result[5] - '0') + 1) % 10) + '0';
    }
    printf("%s\n", result);
  }

}