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
#define pinMode_ID 0

void pinMode_generateCallFrame(arpcDataFrame_t *frame, uint8_t pin,
                               uint8_t mode) {

  uint8_t parameters[sizeof(pin) + sizeof(mode)] = {0};

  memcpy(parameters, &pin, sizeof(pin));
  memcpy(parameters + sizeof(pin), &mode, sizeof(mode));

  arpcEncodeGeneric(frame, pinMode_ID, parameters, sizeof(parameters));
}

void pinMode(uint8_t pin, uint8_t mode) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  pinMode_generateCallFrame(&callFrame, pin, mode);

  arpcSendReceiveFrame(&callFrame, &responseFrame);
}

#define digitalWrite_ID 1

void digitalWrite_generateCallFrame(arpcDataFrame_t *frame, uint8_t pin,
                                    uint8_t val) {

  uint8_t parameters[sizeof(pin) + sizeof(val)] = {0};

  memcpy(parameters, &pin, sizeof(pin));
  memcpy(parameters + sizeof(pin), &val, sizeof(val));

  arpcEncodeGeneric(frame, digitalWrite_ID, parameters, sizeof(parameters));
}

void digitalWrite(uint8_t pin, uint8_t val) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  digitalWrite_generateCallFrame(&callFrame, pin, val);

  arpcSendReceiveFrame(&callFrame, &responseFrame);
}

#define digitalRead_ID 2

void digitalRead_generateCallFrame(arpcDataFrame_t *frame, uint8_t pin) {

  uint8_t parameters[sizeof(pin)] = {0};

  memcpy(parameters, &pin, sizeof(pin));

  arpcEncodeGeneric(frame, digitalRead_ID, parameters, sizeof(parameters));
}

int32_t digitalRead(uint8_t pin) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  digitalRead_generateCallFrame(&callFrame, pin);

  arpcSendReceiveFrame(&callFrame, &responseFrame);

  int32_t returnValue = 0;
  memcpy(&returnValue, responseFrame.parameters, sizeof(int32_t));

  return returnValue;
}

#define analogRead_ID 3

void analogRead_generateCallFrame(arpcDataFrame_t *frame, uint8_t pin) {

  uint8_t parameters[sizeof(pin)] = {0};

  memcpy(parameters, &pin, sizeof(pin));

  arpcEncodeGeneric(frame, analogRead_ID, parameters, sizeof(parameters));
}

int32_t analogRead(uint8_t pin) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  analogRead_generateCallFrame(&callFrame, pin);

  arpcSendReceiveFrame(&callFrame, &responseFrame);

  int32_t returnValue = 0;
  memcpy(&returnValue, responseFrame.parameters, sizeof(int32_t));

  return returnValue;
}

#define analogReference_ID 4

void analogReference_generateCallFrame(arpcDataFrame_t *frame, uint8_t mode) {

  uint8_t parameters[sizeof(mode)] = {0};

  memcpy(parameters, &mode, sizeof(mode));

  arpcEncodeGeneric(frame, analogReference_ID, parameters, sizeof(parameters));
}

void analogReference(uint8_t mode) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  analogReference_generateCallFrame(&callFrame, mode);

  arpcSendReceiveFrame(&callFrame, &responseFrame);
}

#define analogWrite_ID 5

void analogWrite_generateCallFrame(arpcDataFrame_t *frame, uint8_t pin,
                                   int32_t val) {

  uint8_t parameters[sizeof(pin) + sizeof(val)] = {0};

  memcpy(parameters, &pin, sizeof(pin));
  memcpy(parameters + sizeof(pin), &val, sizeof(val));

  arpcEncodeGeneric(frame, analogWrite_ID, parameters, sizeof(parameters));
}

void analogWrite(uint8_t pin, int32_t val) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  analogWrite_generateCallFrame(&callFrame, pin, val);

  arpcSendReceiveFrame(&callFrame, &responseFrame);
}

#define millis_ID 6

void millis_generateCallFrame(arpcDataFrame_t *frame) {

  arpcEncodeGeneric(frame, millis_ID, NULL, 0);
}

uint32_t millis() {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  millis_generateCallFrame(&callFrame);

  arpcSendReceiveFrame(&callFrame, &responseFrame);

  uint32_t returnValue = 0;
  memcpy(&returnValue, responseFrame.parameters, sizeof(uint32_t));

  return returnValue;
}

#define micros_ID 7

void micros_generateCallFrame(arpcDataFrame_t *frame) {

  arpcEncodeGeneric(frame, micros_ID, NULL, 0);
}

uint32_t micros() {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  micros_generateCallFrame(&callFrame);

  arpcSendReceiveFrame(&callFrame, &responseFrame);

  uint32_t returnValue = 0;
  memcpy(&returnValue, responseFrame.parameters, sizeof(uint32_t));

  return returnValue;
}

#define delay_ID 8

void delay_generateCallFrame(arpcDataFrame_t *frame, uint32_t ms) {

  uint8_t parameters[sizeof(ms)] = {0};

  memcpy(parameters, &ms, sizeof(ms));

  arpcEncodeGeneric(frame, delay_ID, parameters, sizeof(parameters));
}

void delay(uint32_t ms) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  delay_generateCallFrame(&callFrame, ms);

  arpcSendReceiveFrame(&callFrame, &responseFrame);
}

#define delayMicroseconds_ID 9

void delayMicroseconds_generateCallFrame(arpcDataFrame_t *frame, uint32_t us) {

  uint8_t parameters[sizeof(us)] = {0};

  memcpy(parameters, &us, sizeof(us));

  arpcEncodeGeneric(frame, delayMicroseconds_ID, parameters,
                    sizeof(parameters));
}

void delayMicroseconds(uint32_t us) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  delayMicroseconds_generateCallFrame(&callFrame, us);

  arpcSendReceiveFrame(&callFrame, &responseFrame);
}

#define pulseIn_ID 10

void pulseIn_generateCallFrame(arpcDataFrame_t *frame, uint8_t pin,
                               uint8_t state, uint32_t timeout) {

  uint8_t parameters[sizeof(pin) + sizeof(state) + sizeof(timeout)] = {0};

  memcpy(parameters, &pin, sizeof(pin));
  memcpy(parameters + sizeof(pin), &state, sizeof(state));
  memcpy(parameters + sizeof(pin) + sizeof(state), &timeout, sizeof(timeout));

  arpcEncodeGeneric(frame, pulseIn_ID, parameters, sizeof(parameters));
}

uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  pulseIn_generateCallFrame(&callFrame, pin, state, timeout);

  arpcSendReceiveFrame(&callFrame, &responseFrame);

  uint32_t returnValue = 0;
  memcpy(&returnValue, responseFrame.parameters, sizeof(uint32_t));

  return returnValue;
}

#define pulseInLong_ID 11

void pulseInLong_generateCallFrame(arpcDataFrame_t *frame, uint8_t pin,
                                   uint8_t state, uint32_t timeout) {

  uint8_t parameters[sizeof(pin) + sizeof(state) + sizeof(timeout)] = {0};

  memcpy(parameters, &pin, sizeof(pin));
  memcpy(parameters + sizeof(pin), &state, sizeof(state));
  memcpy(parameters + sizeof(pin) + sizeof(state), &timeout, sizeof(timeout));

  arpcEncodeGeneric(frame, pulseInLong_ID, parameters, sizeof(parameters));
}

uint32_t pulseInLong(uint8_t pin, uint8_t state, uint32_t timeout) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  pulseInLong_generateCallFrame(&callFrame, pin, state, timeout);

  arpcSendReceiveFrame(&callFrame, &responseFrame);

  uint32_t returnValue = 0;
  memcpy(&returnValue, responseFrame.parameters, sizeof(uint32_t));

  return returnValue;
}

#define shiftOut_ID 12

void shiftOut_generateCallFrame(arpcDataFrame_t *frame, uint8_t dataPin,
                                uint8_t clockPin, uint8_t bitOrder,
                                uint8_t val) {

  uint8_t parameters[sizeof(dataPin) + sizeof(clockPin) + sizeof(bitOrder) +
                     sizeof(val)] = {0};

  memcpy(parameters, &dataPin, sizeof(dataPin));
  memcpy(parameters + sizeof(dataPin), &clockPin, sizeof(clockPin));
  memcpy(parameters + sizeof(dataPin) + sizeof(clockPin), &bitOrder,
         sizeof(bitOrder));
  memcpy(parameters + sizeof(dataPin) + sizeof(clockPin) + sizeof(bitOrder),
         &val, sizeof(val));

  arpcEncodeGeneric(frame, shiftOut_ID, parameters, sizeof(parameters));
}

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder,
              uint8_t val) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  shiftOut_generateCallFrame(&callFrame, dataPin, clockPin, bitOrder, val);

  arpcSendReceiveFrame(&callFrame, &responseFrame);
}

#define shiftIn_ID 13

void shiftIn_generateCallFrame(arpcDataFrame_t *frame, uint8_t dataPin,
                               uint8_t clockPin, uint8_t bitOrder) {

  uint8_t parameters[sizeof(dataPin) + sizeof(clockPin) + sizeof(bitOrder)] = {
      0};

  memcpy(parameters, &dataPin, sizeof(dataPin));
  memcpy(parameters + sizeof(dataPin), &clockPin, sizeof(clockPin));
  memcpy(parameters + sizeof(dataPin) + sizeof(clockPin), &bitOrder,
         sizeof(bitOrder));

  arpcEncodeGeneric(frame, shiftIn_ID, parameters, sizeof(parameters));
}

uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
  arpcDataFrame_t callFrame = {0};
  arpcDataFrame_t responseFrame = {0};

  shiftIn_generateCallFrame(&callFrame, dataPin, clockPin, bitOrder);

  arpcSendReceiveFrame(&callFrame, &responseFrame);

  uint8_t returnValue = 0;
  memcpy(&returnValue, responseFrame.parameters, sizeof(uint8_t));

  return returnValue;
}
