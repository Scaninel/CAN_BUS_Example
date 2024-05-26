#include "main.h"

#define MAX_LIN_DATA_BYTE 8
#define _LIN_BUFFER_SIZE 256

HAL_StatusTypeDef LIN_TxMessage(const uint8_t *data, const uint8_t data_size);