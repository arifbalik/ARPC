#include "arpc_client.h"

/* ========== ARPC Generic Begin ========== */
#include <string.h>

#define MAX_MESSAGE_BLOCK_LENGTH 255
#define MIN_MESSAGE_BLOCK_LENGTH 5
#define SYNC_BYTE '\n'

typedef struct arpcDataFrame_t {
  uint8_t length;
  uint8_t functionId;
  uint8_t parameters[MAX_MESSAGE_BLOCK_LENGTH - MIN_MESSAGE_BLOCK_LENGTH];
  uint16_t crc;
  uint8_t sync;
} arpcDataFrame_t;

/* private generic functions */
void arpcSendFrame(arpcDataFrame_t *encodedFrame);
void arpcReceiveFrame(arpcDataFrame_t *responseFrame);
void arpcSendReceiveFrame(arpcDataFrame_t *encodedFrame,
                          arpcDataFrame_t *responseFrame);

/* to be implemented by the user */
extern void sendByte(uint8_t byte);
extern void shutdown(uint8_t errorCode);
extern void receiveByte(uint8_t *byte);

extern void (*const frameHandlers[UINT8_MAX])(arpcDataFrame_t *callFrame,
                                              arpcDataFrame_t *responseFrame);

void arpcEncodeGeneric(arpcDataFrame_t *frame, uint8_t functionId,
                       uint8_t *parameters, uint8_t parametersLength) {

  if (frame == NULL) {
    shutdown(FrameIsNull);
    return;
  }

  *frame = (const struct arpcDataFrame_t){0}; /* clear */

  frame->length = MIN_MESSAGE_BLOCK_LENGTH + parametersLength;

  frame->functionId = functionId;

  /* not paramter count but parameter length in bytes */
  memcpy(frame->parameters, parameters, parametersLength);

  frame->crc = 0xFFFF;
  frame->sync = SYNC_BYTE;
}

void arpcSendFrame(arpcDataFrame_t *encodedFrame) {
  if (encodedFrame == NULL) {
    shutdown(EncodedFrameIsNull);
    return;
  }

  sendByte(encodedFrame->length);
  sendByte(encodedFrame->functionId);

  for (uint8_t i = 0; i < encodedFrame->length - MIN_MESSAGE_BLOCK_LENGTH;
       i++) {
    sendByte(encodedFrame->parameters[i]);
  }

  sendByte(encodedFrame->crc & 0xFF);
  sendByte(encodedFrame->crc >> 8);
  sendByte(encodedFrame->sync);
}

void arpcReceiveFrame(arpcDataFrame_t *responseFrame) {
  if (responseFrame == NULL) {
    shutdown(ResponseFrameIsNull);
    return;
  }

  receiveByte(&responseFrame->length);

  if (responseFrame->length > MAX_MESSAGE_BLOCK_LENGTH)
    shutdown(BufferOverFlow);
  else if (responseFrame->length < MIN_MESSAGE_BLOCK_LENGTH)
    shutdown(BadFrame);

  receiveByte(&responseFrame->functionId);

  for (uint8_t i = 0; i < responseFrame->length - MIN_MESSAGE_BLOCK_LENGTH;
       i++) {
    receiveByte(&responseFrame->parameters[i]);
  }

  uint8_t crcLow = 0, crcHigh = 0;

  receiveByte(&crcHigh);
  receiveByte(&crcLow);

  responseFrame->crc = (crcHigh << 8) | crcLow;

  receiveByte(&responseFrame->sync);

  if (responseFrame->sync != SYNC_BYTE)
    shutdown(BadFrame);
}

void arpcSendReceiveFrame(arpcDataFrame_t *encodedFrame,
                          arpcDataFrame_t *responseFrame) {
  arpcSendFrame(encodedFrame);
  arpcReceiveFrame(responseFrame);
}

/* ========== ARPC Generic End ========== */
#define sum_ID 0

void sum_generateCallFrame(arpcDataFrame_t *frame, int32_t a, int32_t b) {

  uint8_t parameters[sizeof(a) + sizeof(b)] = {0};

  memcpy(parameters, &a, sizeof(a));
  memcpy(parameters + sizeof(a), &b, sizeof(b));

  arpcEncodeGeneric(frame, sum_ID, parameters, sizeof(parameters));
}

int32_t sum(int32_t a, int32_t b) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  sum_generateCallFrame(&callFrame, a, b);

  arpcSendReceiveFrame(&callFrame, &responseFrame);

  int32_t returnValue = 0;
  memcpy(&returnValue, responseFrame.parameters, sizeof(int32_t));

  return returnValue;
}

#define getAnalogValue_ID 1

void getAnalogValue_generateCallFrame(arpcDataFrame_t *frame, uint8_t pin) {

  uint8_t parameters[sizeof(pin)] = {0};

  memcpy(parameters, &pin, sizeof(pin));

  arpcEncodeGeneric(frame, getAnalogValue_ID, parameters, sizeof(parameters));
}

int16_t getAnalogValue(uint8_t pin) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  getAnalogValue_generateCallFrame(&callFrame, pin);

  arpcSendReceiveFrame(&callFrame, &responseFrame);

  int16_t returnValue = 0;
  memcpy(&returnValue, responseFrame.parameters, sizeof(int16_t));

  return returnValue;
}

#define printToConsole_ID 2

void printToConsole_generateCallFrame(arpcDataFrame_t *frame, uint8_t c) {

  uint8_t parameters[sizeof(c)] = {0};

  memcpy(parameters, &c, sizeof(c));

  arpcEncodeGeneric(frame, printToConsole_ID, parameters, sizeof(parameters));
}

void printToConsole(uint8_t c) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  printToConsole_generateCallFrame(&callFrame, c);

  arpcSendReceiveFrame(&callFrame, &responseFrame);
}

#define emptyFunction_ID 3

void emptyFunction_generateCallFrame(arpcDataFrame_t *frame) {

  arpcEncodeGeneric(frame, emptyFunction_ID, NULL, 0);
}

void emptyFunction() {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  emptyFunction_generateCallFrame(&callFrame);

  arpcSendReceiveFrame(&callFrame, &responseFrame);
}
