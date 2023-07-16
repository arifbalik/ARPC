#include <Arduino.h>
#include <arpc_server.h>

#define SERIALPORT Serial

ARPC_CONSTANT("A0", 54)
ARPC_CONSTANT("A1", 55)
ARPC_CONSTANT("A2", 56)
ARPC_CONSTANT("A3", 57)
ARPC_CONSTANT("A4", 58)
ARPC_CONSTANT("A5", 59)
ARPC_CONSTANT("A6", 60)
ARPC_CONSTANT("A7", 61)
ARPC_CONSTANT("A8", 62)
ARPC_CONSTANT("A9", 63)
ARPC_CONSTANT("A10", 64)
ARPC_CONSTANT("A11", 65)
ARPC_CONSTANT("A12", 66)
ARPC_CONSTANT("A13", 67)
ARPC_CONSTANT("A14", 68)
ARPC_CONSTANT("A15", 69)

ARPC_CONSTANT("INPUT", 0)
ARPC_CONSTANT("OUTPUT", 1)
ARPC_CONSTANT("INPUT_PULLUP", 2)

ARPC_CONSTANT("LOW", 0)
ARPC_CONSTANT("HIGH", 1)

/* prototypes */
RPC void pinMode(uint8_t pin, uint8_t mode);
RPC void digitalWrite(uint8_t pin, uint8_t val);
RPC int32_t digitalRead(uint8_t pin);
RPC int32_t analogRead(uint8_t pin);
RPC void analogReference(uint8_t mode);
RPC void analogWrite(uint8_t pin, int32_t val);

RPC uint32_t millis();
RPC uint32_t micros();
RPC void delay(uint32_t ms);
RPC void delayMicroseconds(uint32_t us);
RPC uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout);
RPC uint32_t pulseInLong(uint8_t pin, uint8_t state, uint32_t timeout);

RPC void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
RPC uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

void setup() { SERIALPORT.begin(250000); }
void loop() { if(SERIALPORT.available()) arpcByteReceived(SERIALPORT.read()); }


extern "C" void sendByte(uint8_t byte) {
  while (!SERIALPORT.availableForWrite())
    ;
  SERIALPORT.write(byte);
}

extern "C" void shutdown(uint8_t errorCode) {
  noInterrupts();
  CONSOLE.println("Error: " + String(errorCode));
  while (1)
    ;
}
