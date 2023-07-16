#include "arpc_server.h"
#include "arpc.h"

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

  if (checkCRC(&callFrame))
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

#define writeAnalogValue_ID 0
extern void writeAnalogValue(uint8_t pin, uint16_t value);

void writeAnalogValue_generateResponseFrame(arpcDataFrame_t *callFrame, arpcDataFrame_t *responseFrame) {
uint8_t pin;
  memcpy(&pin, callFrame->parameters, sizeof(uint8_t));
uint16_t value;
  memcpy(&value, callFrame->parameters + sizeof(uint8_t), sizeof(uint16_t));
writeAnalogValue(pin, value);

arpcEncodeGeneric(responseFrame, writeAnalogValue_ID, NULL, 0);

}


#define readAnalogValue_ID 1
extern uint16_t readAnalogValue(uint8_t pin);

void readAnalogValue_generateResponseFrame(arpcDataFrame_t *callFrame, arpcDataFrame_t *responseFrame) {
uint8_t pin;
  memcpy(&pin, callFrame->parameters, sizeof(uint8_t));
uint16_t returnValue = readAnalogValue(pin);

uint8_t returnValueSerialised[sizeof(uint16_t)] = { 0 };
memcpy(returnValueSerialised, &returnValue, sizeof(uint16_t));
arpcEncodeGeneric(responseFrame, readAnalogValue_ID, returnValueSerialised, sizeof(returnValueSerialised));

}


#define gpioWrite_ID 2
extern void gpioWrite(uint8_t pin, uint8_t value);

void gpioWrite_generateResponseFrame(arpcDataFrame_t *callFrame, arpcDataFrame_t *responseFrame) {
uint8_t pin;
  memcpy(&pin, callFrame->parameters, sizeof(uint8_t));
uint8_t value;
  memcpy(&value, callFrame->parameters + sizeof(uint8_t), sizeof(uint8_t));
gpioWrite(pin, value);

arpcEncodeGeneric(responseFrame, gpioWrite_ID, NULL, 0);

}


#define gpioRead_ID 3
extern uint8_t gpioRead(uint8_t pin);

void gpioRead_generateResponseFrame(arpcDataFrame_t *callFrame, arpcDataFrame_t *responseFrame) {
uint8_t pin;
  memcpy(&pin, callFrame->parameters, sizeof(uint8_t));
uint8_t returnValue = gpioRead(pin);

uint8_t returnValueSerialised[sizeof(uint8_t)] = { 0 };
memcpy(returnValueSerialised, &returnValue, sizeof(uint8_t));
arpcEncodeGeneric(responseFrame, gpioRead_ID, returnValueSerialised, sizeof(returnValueSerialised));

}


#define setPinMode_ID 4
extern void setPinMode(uint8_t pin, uint8_t mode);

void setPinMode_generateResponseFrame(arpcDataFrame_t *callFrame, arpcDataFrame_t *responseFrame) {
uint8_t pin;
  memcpy(&pin, callFrame->parameters, sizeof(uint8_t));
uint8_t mode;
  memcpy(&mode, callFrame->parameters + sizeof(uint8_t), sizeof(uint8_t));
setPinMode(pin, mode);

arpcEncodeGeneric(responseFrame, setPinMode_ID, NULL, 0);

}


#define delayMs_ID 5
extern void delayMs(uint16_t ms);

void delayMs_generateResponseFrame(arpcDataFrame_t *callFrame, arpcDataFrame_t *responseFrame) {
uint16_t ms;
  memcpy(&ms, callFrame->parameters, sizeof(uint16_t));
delayMs(ms);

arpcEncodeGeneric(responseFrame, delayMs_ID, NULL, 0);

}


#define delayUs_ID 6
extern void delayUs(uint16_t us);

void delayUs_generateResponseFrame(arpcDataFrame_t *callFrame, arpcDataFrame_t *responseFrame) {
uint16_t us;
  memcpy(&us, callFrame->parameters, sizeof(uint16_t));
delayUs(us);

arpcEncodeGeneric(responseFrame, delayUs_ID, NULL, 0);

}


#define initConsole_ID 7
extern void initConsole(uint32_t baud);

void initConsole_generateResponseFrame(arpcDataFrame_t *callFrame, arpcDataFrame_t *responseFrame) {
uint32_t baud;
  memcpy(&baud, callFrame->parameters, sizeof(uint32_t));
initConsole(baud);

arpcEncodeGeneric(responseFrame, initConsole_ID, NULL, 0);

}


#define printToConsole_ID 8
extern void printToConsole(uint8_t c);

void printToConsole_generateResponseFrame(arpcDataFrame_t *callFrame, arpcDataFrame_t *responseFrame) {
uint8_t c;
  memcpy(&c, callFrame->parameters, sizeof(uint8_t));
printToConsole(c);

arpcEncodeGeneric(responseFrame, printToConsole_ID, NULL, 0);

}

void (*const arpcFrameHandler[UINT8_MAX])(arpcDataFrame_t *callFrame, arpcDataFrame_t *responseFrame) = {&writeAnalogValue_generateResponseFrame, &readAnalogValue_generateResponseFrame, &gpioWrite_generateResponseFrame, &gpioRead_generateResponseFrame, &setPinMode_generateResponseFrame, &delayMs_generateResponseFrame, &delayUs_generateResponseFrame, &initConsole_generateResponseFrame, &printToConsole_generateResponseFrame};