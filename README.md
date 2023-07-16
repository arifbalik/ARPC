# ARPC
ARPC is a fast, very simple, and easy-to-use RPC implementation in C. It is intended to be used in small embedded systems.

## How Does It Work?

Unlike other RPC implementations, ARPC does not need an additional file with a new language like '.proto' or '.idl'. Instead, a Python script searches all of your files and finds functions marked with a spesific marker of your choice, and automatically detects and creates necessary files and functions to call them from any client.

All the user has to do is provide a function for sending and receiving bytes.

## Protocol Message Block

The protocol for ARPC is super simple to increase speed and decrease complexity.

`<Length: 1B>` `<Function ID: 1B>` `<Parameter1: VL>` `<Parameter2: VL>` `<ParameterN: VL>` `<CRC16: 2B>` `<Sync: 1B>`

`<Length: 1B>`: Length of the entire message block in bytes. Assuming a function call with no parameters it can be 5 minimum and 255 maximum. 

`<Function ID: 1B>`: The id of the function to be called. As it is only a byte, only 255 different calls can be registered.

`<ParameterN: VL>`: Nth parameter of the function. It is variable length and it is determined at compile time and encoded/decoded accordingly.

`<CRC16: 2B>`: CRC of the message block including the sync byte

`<Sync: 1B>`: Sync byte

The return of the function has the same signature. Only this time there is only one parameter after the function ID, which is the return value of the function.

## Implementation of Call/Response Frame Generations

A Python script searches all '.c' files in a directory -including sub dirs- and finds all function prototypes starting with `RPC` which is just an empty definition.

```C
#define RPC
```

Consider the following prototype;

```C
RPC int32_t sum(uint32_t a, uint32_t b);
```

This is just a simple function that sums two 32-bit numbers and returns a 32-bit number. The implementation does not care about how the function is defined, it is only concerned about its declaration.

The implementation divides the function into three sections.

- Return Type
- Function Name
- Parameter Types And Names

Then it creates two functions to generate a call and a response frame specific to this function. The pseudo code for those functions are as follows.

### Host Stub Generation
```C
#define sum_ID 0

void sum_generateCallFrame(arpcDataFrame_t *frame, int32_t a, int32_t b) {
  uint8_t parameters[sizeof(a) + sizeof(b)] = { 0 };

  memcpy(parameters, &a, sizeof(a));
  memcpy(parameters + sizeof(a), &b, sizeof(b));

  arpcEncodeGeneric(frame, sum_ID, parameters, sizeof(parameters));
}
```

This function just serializes the parameters into a byte array. And the rest is just general frame generation. CRC and Sync bytes are added inside `arpcEncodeGeneric`. Of course one has to be sure of the implementation and endianness of the types in both host and client.

### Client Marshalling
```C
extern int32_t sum(int32_t a, int32_t b);

void sum_generateResponseFrame(arpcDataFrame_t *callFrame,
                                    arpcDataFrame_t *responseFrame) {
  int32_t parameter1 = parameter1InitZero;
  int32_t parameter2 = parameter2InitZero;

  memcpy(&parameter1, callFrame->parameters, sizeof(int32_t ));
  memcpy(&parameter2, callFrame->parameters + sizeof(int32_t ), sizeof(int32_t ));

  int32_t result = sum(parameter1, parameter2);

  uint8_t resultSerialized[sizeof(int32_t)] = { 0 };

  memcpy(parameters, &resultSerialized, sizeof(resultSerialized));

  arpcEncodeGeneric(responseFrame, callFrame->functionId, resultSerialized,
                    sizeof(resultSerialized));
}
```

Generating response is pretty much the same. First, the call frame is de-serialized and the function is called. Then the return is serialized again and the generic frame encode function is called.

Of course, the client will not call the `sum_generateCallFrame`. The client calls the function `sum` just as it would be defined in the host machine. And that would look something like this inside the client;

```C
int32_t sum(int32_t a, int32_t b) {
  arpcDataFrame_t encodedFrame, responseFrame;

  sum_generateCallFrame(&encodedFrame, a, b);

  arpcSendReceiveFrame(&encodedFrame, &responseFrame);

  int32_t returnVal = 0;

  memcpy(&returnVal, responseFrame.parameters, sizeof(returnVal));

  return returnVal;
}
```

### Supported Data Types

To avoid confusion, the Protocol only supports exact types right now. **It is important to note that the current implementation does not consider the endianness of the client and server**. It is the job of the user to make sure they are compatible.

        void,
        int8_t, uint8_t,
        int16_t, uint16_t,
        int32_t, uint32_t,
        int64_t, uint64_t,
        float, double,
