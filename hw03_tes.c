//#include "osc.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


// Constants for OSC typetag
#define OSC_TT_INT 'i'
#define OSC_TT_STRING 's'
#define OSC_TT_FLOAT 'f'
#define OSC_TT_TIMETAG 't'

#define OSC_TYPETAG(...) {',', __VA_ARGS__, '\0'}



struct osc_timetag {
	uint32_t sec;
	uint32_t frac;
};

struct osc_message {
	char *address;
	char *typetag;
	char *raw_data;
};

union osc_msg_argument {
	const char *s;
	const uint32_t i;
	const float f;
	const struct osc_timetag t;
};

void OSC_TIMETAG_IMMEDIATE(struct osc_timetag *timetag) {
	timetag->sec = 0;
	timetag->frac = 1;
}

void OSC_TIMETAG_NULL(struct osc_timetag *timetag) {
	timetag->sec = 0;
	timetag->frac = 0;
}

void OSC_MESSAGE_NULL(struct osc_message *message) {
	message->address = NULL;
	message->typetag = NULL;
	message->raw_data = NULL;
}

int osc_message_new(struct osc_message *msg) {
  char* pData = (char*)calloc(12, sizeof(char));

  if (!pData) {
    return 1;
  }
  *(pData + 8) = ',';
  msg->raw_data = pData;
  msg->address = msg->raw_data + 4;
  msg->typetag = msg->raw_data + 8;

  return 0;
}

int main(void) {
	return 0;
}


