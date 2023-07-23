#include "arpc_server.h"
#include "arpc_generic.h"
/* ========== ARPC Server Generic Begin ========== */
static uint8_t buffer[MAX_MESSAGE_BLOCK_LENGTH] = {0};
static uint32_t bufferIndex = 0;

void sendString(const char *msg, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    sendByte(msg[i]);
  }
}

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

#define COPY_ALL

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
  buffer[bufferIndex] = byte;
  bufferIndex++;

#define FRAME_LENGTH (buffer[0])
  if (bufferIndex >= FRAME_LENGTH)
    processBuffer();
  else if (bufferIndex >= MAX_MESSAGE_BLOCK_LENGTH)
    shutdown(BufferOverFlow);
}

/* ========== ARPC Server Generic End ========== */

#define pinMode_ID 0
extern void pinMode(uint8_t pin, uint8_t mode);

void pinMode_generateResponseFrame(arpcDataFrame_t *callFrame,
                                   arpcDataFrame_t *responseFrame) {
  uint8_t pin;
  memcpy(&pin, callFrame->parameters, sizeof(uint8_t));
  uint8_t mode;
  memcpy(&mode, callFrame->parameters + sizeof(uint8_t), sizeof(uint8_t));
  pinMode(pin, mode);

  arpcEncodeGeneric(responseFrame, pinMode_ID, NULL, 0);
}

#define digitalWrite_ID 1
extern void digitalWrite(uint8_t pin, uint8_t val);

void digitalWrite_generateResponseFrame(arpcDataFrame_t *callFrame,
                                        arpcDataFrame_t *responseFrame) {
  uint8_t pin;
  memcpy(&pin, callFrame->parameters, sizeof(uint8_t));
  uint8_t val;
  memcpy(&val, callFrame->parameters + sizeof(uint8_t), sizeof(uint8_t));
  digitalWrite(pin, val);

  arpcEncodeGeneric(responseFrame, digitalWrite_ID, NULL, 0);
}

#define digitalRead_ID 2
extern int32_t digitalRead(uint8_t pin);

void digitalRead_generateResponseFrame(arpcDataFrame_t *callFrame,
                                       arpcDataFrame_t *responseFrame) {
  uint8_t pin;
  memcpy(&pin, callFrame->parameters, sizeof(uint8_t));
  int32_t returnValue = digitalRead(pin);

  uint8_t returnValueSerialised[sizeof(int32_t)] = {0};
  memcpy(returnValueSerialised, &returnValue, sizeof(int32_t));
  arpcEncodeGeneric(responseFrame, digitalRead_ID, returnValueSerialised,
                    sizeof(returnValueSerialised));
}

#define analogRead_ID 3
extern int32_t analogRead(uint8_t pin);

void analogRead_generateResponseFrame(arpcDataFrame_t *callFrame,
                                      arpcDataFrame_t *responseFrame) {
  uint8_t pin;
  memcpy(&pin, callFrame->parameters, sizeof(uint8_t));
  int32_t returnValue = analogRead(pin);

  uint8_t returnValueSerialised[sizeof(int32_t)] = {0};
  memcpy(returnValueSerialised, &returnValue, sizeof(int32_t));
  arpcEncodeGeneric(responseFrame, analogRead_ID, returnValueSerialised,
                    sizeof(returnValueSerialised));
}

#define analogReference_ID 4
extern void analogReference(uint8_t mode);

void analogReference_generateResponseFrame(arpcDataFrame_t *callFrame,
                                           arpcDataFrame_t *responseFrame) {
  uint8_t mode;
  memcpy(&mode, callFrame->parameters, sizeof(uint8_t));
  analogReference(mode);

  arpcEncodeGeneric(responseFrame, analogReference_ID, NULL, 0);
}

#define analogWrite_ID 5
extern void analogWrite(uint8_t pin, int32_t val);

void analogWrite_generateResponseFrame(arpcDataFrame_t *callFrame,
                                       arpcDataFrame_t *responseFrame) {
  uint8_t pin;
  memcpy(&pin, callFrame->parameters, sizeof(uint8_t));
  int32_t val;
  memcpy(&val, callFrame->parameters + sizeof(uint8_t), sizeof(int32_t));
  analogWrite(pin, val);

  arpcEncodeGeneric(responseFrame, analogWrite_ID, NULL, 0);
}

#define millis_ID 6
extern uint32_t millis();

void millis_generateResponseFrame(arpcDataFrame_t *callFrame,
                                  arpcDataFrame_t *responseFrame) {
  uint32_t returnValue = millis();

  uint8_t returnValueSerialised[sizeof(uint32_t)] = {0};
  memcpy(returnValueSerialised, &returnValue, sizeof(uint32_t));
  arpcEncodeGeneric(responseFrame, millis_ID, returnValueSerialised,
                    sizeof(returnValueSerialised));
}

#define micros_ID 7
extern uint32_t micros();

void micros_generateResponseFrame(arpcDataFrame_t *callFrame,
                                  arpcDataFrame_t *responseFrame) {
  uint32_t returnValue = micros();

  uint8_t returnValueSerialised[sizeof(uint32_t)] = {0};
  memcpy(returnValueSerialised, &returnValue, sizeof(uint32_t));
  arpcEncodeGeneric(responseFrame, micros_ID, returnValueSerialised,
                    sizeof(returnValueSerialised));
}

#define delay_ID 8
extern void delay(uint32_t ms);

void delay_generateResponseFrame(arpcDataFrame_t *callFrame,
                                 arpcDataFrame_t *responseFrame) {
  uint32_t ms;
  memcpy(&ms, callFrame->parameters, sizeof(uint32_t));
  delay(ms);

  arpcEncodeGeneric(responseFrame, delay_ID, NULL, 0);
}

#define delayMicroseconds_ID 9
extern void delayMicroseconds(uint32_t us);

void delayMicroseconds_generateResponseFrame(arpcDataFrame_t *callFrame,
                                             arpcDataFrame_t *responseFrame) {
  uint32_t us;
  memcpy(&us, callFrame->parameters, sizeof(uint32_t));
  delayMicroseconds(us);

  arpcEncodeGeneric(responseFrame, delayMicroseconds_ID, NULL, 0);
}

#define pulseIn_ID 10
extern uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout);

void pulseIn_generateResponseFrame(arpcDataFrame_t *callFrame,
                                   arpcDataFrame_t *responseFrame) {
  uint8_t pin;
  memcpy(&pin, callFrame->parameters, sizeof(uint8_t));
  uint8_t state;
  memcpy(&state, callFrame->parameters + sizeof(uint8_t), sizeof(uint8_t));
  uint32_t timeout;
  memcpy(&timeout, callFrame->parameters + sizeof(uint8_t) + sizeof(uint8_t),
         sizeof(uint32_t));
  uint32_t returnValue = pulseIn(pin, state, timeout);

  uint8_t returnValueSerialised[sizeof(uint32_t)] = {0};
  memcpy(returnValueSerialised, &returnValue, sizeof(uint32_t));
  arpcEncodeGeneric(responseFrame, pulseIn_ID, returnValueSerialised,
                    sizeof(returnValueSerialised));
}

#define pulseInLong_ID 11
extern uint32_t pulseInLong(uint8_t pin, uint8_t state, uint32_t timeout);

void pulseInLong_generateResponseFrame(arpcDataFrame_t *callFrame,
                                       arpcDataFrame_t *responseFrame) {
  uint8_t pin;
  memcpy(&pin, callFrame->parameters, sizeof(uint8_t));
  uint8_t state;
  memcpy(&state, callFrame->parameters + sizeof(uint8_t), sizeof(uint8_t));
  uint32_t timeout;
  memcpy(&timeout, callFrame->parameters + sizeof(uint8_t) + sizeof(uint8_t),
         sizeof(uint32_t));
  uint32_t returnValue = pulseInLong(pin, state, timeout);

  uint8_t returnValueSerialised[sizeof(uint32_t)] = {0};
  memcpy(returnValueSerialised, &returnValue, sizeof(uint32_t));
  arpcEncodeGeneric(responseFrame, pulseInLong_ID, returnValueSerialised,
                    sizeof(returnValueSerialised));
}

#define shiftOut_ID 12
extern void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

void shiftOut_generateResponseFrame(arpcDataFrame_t *callFrame,
                                    arpcDataFrame_t *responseFrame) {
  uint8_t dataPin;
  memcpy(&dataPin, callFrame->parameters, sizeof(uint8_t));
  uint8_t clockPin;
  memcpy(&clockPin, callFrame->parameters + sizeof(uint8_t), sizeof(uint8_t));
  uint8_t bitOrder;
  memcpy(&bitOrder, callFrame->parameters + sizeof(uint8_t) + sizeof(uint8_t),
         sizeof(uint8_t));
  shiftOut(dataPin, clockPin, bitOrder);

  arpcEncodeGeneric(responseFrame, shiftOut_ID, NULL, 0);
}

#define shiftIn_ID 13
extern uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

void shiftIn_generateResponseFrame(arpcDataFrame_t *callFrame,
                                   arpcDataFrame_t *responseFrame) {
  uint8_t dataPin;
  memcpy(&dataPin, callFrame->parameters, sizeof(uint8_t));
  uint8_t clockPin;
  memcpy(&clockPin, callFrame->parameters + sizeof(uint8_t), sizeof(uint8_t));
  uint8_t bitOrder;
  memcpy(&bitOrder, callFrame->parameters + sizeof(uint8_t) + sizeof(uint8_t),
         sizeof(uint8_t));
  uint8_t returnValue = shiftIn(dataPin, clockPin, bitOrder);

  uint8_t returnValueSerialised[sizeof(uint8_t)] = {0};
  memcpy(returnValueSerialised, &returnValue, sizeof(uint8_t));
  arpcEncodeGeneric(responseFrame, shiftIn_ID, returnValueSerialised,
                    sizeof(returnValueSerialised));
}

void (*const arpcFrameHandler[UINT8_MAX])(arpcDataFrame_t *callFrame,
                                          arpcDataFrame_t *responseFrame) = {
    &pinMode_generateResponseFrame,
    &digitalWrite_generateResponseFrame,
    &digitalRead_generateResponseFrame,
    &analogRead_generateResponseFrame,
    &analogReference_generateResponseFrame,
    &analogWrite_generateResponseFrame,
    &millis_generateResponseFrame,
    &micros_generateResponseFrame,
    &delay_generateResponseFrame,
    &delayMicroseconds_generateResponseFrame,
    &pulseIn_generateResponseFrame,
    &pulseInLong_generateResponseFrame,
    &shiftOut_generateResponseFrame,
    &shiftIn_generateResponseFrame};