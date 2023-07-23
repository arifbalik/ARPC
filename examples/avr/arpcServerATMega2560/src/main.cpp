#include <Arduino.h>
#include <arpc_server.h>

#define SERIALPORT Serial
#define CONSOLE Serial

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
RPC int digitalRead(uint8_t pin);
RPC int analogRead(uint8_t pin);
RPC void analogReference(uint8_t mode);
RPC void analogWrite(uint8_t pin, int val);

RPC unsigned long millis();
RPC unsigned long micros();
RPC void delay(uint32_t ms);
RPC void delayMicroseconds(unsigned int us);
RPC uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout);
RPC uint32_t pulseInLong(uint8_t pin, uint8_t state, uint32_t timeout);

RPC void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder,
                  uint8_t val);
RPC uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

void setup() { SERIALPORT.begin(250000); }
void loop() {
  if (SERIALPORT.available()) {
    uint8_t byte = SERIALPORT.read();
    // SERIALPORT.println("Byte Received: " + String(byte));
    arpcByteReceived(byte);
  }
}

extern "C" void sendByte(uint8_t byte) {
  while (!SERIALPORT.availableForWrite())
    ;
  SERIALPORT.write(byte);
}

extern "C" void shutdown(uint8_t errorCode) {
  noInterrupts();
  CONSOLE.println("Error: " + String(errorCode));
  CONSOLE.flush();
  while (1)
    ;
}
