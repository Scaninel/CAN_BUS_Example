#include "usr_lin.h"

HAL_StatusTypeDef UsrCanTempTxProccess(uint8_t *data, uint8_t data_size);
HAL_StatusTypeDef UsrCanRefSpeedTxProccess(uint8_t *data, uint8_t data_size);

extern volatile uint32_t canTimer;