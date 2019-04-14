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
  int32_t sec;
  int32_t frac;
};

struct osc_message {
  char *address;
  char *typetag;
  void *raw_data;
};

union osc_msg_argument {
  const char *s;
  const int32_t i;
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
    void *oldPtr = msg->raw_data;
    void *pData = realloc(msg->raw_data, (adrLen + typetagLen + 4));
    if (!pData) {
      msg->raw_data = oldPtr;
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

char *addNewTypeTag(char *msgTypetag, char tag) {
  size_t typetagLen = strlen(msgTypetag);
  char *newTypeTag = ((typetagLen % 4) == 3) ? 
                      calloc(typetagLen + 5, sizeof(char)) : 
                      calloc(getLength(msgTypetag), sizeof(char));

  if (!newTypeTag) {
    return NULL;
  }

  memcpy(newTypeTag, msgTypetag, typetagLen);
  *(newTypeTag + typetagLen) = tag;
  return newTypeTag;
}

int osc_message_add_int32(struct osc_message *msg, int32_t data) {
  size_t adrLen = getLength(msg->address);
  // TOTO FREE
  void *oldPtr = msg->raw_data;
  char *newTypeTag = addNewTypeTag(msg->typetag, OSC_TT_INT);

  if (!newTypeTag) {
    msg->raw_data = oldPtr;
    return 1;
  }
  size_t newTtagLen = getLength(newTypeTag);
  void *pData = realloc(msg->raw_data, (newTtagLen + adrLen + sizeof(int) + sizeof(int32_t)));

  if (!pData) {
    msg->raw_data = oldPtr;
    return 1;
  }

  char *pAddress = pData + 4;
  msg->address = pAddress;

  char *pTypeTag = pData + 4 + adrLen;
  memcpy(pTypeTag, newTypeTag, newTtagLen);
  msg->typetag = pTypeTag;

  int32_t *addDataPointer = pData + 4 + adrLen + newTtagLen;
  *addDataPointer = data;
  
  int *pRawData = pData;
  *pRawData = (newTtagLen + adrLen + sizeof(data));
  msg->raw_data = pRawData;

  // Tu sa vykonava free.
  free(newTypeTag);

  return 0;

}

int osc_message_add_float(struct osc_message *msg, float data) {
  size_t adrLen = getLength(msg->address);
  // TOTO FREE
  void *oldPtr = msg->raw_data;
  char *newTypeTag = addNewTypeTag(msg->typetag, OSC_TT_FLOAT);

  if (!newTypeTag) {
    msg->raw_data = oldPtr;
    return 1;
  }
  size_t newTtagLen = getLength(newTypeTag);
  void *pData = realloc(msg->raw_data, (newTtagLen + adrLen + 8));

  if (!pData) {
    msg->raw_data = oldPtr;
    return 1;
  }

  char *pAddress = pData + 4;
  msg->address = pAddress;

  char *pTypeTag = pData + 4 + adrLen;
  memcpy(pTypeTag, newTypeTag, newTtagLen);
  msg->typetag = pTypeTag;

  float *addDataPointer = pData + 4 + adrLen + newTtagLen;
  *addDataPointer = data;
  
  int *pRawData = pData;
  *pRawData = (newTtagLen + adrLen + sizeof(data));
  msg->raw_data = pRawData;

  // Tu sa vykonava free.
  free(newTypeTag);

  return 0;
}

int osc_message_add_timetag(struct osc_message *msg, struct osc_timetag tag) {
  size_t adrLen = getLength(msg->address);
  // TOTO FREE
  void *oldPtr = msg->raw_data;
  char *newTypeTag = addNewTypeTag(msg->typetag, OSC_TT_TIMETAG);

  if (!newTypeTag) {
    msg->raw_data = oldPtr;
    return 1;
  }
  size_t newTtagLen = getLength(newTypeTag);
  void *pData = realloc(msg->raw_data, (newTtagLen + adrLen + sizeof(int) + sizeof(struct osc_timetag)));

  if (!pData) {
    msg->raw_data = oldPtr;
    return 1;
  }

  char *pAddress = pData + 4;
  msg->address = pAddress;

  char *pTypeTag = pData + 4 + adrLen;
  memcpy(pTypeTag, newTypeTag, newTtagLen);
  msg->typetag = pTypeTag;
  //mozno picovina
  struct osc_timetag *addTagPointer = pData + 4 + adrLen + newTtagLen;
  *addTagPointer = tag;
  
  int *pRawData = pData;
  *pRawData = (newTtagLen + adrLen + sizeof(tag));
  msg->raw_data = pRawData;

  // Tu sa vykonava free.
  free(newTypeTag);

  return 0;
}

int osc_message_add_string(struct osc_message *msg, const char *data) {
  size_t adrLen = getLength(msg->address);
  // TOTO FREE
  void *oldPtr = msg->raw_data;
  char *newTypeTag = addNewTypeTag(msg->typetag, OSC_TT_STRING);

  if (!newTypeTag) {
    msg->raw_data = oldPtr;
    return 1;
  }
  size_t newTtagLen = getLength(newTypeTag);
  // +1 for ending zero
  void *pData = realloc(msg->raw_data, (newTtagLen + adrLen + sizeof(int) + (strlen(data) + 1)*sizeof(char)));

  if (!pData) {
    msg->raw_data = oldPtr;
    return 1;
  }

  char *pAddress = pData + 4;
  msg->address = pAddress;

  char *pTypeTag = pData + 4 + adrLen;
  memcpy(pTypeTag, newTypeTag, newTtagLen);
  msg->typetag = pTypeTag;
  //mozno picovina
  char *addDataPointer = pData + 4 + adrLen + newTtagLen;
  memcpy(addDataPointer, data, strlen(data) + 1);
  
  int *pRawData = pData;
  *pRawData = (newTtagLen + adrLen + strlen(data) + 1);
  msg->raw_data = pRawData;

  // Tu sa vykonava free.
  free(newTypeTag);

  return 0;
}

int main(void) {
  return 0;
}