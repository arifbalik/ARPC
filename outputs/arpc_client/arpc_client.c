#include "arpc_client.h"
#include "arpc_generic.h"

/* ========== ARPC Client Generic Begin ========== */

extern void receiveByte(uint8_t *byte);

void arpcSendReceiveFrame(arpcDataFrame_t *encodedFrame,
                          arpcDataFrame_t *responseFrame);
void arpcReceiveFrame(arpcDataFrame_t *responseFrame);

inline void arpcReceiveFrame(arpcDataFrame_t *responseFrame) {
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

inline void arpcSendReceiveFrame(arpcDataFrame_t *encodedFrame,
                                 arpcDataFrame_t *responseFrame) {
  arpcSendFrame(encodedFrame);
  arpcReceiveFrame(responseFrame);
}

/* ========== ARPC Client Generic End ========== */
#define HAL_Delay_ID 0

void HAL_Delay_generateCallFrame(arpcDataFrame_t *frame, uint32_t Delay) {

  uint8_t parameters[sizeof(Delay)] = {0};

  memcpy(parameters, &Delay, sizeof(Delay));

  arpcEncodeGeneric(frame, HAL_Delay_ID, parameters, sizeof(parameters));
}

void HAL_Delay(uint32_t Delay) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  HAL_Delay_generateCallFrame(&callFrame, Delay);

  arpcSendReceiveFrame(&callFrame, &responseFrame);
}

#define setLED_ID 1

void setLED_generateCallFrame(arpcDataFrame_t *frame, uint8_t value) {

  uint8_t parameters[sizeof(value)] = {0};

  memcpy(parameters, &value, sizeof(value));

  arpcEncodeGeneric(frame, setLED_ID, parameters, sizeof(parameters));
}

void setLED(uint8_t value) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  setLED_generateCallFrame(&callFrame, value);

  arpcSendReceiveFrame(&callFrame, &responseFrame);
}
