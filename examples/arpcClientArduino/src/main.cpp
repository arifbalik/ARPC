#include <Arduino.h>
#include <arpc_client.h>

#define ARPCPORT Serial1

uint8_t receiveBytes[5] = {5, 3, 0xFF, 0xFF, '\n'};
uint8_t receiveIndex = 0;

extern "C" void sendByte(uint8_t byte) {
  while (!ARPCPORT.availableForWrite())
    ;
  ARPCPORT.write(byte);
}
extern "C" void receiveByte(uint8_t *byte) {
  while (!ARPCPORT.available())
    ;
  *byte = ARPCPORT.read();
}
extern "C" void shutdown(uint8_t errorCode) {
  String errorString;
  switch (errorCode) {
  case FrameIsNull:
    errorString = "Frame is null";
    break;
  case EncodedFrameIsNull:
    errorString = "Encoded frame is null";
    break;
  case ResponseFrameIsNull:
    errorString = "Response frame is null";
    break;
  case BadFrame:
    errorString = "Bad frame";
    break;
  case BufferOverFlow:
    errorString = "Buffer overflow";
    break;
  default:
    errorString = "Unknown error";
    break;
  }

  Serial.print("Error: " + errorString);
  while (1)
    ;
}

void setup() {
  ARPCPORT.begin(250000);
  Serial.begin(9600);
  Serial.println("Client Started");
}

void loop() {
  long start = millis();
  long numberOfOperations = 0;

  while (millis() - start < 1000) {
    long val = 0;
#define OVERSAMPLE 16
#define NUMBER_OF_CHANNELS 20
    for (int i = 0; i < OVERSAMPLE * NUMBER_OF_CHANNELS; i++) {
      val += getAnalogValue(54);
      numberOfOperations++;
    }
    Serial.println(val / (OVERSAMPLE * NUMBER_OF_CHANNELS));
  }

  delay(200);

  Serial.println("Number of operations per second: " +
                 String(numberOfOperations) + " One operation takes: " +
                 String((1000000.0f / (float)numberOfOperations)) + " us");
}