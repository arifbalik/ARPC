#include "arpc_generic.h"

inline uint8_t checkBuffer(uint8_t *buffer, uint32_t bufferIndex) {
  const uint8_t receivedBytes = bufferIndex - 1;

  if (receivedBytes < MIN_MESSAGE_BLOCK_LENGTH) {
    return 0;
  }

#define LAST_BYTE (buffer[receivedBytes])
  if (LAST_BYTE != SYNC_BYTE) {
    return 0;
  }

#define FRAME_LENGTH (buffer[0])
  if (FRAME_LENGTH != receivedBytes) {
    return 0;
  }

  return 1;
}

inline void resetBuffer(uint8_t *buffer, uint32_t *bufferIndex) {
  memset(buffer, 0, MAX_MESSAGE_BLOCK_LENGTH);
  bufferIndex = 0;
}

inline uint16_t calculateCRC(arpcDataFrame_t *frame) {
  uint16_t crc = 0xFFFF;

  return crc;
}

inline uint8_t checkCRC(arpcDataFrame_t *frame) {
  const uint16_t calculatedCRC = calculateCRC(frame);
  return frame->crc != calculatedCRC;
}

inline void arpcEncodeGeneric(arpcDataFrame_t *frame, uint8_t functionId,
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

  frame->sync = SYNC_BYTE;

  calculateCRC(frame);
}

inline void arpcSendFrame(arpcDataFrame_t *encodedFrame) {
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
