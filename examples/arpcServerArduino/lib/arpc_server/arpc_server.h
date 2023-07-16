#ifndef ARPC_SERVER_H
#define ARPC_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void arpcByteReceived(uint8_t byte);

#define RPC extern "C"
#define ARPC_CONSTANT(x, y)

#ifdef __cplusplus
}
#endif
#endif // ARPC_SERVER_H
