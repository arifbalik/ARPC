#include "arpc_server.h"
#include "arpc_generic.h"
#include "usbd_cdc_if.h"

/* ========== ARPC Server Generic Begin ========== */
static uint8_t buffer[MAX_MESSAGE_BLOCK_LENGTH] = {0};
static uint32_t bufferIndex = 0;

void arpcReceiveFrame(arpcDataFrame_t *responseFrame, uint8_t *buffer);

inline void arpcReceiveFrame(arpcDataFrame_t *responseFrame, uint8_t *buffer) {
  if (responseFrame == NULL) {
    shutdown(ResponseFrameIsNull);
    return;
  }

  responseFrame->length = buffer[0];
  responseFrame->functionId = buffer[1];

  const uint8_t parameterByteCount =
      responseFrame->length - MIN_MESSAGE_BLOCK_LENGTH;

#define OFFSET_BYTE 2
  memcpy(responseFrame->parameters, buffer + OFFSET_BYTE, parameterByteCount);

//#define COPY_ALL

/* not neccesary to copy */
#ifdef COPY_ALL
  responseFrame->crc = buffer[bufferIndex - 2] | (buffer[bufferIndex - 3] << 8);
  responseFrame->sync = buffer[bufferIndex - 1];
#endif
}

void processBuffer() {
  if (!checkBuffer(buffer, bufferIndex))
    goto reset;

  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  arpcReceiveFrame(&callFrame, buffer);

  if (!checkCRC(&callFrame))
    goto reset;


  arpcFrameHandler[callFrame.functionId](&callFrame, &responseFrame);


  arpcSendFrame(&responseFrame);


reset:
  resetBuffer(buffer, &bufferIndex);
}

void arpcByteReceived(uint8_t byte) {
  buffer[bufferIndex++] = byte;

#define FRAME_LENGTH (buffer[0])
  if (bufferIndex >= FRAME_LENGTH)
    processBuffer();
  else if (bufferIndex >= MAX_MESSAGE_BLOCK_LENGTH)
    shutdown(BufferOverFlow);
}

/* ========== ARPC Server Generic End ========== */

#define HAL_Delay_ID 0
extern void HAL_Delay(uint32_t Delay);

void HAL_Delay_generateResponseFrame(arpcDataFrame_t *callFrame,
                                     arpcDataFrame_t *responseFrame) {
  uint32_t Delay;
  memcpy(&Delay, callFrame->parameters, sizeof(uint32_t));
  HAL_Delay(Delay);

  arpcEncodeGeneric(responseFrame, HAL_Delay_ID, NULL, 0);
}

#define setLED_ID 1
extern void setLED(uint8_t value);

void setLED_generateResponseFrame(arpcDataFrame_t *callFrame,
                                  arpcDataFrame_t *responseFrame) {
  uint8_t value;
  memcpy(&value, callFrame->parameters, sizeof(uint8_t));
  setLED(value);

  arpcEncodeGeneric(responseFrame, setLED_ID, NULL, 0);
}

void (*const arpcFrameHandler[UINT8_MAX])(arpcDataFrame_t *callFrame,
                                          arpcDataFrame_t *responseFrame) = {
    &HAL_Delay_generateResponseFrame, &setLED_generateResponseFrame};
