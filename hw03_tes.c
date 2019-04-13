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
  void *raw_data;
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
  void *pData = calloc(12, 1);

  if (!pData) {
    return 1;
  }

  int *pLength = pData;
  *pLength = 8;

  char *pAddress = pData + 4;

  char *pTyptag = pData + 8;
  *pTyptag = ',';
  
  msg->raw_data = pData;
  msg->address = pAddress;
  msg->typetag = pTyptag;

  return 0;
}

void osc_message_destroy(struct osc_message *msg) {
  free(msg->raw_data);
  OSC_MESSAGE_NULL(msg);
}

size_t getLength(const char *address) {
  size_t len = strlen(address);
  size_t rest = len % 4;
  return len + (4 - rest);
}

int osc_message_set_address(struct osc_message *msg, const char *address) {
  size_t msgLen = msg->typetag - msg->address;
  size_t adrLen = getLength(address);
  size_t typetagLen = getLength(msg->typetag);

  if (msgLen != adrLen) {
    void *pData = realloc(msg->raw_data, (adrLen + typetagLen + 4));
    if (!pData) {
      return 1;
    }

    char *newAddress = pData + 4;
    memcpy(newAddress, address, adrLen);
    msg->address = newAddress;

    char *newTypeTag = pData + (4 + adrLen);
    memcpy(newTypeTag, msg->typetag, typetagLen);
    msg->typetag = newTypeTag;

    int *newPLength = pData;
    *newPLength = (adrLen + typetagLen);
    msg->raw_data = newPLength;

  } else {
    memcpy(msg->address, address, adrLen);
  }

  return 0;

}

int main(void) {
  return 0;
}



