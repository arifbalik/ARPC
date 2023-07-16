#ifndef ARPC_CLIENT_H
#define ARPC_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define RPC extern "C"
#define ARPC_CONSTANT(x, y)

#define A0  54
#define A1  55
#define A2  56
#define A3  57
#define A4  58
#define A5  59
#define A6  60
#define A7  61
#define A8  62
#define A9  63
#define A10  64
#define A11  65
#define A12  66
#define A13  67
#define A14  68
#define A15  69
#define INPUT  0
#define OUTPUT  1
#define INPUT_PULLUP  2
#define LOW  0
#define HIGH  1

 void pinMode(uint8_t pin, uint8_t mode);
 void digitalWrite(uint8_t pin, uint8_t val);
 int32_t digitalRead(uint8_t pin);
 int32_t analogRead(uint8_t pin);
 void analogReference(uint8_t mode);
 void analogWrite(uint8_t pin, int32_t val);
 uint32_t millis();
 uint32_t micros();
 void delay(uint32_t ms);
 void delayMicroseconds(uint32_t us);
 uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout);
 uint32_t pulseInLong(uint8_t pin, uint8_t state, uint32_t timeout);
 void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
 uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

#ifdef __cplusplus
}
#endif
#endif // ARPC_CLIENT_H
