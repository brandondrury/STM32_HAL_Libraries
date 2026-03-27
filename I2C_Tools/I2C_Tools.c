#include "I2C_Tools.h"
#include "main.h"
#include "SEGGER_RTT.h"


void I2C_Scanner(I2C_HandleTypeDef hi2c) {

  uint8_t ret;
  uint16_t address_available[10] = {};
  uint8_t j = 0;
 
  SEGGER_RTT_printf(0, "\r\n---- I2C Scanner Begins ---\r\n");
  uint32_t h = 0;

  for (int i = 0; i < 128; i++) {

    /* Slow down scanner.  I'm not sure if this is necessary, but it seems to be
     * more reliable for some reason. */
    while (h < 10000) {
      h++;
    }
    h = 0;
    ret = HAL_I2C_IsDeviceReady(&hi2c, i << 1, 4, 10);

    if (ret == HAL_OK) {

      // Target device address must be shifted to the left 1 ( 2x )
      SEGGER_RTT_printf(0, "%d", i << 1); // 2x offset.

      address_available[j] = i << 1;

      j++;

    } else {

      SEGGER_RTT_printf(0, ".");
    }

    if (i > 0 && i % 50 == 0) {
      SEGGER_RTT_printf(0, "\r\n");
    }
  }

  SEGGER_RTT_printf(0, "\r\n\r\n");

  for (uint8_t i = 0; i < j; i++) {
    SEGGER_RTT_printf(0, "Values: %d = 0x%02X  \r\n", address_available[i],
                      address_available[i]);
  }

  SEGGER_RTT_printf(0, "\r\n---- I2C Scanner Complete ---\r\n\r\n\r\n");
}




uint8_t I2C_DeviceCheck(I2C_HandleTypeDef hi2c, char *DeviceName,
                          uint8_t address) {

  uint8_t status;
  status = HAL_I2C_IsDeviceReady(&hi2c, address, 4, 100);
  if (status == HAL_OK) {
    SEGGER_RTT_printf(0, "\r\nReady -  %s - address: %d", DeviceName, address);
  } else {
    SEGGER_RTT_printf(0, "\r\nWARNING! %s is not ready at address: %d",
                      DeviceName, address);
  }

  return status;
}