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
  // velkost raw data (celej spravy bez prvych 4B);
  int msgLen = *(int*)msg->raw_data;
  // Velkost typetagu + zvysnych dat
  size_t ttDataLen = msgLen - msgAdrLen;
  // Velkost hlavicky
  int newDataLength = addressLen + ttDataLen;
  // Realokacia
  void *pData = realloc(msg->raw_data, (addressLen + ttDataLen + sizeof(int)));
  if (!pData) {
    return 1;
  }
  // Zmenim pointer na novu adresu 
  char *newAddress = pData + (4*sizeof(char));
  memcpy(newAddress, address, addressLen);
  msg->address = newAddress;
  // Zmenit pointer na novy typetag spolu s datami
  char *newTypeTagData = pData + sizeof(char)*(sizeof(int) + addressLen);
  memcpy(newTypeTagData, msg->typetag, ttDataLen);
  msg->typetag = newTypeTagData;
  // Zmenim velkost
  int *PoiNewDataLength = pData;
  *PoiNewDataLength = newDataLength;
  msg->raw_data = PoiNewDataLength;

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
  char *restData = msg->raw_data + msgAdrLen + msgTtLen + sizeof(int);
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
  void *pData = realloc(msg->raw_data, (sizeof(int) + newMsgLen));

  if (!pData) {
    return 1;
  }
  // Stare data
  char *pRestData = pData + sizeof(int) + msgAdrLen + newTtagLen;
  memcpy(pRestData, restData, restDataLen);
  // Nove data
  int32_t *pNewData = pData + sizeof(int) + msgAdrLen + newTtagLen + restDataLen;
  *pNewData = data;
  // Prve 4B (aktualizovana dlzka)
  int *pRawData = pData;
  *pRawData = newMsgLen;
  msg->raw_data = pRawData;
  // Adresa
  char *pAddress = pData + sizeof(int);
  memcpy(pAddress, msg->address, msgAdrLen);
  msg->address = pAddress;
  // TypeTag
  char *pTypetag = pData + sizeof(int) + msgAdrLen;
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
  // pointer pre pripad zlyhania reallocu
  void *oldPtr = msg->raw_data;
  // pointer na data ktore sa nachadzaju za typetagom
  char *restData = msg->raw_data + msgAdrLen + msgTtLen + sizeof(int);
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
  void *pData = realloc(msg->raw_data, (sizeof(int) + newMsgLen));

  if (!pData) {
    return 1;
  }
  // Stare data
  char *pRestData = pData + sizeof(int) + msgAdrLen + newTtagLen;
  memcpy(pRestData, restData, restDataLen);
  // Nove data
  float *pNewData = pData + sizeof(int) + msgAdrLen + newTtagLen + restDataLen;
  *pNewData = data;
  // Prve 4B (aktualizovana dlzka)
  int *pRawData = pData;
  *pRawData = newMsgLen;
  msg->raw_data = pRawData;
  // Adresa
  char *pAddress = pData + sizeof(int);
  memcpy(pAddress, msg->address, msgAdrLen);
  msg->address = pAddress;
  // TypeTag
  char *pTypetag = pData + sizeof(int) + msgAdrLen;
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
  // pointer pre pripad zlyhania reallocu
  void *oldPtr = msg->raw_data;
  // pointer na data ktore sa nachadzaju za typetagom
  char *restData = msg->raw_data + msgAdrLen + msgTtLen + sizeof(int);
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
  void *pData = realloc(msg->raw_data, (sizeof(int) + newMsgLen));

  if (!pData) {
    return 1;
  }
  // Stare data
  char *pRestData = pData + sizeof(int) + msgAdrLen + newTtagLen;
  memcpy(pRestData, restData, restDataLen);
  // Nove data
  struct osc_timetag *pNewData = pData + sizeof(int) + msgAdrLen + newTtagLen + restDataLen;
  *pNewData = tag;
  // Prve 4B (aktualizovana dlzka)
  int *pRawData = pData;
  *pRawData = newMsgLen;
  msg->raw_data = pRawData;
  // Adresa
  char *pAddress = pData + sizeof(int);
  memcpy(pAddress, msg->address, msgAdrLen);
  msg->address = pAddress;
  // TypeTag
  char *pTypetag = pData + sizeof(int) + msgAdrLen;
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
  // pointer pre pripad zlyhania reallocu
  void *oldPtr = msg->raw_data;
  // pointer na data ktore sa nachadzaju za typetagom
  char *restData = msg->raw_data + msgAdrLen + msgTtLen + sizeof(int);
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
  void *pData = realloc(msg->raw_data, (sizeof(int) + newMsgLen));

  if (!pData) {
    return 1;
  }
  // Stare data
  char *pRestData = pData + sizeof(int) + msgAdrLen + newTtagLen;
  memcpy(pRestData, restData, restDataLen);
  // Nove data
  char *pNewData = pData + sizeof(int) + msgAdrLen + newTtagLen + restDataLen;
  memcpy(pNewData, data, strlen(data) + 1);
  // Prve 4B (aktualizovana dlzka)
  int *pRawData = pData;
  *pRawData = newMsgLen;
  msg->raw_data = pRawData;
  // Adresa
  char *pAddress = pData + sizeof(int);
  memcpy(pAddress, msg->address, msgAdrLen);
  msg->address = pAddress;
  // TypeTag
  char *pTypetag = pData + sizeof(int) + msgAdrLen;
  memcpy(pTypetag, newTypeTag, newTtagLen);
  msg->typetag = pTypetag;
  // Uvolnit pamat ktora vznikla vo funkcii addNewTypeTag
  free(newTypeTag);
  return 0;
}

size_t osc_message_argc(const struct osc_message *msg) {
  return strlen(msg->typetag) - 1;
}

const union osc_msg_argument *osc_message_arg(const struct osc_message *msg, size_t arg_index) {
  
}

size_t osc_message_serialized_length(const struct osc_message *msg) {
  return *(int*)msg->raw_data;
}


int main(void) {
  return 0;
}