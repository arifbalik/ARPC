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