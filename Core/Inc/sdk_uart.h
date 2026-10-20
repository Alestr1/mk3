#ifndef SDK_UART_H
#define SDK_UART_H
#include "usart.h"
#include <string.h>

static inline void UART_Transmit(uint8_t* buf) {
    HAL_UART_Transmit(&huart1, buf, strlen((char*)buf), 100);
}
#endif
