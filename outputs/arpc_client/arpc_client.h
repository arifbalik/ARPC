#ifndef ARPC_CLIENT_H
#define ARPC_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void HAL_Delay(uint32_t Delay);
void setLED(uint8_t value);

#ifdef __cplusplus
}
#endif
#endif // ARPC_CLIENT_H
