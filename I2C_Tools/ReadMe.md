## I2C_Tool

  
A few handy functions for working with I2C chips in STM32 HAL.

  

    /* I2C Scanner using SEGGAR SPRINTF()
    
    The address has been shifted to the left by 1 bit, to match the format used by the HAL functions.
    
    Use the address reported by this function add the device ID in all HAL I2C functions.
    
    */
    
    void I2C_Scanner(I2C_HandleTypeDef hi2c);

  
  
  

    /* Reports if an I2C device is ready for communication at startup. It prints out a message via Segger printf.
    
    It should be called for all I2C devices before entering main loop.
    
    Returns HAL_OK or HAL_ERROR; I use it with a param_assert(0) to kill the code immediately if an I2C device does not respond.
    
    */
    
    uint8_t I2C_DeviceCheck(I2C_HandleTypeDef hi2c, char *DeviceName, uint8_t address);



