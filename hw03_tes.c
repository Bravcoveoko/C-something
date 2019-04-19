#include "osc.h"
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

void OSC_BUNDLE_NULL(struct osc_bundle *bundle) {
  bundle->timetag = NULL;
  bundle->raw_data = NULL;
}

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

void osc_message_destroy(struct osc_message *msg) {
  free(msg->raw_data);
  OSC_MESSAGE_NULL(msg);
}

int osc_message_new(struct osc_message *msg) {
  void *pData = calloc(12, 1);

  if (!pData) {
    return 1;
  }

  int *pLength = pData;
  *pLength = 8;

  char *pAddress = (char *)pData + sizeof(uint32_t);

  char *pTyptag = (char*)pData + (2 * sizeof(uint32_t));
  *pTyptag = ',';
  
  msg->raw_data = pData;
  msg->address = pAddress;
  msg->typetag = pTyptag;

  return 0;
}

size_t getLength(const char *address) {
  size_t len = strlen(address);
  size_t rest = len % 4;
  return len + (4 - rest);
}
// TOTO BY UZ MALO BYT DOBRE
int osc_message_set_address(struct osc_message *msg, const char *address) {
  //Velkost msg->address
  size_t msgAdrLen = getLength(msg->address);
  // velkost zadanej adresy
  size_t addressLen = getLength(address);
  if (msgAdrLen == addressLen) {
    memcpy(msg->address, address, addressLen);
    return 0;
  }
  // Velkost raw data bez 4B
  int msgLen = *((int *)(msg->raw_data));
  // Velkost typetagu + zvysnych dat
  size_t ttDataLen = msgLen - msgAdrLen;
  // Nova velkost data + typetag + nova Adresa
  uint32_t newDataLength = addressLen + ttDataLen;
  void *pData = (void *)(calloc(newDataLength + sizeof(uint32_t), sizeof(char)));
  if (!pData) {
    return 1;
  }
  // Stary pointer = typetag + data
  char *oldPtr = (char *)(msg->raw_data) + sizeof(uint32_t) + msgAdrLen;
  // nova velkost na prvych 4B
  *((int*)pData) = newDataLength;
  // nova adresa
  char *newAddr = (char *)pData + sizeof(uint32_t);
  memcpy(newAddr, address, addressLen);
  // kopirovanie udajov typetag + data
  char *pTtData  = (char *)pData + sizeof(uint32_t) + addressLen;
  memcpy(pTtData, oldPtr, ttDataLen);

  free(msg->raw_data);
  msg->raw_data = pData;
  msg->address = newAddr;
  msg->typetag = pTtData;

  return 0;
}

// Vrati pointer na novu naalokovanu pamat ktora obsahuje
// povodny string z typetagu + tag
char *addNewTypeTag(char *msgTypetag, char tag) {
  size_t typetagLen = strlen(msgTypetag);
  // Kriticka situacia. Pri ',if\0' sa prida novy typetag
  // a musia sa podoplnat \0 nakonci inac len pridat novy tt.
  // Calloc zarucuje \0
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
  // Velkost adresy v msg
  size_t msgAdrLen = getLength(msg->address);
  // Velkost tt v msg
  size_t msgTtLen = getLength(msg->typetag);
  // pointer na data ktore sa nachadzaju za typetagom
  char *restData = (char *)msg->raw_data + msgAdrLen + msgTtLen + sizeof(int);
  // Velkost zvysnych dat
  int restDataLen = *(int*)msg->raw_data - msgAdrLen - msgTtLen; 
  // Toto sa bude muset uvolnit
  char *newTypeTag = addNewTypeTag(msg->typetag, OSC_TT_INT);

  if (!newTypeTag) {
    return 1;
  }

  // velkost noveho typetagu
  size_t newTtagLen = getLength(newTypeTag);
  // Nova dlzka ulozena na prvych 4B
  int newMsgLen = msgAdrLen + newTtagLen + restDataLen + sizeof(int32_t);
  // prve 4B + dlzka adresy + dlzka typetagu(noveho) + dlzka starych dat + dlzka noveho data ,kt. sa prida
  void *pData = realloc(msg->raw_data, (sizeof(uint32_t) + newMsgLen));

  if (!pData) {
    return 1;
  }
  // Stare data
  char *pRestData = (char *)pData + sizeof(uint32_t) + msgAdrLen + newTtagLen;
  memcpy(pRestData, restData, restDataLen);
  // Nove data
  int32_t *pNewData = (int32_t*)((char*)pData + sizeof(uint32_t) + msgAdrLen + newTtagLen + restDataLen);
  *pNewData = data;
  // Prve 4B (aktualizovana dlzka)
  int *pRawData = pData;
  *pRawData = newMsgLen;
  msg->raw_data = pRawData;
  // Adresa
  char *pAddress = (char *)pData + sizeof(uint32_t);
  memcpy(pAddress, msg->address, msgAdrLen);
  msg->address = pAddress;
  // TypeTag
  char *pTypetag = (char *)pData + sizeof(uint32_t) + msgAdrLen;
  memcpy(pTypetag, newTypeTag, newTtagLen);
  msg->typetag = pTypetag;
  // Uvolnit pamat ktora vznikla vo funkcii addNewTypeTag
  free(newTypeTag);
  return 0;
}

int osc_message_add_float(struct osc_message *msg, float data) {
  // Velkost adresy v msg
  size_t msgAdrLen = getLength(msg->address);
  // Velkost tt v msg
  size_t msgTtLen = getLength(msg->typetag);
  // pointer na data ktore sa nachadzaju za typetagom
  char *restData = (char *)msg->raw_data + msgAdrLen + msgTtLen + sizeof(int);
  // Velkost zvysnych dat
  int restDataLen = *(int*)msg->raw_data - msgAdrLen - msgTtLen; 
  // Toto sa bude muset uvolnit
  char *newTypeTag = addNewTypeTag(msg->typetag, OSC_TT_FLOAT);

  if (!newTypeTag) {
    return 1;
  }

  // velkost noveho typetagu
  size_t newTtagLen = getLength(newTypeTag);
  // Nova dlzka ulozena na prvych 4B
  int newMsgLen = msgAdrLen + newTtagLen + restDataLen + sizeof(float);
  // prve 4B + dlzka adresy + dlzka typetagu(noveho) + dlzka starych dat + dlzka noveho data ,kt. sa prida
  void *pData = realloc(msg->raw_data, (sizeof(uint32_t) + newMsgLen));

  if (!pData) {
    return 1;
  }
  // Stare data
  char *pRestData = (char*)pData + sizeof(uint32_t) + msgAdrLen + newTtagLen;
  memcpy(pRestData, restData, restDataLen);
  // Nove data
  float *pNewData = (float*)((char*)pData + sizeof(uint32_t) + msgAdrLen + newTtagLen + restDataLen);
  *pNewData = data;
  // Prve 4B (aktualizovana dlzka)
  int *pRawData = pData;
  *pRawData = newMsgLen;
  msg->raw_data = pRawData;
  // Adresa
  char *pAddress = (char*)pData + sizeof(uint32_t);
  memcpy(pAddress, msg->address, msgAdrLen);
  msg->address = pAddress;
  // TypeTag
  char *pTypetag = (char*)pData + sizeof(uint32_t) + msgAdrLen;
  memcpy(pTypetag, newTypeTag, newTtagLen);
  msg->typetag = pTypetag;
  // Uvolnit pamat ktora vznikla vo funkcii addNewTypeTag
  free(newTypeTag);
  return 0;
}

int osc_message_add_timetag(struct osc_message *msg, struct osc_timetag tag) {
  // Velkost adresy v msg
  size_t msgAdrLen = getLength(msg->address);
  // Velkost tt v msg
  size_t msgTtLen = getLength(msg->typetag);
  // pointer na data ktore sa nachadzaju za typetagom
  char *restData = (char *)msg->raw_data + msgAdrLen + msgTtLen + sizeof(int);
  // Velkost zvysnych dat
  int restDataLen = *(int*)msg->raw_data - msgAdrLen - msgTtLen; 
  // Toto sa bude muset uvolnit
  char *newTypeTag = addNewTypeTag(msg->typetag, OSC_TT_TIMETAG);

  if (!newTypeTag) {
    return 1;
  }

  // velkost noveho typetagu
  size_t newTtagLen = getLength(newTypeTag);
  // Nova dlzka ulozena na prvych 4B
  int newMsgLen = msgAdrLen + newTtagLen + restDataLen + sizeof(struct osc_timetag);
  // prve 4B + dlzka adresy + dlzka typetagu(noveho) + dlzka starych dat + dlzka noveho data ,kt. sa prida
  void *pData = realloc(msg->raw_data, (sizeof(uint32_t) + newMsgLen));

  if (!pData) {
    return 1;
  }

  // Stare data
  char *pRestData = (char*)pData + sizeof(uint32_t) + msgAdrLen + newTtagLen;
  memcpy(pRestData, restData, restDataLen);
  // Nove data
  struct osc_timetag *pNewData = (struct osc_timetag*)((char*)pData + sizeof(uint32_t) + msgAdrLen + newTtagLen + restDataLen);
  *pNewData = tag;
  // Prve 4B (aktualizovana dlzka)
  int *pRawData = pData;
  *pRawData = newMsgLen;
  msg->raw_data = pRawData;
  // Adresa
  char *pAddress = (char*)pData + sizeof(uint32_t);
  memcpy(pAddress, msg->address, msgAdrLen);
  msg->address = pAddress;
  // TypeTag
  char *pTypetag = (char*)pData + sizeof(uint32_t) + msgAdrLen;
  memcpy(pTypetag, newTypeTag, newTtagLen);
  msg->typetag = pTypetag;
  // Uvolnit pamat ktora vznikla vo funkcii addNewTypeTag
  free(newTypeTag);
  return 0;
}

int osc_message_add_string(struct osc_message *msg, const char *data) {
  // Velkost adresy v msg
  size_t msgAdrLen = getLength(msg->address);
  // Velkost tt v msg
  size_t msgTtLen = getLength(msg->typetag);
  // pointer na data ktore sa nachadzaju za typetagom
  char *restData = (char *)msg->raw_data + msgAdrLen + msgTtLen + sizeof(uint32_t);
  // Velkost zvysnych dat
  int restDataLen = *(int*)msg->raw_data - msgAdrLen - msgTtLen; 
  // Toto sa bude muset uvolnit
  char *newTypeTag = addNewTypeTag(msg->typetag, OSC_TT_STRING);

  if (!newTypeTag) {
    return 1;
  }

  // velkost noveho typetagu
  size_t newTtagLen = getLength(newTypeTag);
  // Nova dlzka ulozena na prvych 4B
  int newMsgLen = msgAdrLen + newTtagLen + restDataLen + strlen(data) + 1;
  // prve 4B + dlzka adresy + dlzka typetagu(noveho) + dlzka starych dat + dlzka noveho data ,kt. sa prida
  void *pData = realloc(msg->raw_data, (sizeof(uint32_t) + newMsgLen));

  if (!pData) {
    return 1;
  }

  // Stare data
  char *pRestData = (char*)pData + sizeof(uint32_t) + msgAdrLen + newTtagLen;
  memcpy(pRestData, restData, restDataLen);
  // Nove data
  char *pNewData = (char*)pData + sizeof(uint32_t) + msgAdrLen + newTtagLen + restDataLen;
  memcpy(pNewData, data, strlen(data) + 1);
  // Prve 4B (aktualizovana dlzka)
  int *pRawData = pData;
  *pRawData = newMsgLen;
  msg->raw_data = pRawData;
  // Adresa
  char *pAddress = (char*)pData + sizeof(uint32_t);
  memcpy(pAddress, msg->address, msgAdrLen);
  msg->address = pAddress;
  // TypeTag
  char *pTypetag = (char*)pData + sizeof(uint32_t) + msgAdrLen;
  memcpy(pTypetag, newTypeTag, newTtagLen);
  msg->typetag = pTypetag;
  // Uvolnit pamat ktora vznikla vo funkcii addNewTypeTag
  free(newTypeTag);
  return 0;
}

size_t osc_message_argc(const struct osc_message *msg) {
  return strlen(msg->typetag) - 1;
}

void *getArgument(char letter, void *tmpRD) {
  size_t length = 0;
  switch(letter) {
    case OSC_TT_INT:
      tmpRD = (int*)tmpRD + 1;
      return tmpRD;
    case OSC_TT_FLOAT:
      tmpRD = (float*)tmpRD + 1;
      return tmpRD;
    case OSC_TT_TIMETAG:
      tmpRD = (struct osc_timetag*)tmpRD + 1;
      return tmpRD;
    case OSC_TT_STRING:
      length = strlen((char*)tmpRD) + 1;
      tmpRD = (char*)tmpRD + length;
      return tmpRD;
    default:
      tmpRD = NULL;
      return tmpRD;
  }
}

const union osc_msg_argument *osc_message_arg(const struct osc_message *msg, size_t arg_index) {
  size_t typeTagLen = strlen(msg->typetag);
  if (typeTagLen - 1 <= arg_index) {
    return NULL;
  }

  size_t index = 0;
  void *tmpRawData = (char*)msg->raw_data + sizeof(uint32_t) + getLength(msg->typetag) + getLength(msg->address);
  do {
    tmpRawData = getArgument(*(msg->typetag + 1 + index), tmpRawData);
    if (!tmpRawData) {
      return NULL;
    }
    ++index;
  }while(index - 1 != arg_index);

  return (union osc_msg_argument *)tmpRawData;
}

size_t osc_message_serialized_length(const struct osc_message *msg) {
  return *((int *)(msg->raw_data));
}

int osc_bundle_new(struct osc_bundle *bnd) {
  void *pData = calloc(20, 1);

  if (!pData) {
    return 1;
  }
  bnd->raw_data = pData;

  int *pLength = pData;
  *pLength = 16;

  char *pText = (char*)pData + sizeof(uint32_t);
  char *bundleText = "#bundle";
  memcpy(pText, bundleText, strlen(bundleText) + 1);

  struct osc_timetag *pTimeTag = (struct osc_timetag *)(pText + (8 * sizeof(char)));
  OSC_TIMETAG_IMMEDIATE(pTimeTag);

  bnd->timetag = pTimeTag;

  return 0;
}

void osc_bundle_destroy(struct osc_bundle *bn) {
  free(bn->raw_data);
  OSC_BUNDLE_NULL(bn);
}

void osc_bundle_set_timetag(struct osc_bundle *bn, struct osc_timetag tag) {
  memcpy(bn->timetag, &tag, sizeof(struct osc_timetag));
}

int osc_bundle_add_message(struct osc_bundle *bundle, const struct osc_message *msg) {
  size_t bundleLenght = *((int *)bundle->raw_data);
  size_t msgLength = *((int *)msg->raw_data) + sizeof(uint32_t);

  void *pData = realloc(bundle->raw_data, bundleLenght + msgLength + sizeof(uint32_t));

  if (!pData) {
    return 1;
  }

  bundle->raw_data = pData;

  *((int *)(bundle->raw_data)) = bundleLenght + msgLength;

  struct osc_message *pMsg = (struct osc_message *)((char*)pData + sizeof(uint32_t) + bundleLenght);
  memcpy(pMsg, msg, msgLength);

  return 0;
}

struct osc_message osc_bundle_next_message(const struct osc_bundle *bundle, struct osc_message prev) {
  // Ignorovat prve 4B, #bundle string, timetag
  size_t ignoringBytes = sizeof(uint32_t) + (8 * sizeof(char)) + sizeof(struct osc_timetag);
  // Konkretna sprava
  char *currentMsg = (char *)(bundle->raw_data) + ignoringBytes;
  // Dlzka danej spravy + 4B
  size_t currentMsgLen = *((int *)(currentMsg)) + sizeof(int32_t);
  if (prev.raw_data == NULL) {
    void *pData = calloc(currentMsgLen, sizeof(char));
    if (!pData) {
      struct osc_message result = {NULL, NULL, NULL};
      return result;
    }
    memcpy(pData, currentMsg, currentMsgLen);

    struct osc_message result;
    result.raw_data = pData;
    result.address = ((char *)(pData)) + sizeof(uint32_t);
    result.typetag = ((char *)(pData)) + sizeof(uint32_t) + getLength(result.address);

    return result;
  }
  // Hodnota od ktorej sa bude odcitavat
  size_t bundleLength = *((int *)(bundle->raw_data)) + sizeof(uint32_t) - ignoringBytes;
  if (bundleLength == 0) {
    struct osc_message result = {NULL, NULL, NULL};
    return result;
  }

  while(memcmp((struct osc_message *)currentMsg, &prev, currentMsgLen) != 0) {
    bundleLength -= currentMsgLen;
    if (bundleLength == 0) {
      struct osc_message result = {NULL, NULL, NULL};
      return result;
    }

    currentMsg = ((char *)(currentMsg)) + currentMsgLen;
    currentMsgLen = *((int *)currentMsg) + sizeof(uint32_t);

  }
  // Ide o posledny ? 
  if (bundleLength - currentMsgLen == 0) {
    struct osc_message result = {NULL, NULL, NULL};
    return result;
  }
  currentMsg = ((char *)(currentMsg)) + currentMsgLen;
  int newMsgLen = *((int *)(currentMsg)) + sizeof(uint32_t);
  void *pData = calloc(newMsgLen, sizeof(char));
  if (!pData) {
      struct osc_message result = {NULL, NULL, NULL};
      return result;
    }
  memcpy(pData, currentMsg, newMsgLen);
  struct osc_message result;
  result.raw_data = pData;
  result.address = ((char *)(pData)) + sizeof(uint32_t);
  result.typetag = ((char *)(pData)) + sizeof(uint32_t) + getLength(result.address);

  return result;
  
}

size_t osc_bundle_serialized_length(const struct osc_bundle *bundle) {
  return *((int *)(bundle->raw_data));
}
