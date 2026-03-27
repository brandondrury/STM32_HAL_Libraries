#ifndef I2C_TOOLS
#define I2C_TOOLS

#include "main.h"

/* I2C Scanner using SEGGAR SPRINTF()
The address has been shifted to the left by 1 bit, to match the format used by the HAL functions.  
Use the address reported by this function add the device ID in all HAL I2C functions.  
*/
void I2C_Scanner(I2C_HandleTypeDef hi2c);



/* Reports if an I2C device is ready for communication at startup. It prints out a message via Segger printf.
It should be called for all I2C devices before entering main loop.  
Returns HAL_OK or HAL_ERROR;
*/
uint8_t I2C_DeviceCheck(I2C_HandleTypeDef hi2c, char *DeviceName, uint8_t address);



#endif