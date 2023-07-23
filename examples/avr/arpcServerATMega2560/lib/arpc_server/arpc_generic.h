
#ifndef ARPC_H
#define ARPC_H

#include <stdint.h>
#include <string.h>

#define MAX_MESSAGE_BLOCK_LENGTH 255
#define MIN_MESSAGE_BLOCK_LENGTH 5
#define SYNC_BYTE 10

typedef struct arpcDataFrame_t {
  uint8_t length;
  uint8_t functionId;
  uint8_t parameters[MAX_MESSAGE_BLOCK_LENGTH - MIN_MESSAGE_BLOCK_LENGTH];
  uint16_t crc;
  uint8_t sync;
} arpcDataFrame_t;

typedef enum {
  FrameIsNull,
  EncodedFrameIsNull,
  ResponseFrameIsNull,
  BadFrame,
  BufferOverFlow
} ErrorMessages;

extern void (*const arpcFrameHandler[UINT8_MAX])(
    arpcDataFrame_t *callFrame, arpcDataFrame_t *responseFrame);

void arpcEncodeGeneric(arpcDataFrame_t *frame, uint8_t functionId,
                       uint8_t *parameters, uint8_t parametersLength);
void arpcSendFrame(arpcDataFrame_t *encodedFrame);
uint8_t checkBuffer(uint8_t *buffer, uint32_t bufferIndex);
void resetBuffer(uint8_t *buffer, uint32_t *bufferIndex);
uint16_t calculateCRC(arpcDataFrame_t *frame);
uint8_t checkCRC(arpcDataFrame_t *frame);

/* to be implemented by the user */
extern void sendByte(uint8_t byte);
extern void shutdown(uint8_t errorCode);

#endif