#ifndef ARPC_CLIENT_H
#define ARPC_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
  FrameIsNull,
  EncodedFrameIsNull,
  ResponseFrameIsNull,
  BadFrame,
  BufferOverFlow
} ErrorMessages;
 int32_t sum(int32_t a, int32_t b);
 int16_t getAnalogValue(uint8_t pin);
 void printToConsole(uint8_t c);
 void emptyFunction();

#ifdef __cplusplus
}
#endif
#endif // ARPC_CLIENT_H
