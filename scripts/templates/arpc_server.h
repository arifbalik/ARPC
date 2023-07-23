#ifndef ARPC_SERVER_H
#define ARPC_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void arpcByteReceived(uint8_t byte);

#ifndef RPC
#ifdef __cplusplus
#define RPC extern "C"
#else
#define RPC
#endif
#endif

#ifndef ARPC_CONSTANT
#define ARPC_CONSTANT(x, y)
#endif

#ifdef __cplusplus
}
#endif
#endif // ARPC_SERVER_H
