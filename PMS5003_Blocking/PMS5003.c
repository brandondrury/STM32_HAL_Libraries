#include "PMS5003.h"
#include "SEGGER_RTT.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_uart.h"
#include <stdbool.h>
#include <stdint.h>

PMS5003_Data_t DustSensor;
PMS5003_Monitor_t PMS5003_Health;

uint8_t PMS5003_raw_data[30];
uint8_t start_byte_check[2];
uint8_t PMS5003_data_ok = 0;

/* This sensor library needs to count bad reads in a row.  See PMS5003_Health
 */

uint16_t Gen16BitFrom8Bit(uint8_t high, uint8_t low) {
  uint16_t merged_value = high << 8 | low;
  return merged_value;
}

Status_PMS5003_t PMS5003_Measurement(UART_HandleTypeDef huart) {

  uint8_t status;

  status = PMS5003_read_data(huart);
  if (status != PMS5003_OK) {
    SEGGER_RTT_printf(0, "\r\n UART read error on PMS5003 sensor.\r\n");
    return PMS5003_UART_READ_ERROR;
  }

  status = PMS5003_checksum();
  if (status != PMS5003_OK) {
    SEGGER_RTT_printf(0, "\r\n Checksum error on PMS5003 sensor.\r\n");
    return PMS5003_CHECKSUM_ERROR;
  }

  PMS5003_parse_data();

  status = PMS5003_data_plausible();
  if (status != PMS5003_OK) {
    SEGGER_RTT_printf(0,
                      "\r\n Data out-of-bounds error on PMS5003 sensor.\r\n");
    return PMS5003_VALUE_BOUNDS_ERROR;
  }


  status = PMS5003_froze_sensor();
  if (status != PMS5003_OK) {
    SEGGER_RTT_printf(0, "\r\n Error: PMS5003 sensor not updating.\r\n");
    return PMS5003_FROZE;
  }

  return PMS5003_OK;
}

Status_PMS5003_t PMS5003_read_data(UART_HandleTypeDef huart) {
  // Blocking mode
  while (1) {

    // check startbyte 0 first.
    HAL_UART_Receive(&huart, &start_byte_check[0], 1, HAL_MAX_DELAY);

    if (start_byte_check[0] != PMS5003_START_BYTE0) {
      continue;
    } // restart loop if we don't have the start byte

    HAL_UART_Receive(&huart, &start_byte_check[1], 1, HAL_MAX_DELAY);

    if (start_byte_check[1] != PMS5003_START_BYTE1) {
      continue;
    } // restart loop if we don't have the second start byte, too.

    // If we have both start bytes, read the 30 bytes of dust data
    HAL_UART_Receive(&huart, PMS5003_raw_data, 30, HAL_MAX_DELAY);
    // break;
    return PMS5003_OK;
  }
}

bool RangeCheck(uint16_t value) {
  /* This sensor consistently reads zero, but appears to be funcitoning
  "correctly. I'm removing the minimum value check."*/
  if (value <= MAX_CONCENTRATION_VALUE) {
    return 0;
  } else {
    return 1;
  }
}

// maybe dead is a better word than froze.

Status_PMS5003_t PMS5003_froze_sensor(void) {

  if (PMS5003_Health.reads_without_change > MAX_READS_WITHOUT_CHANGE_THRESH) {
    return PMS5003_FROZE;
  } else {
    return PMS5003_OK;
  }
}

Status_PMS5003_t PMS5003_data_plausible(void) {
  // check if values are within range?  What range?  I don't know.
  uint8_t range_issues_count = 0;
  bool issue = 0;

  issue = RangeCheck(DustSensor.PM1_0_Concentration);
  range_issues_count += issue;

  issue = RangeCheck(DustSensor.PM2_5_Concentration);
  range_issues_count += issue;

  issue = RangeCheck(DustSensor.PM10_Concentration);
  range_issues_count += issue;

  if (range_issues_count == 0) {
    return PMS5003_OK;
  } else {
    return PMS5003_VALUE_BOUNDS_ERROR;
  }
}

void PMS5003_parse_data(void) {

  DustSensor.PM1_0_Concentration =
      Gen16BitFrom8Bit(PMS5003_raw_data[PM1_0_con_index_high],
                       PMS5003_raw_data[PM1_0_con_index_low]);

  DustSensor.PM2_5_Concentration =
      Gen16BitFrom8Bit(PMS5003_raw_data[PM2_5_con_index_high],
                       PMS5003_raw_data[PM2_5_con_index_low]);

  DustSensor.PM10_Concentration =
      Gen16BitFrom8Bit(PMS5003_raw_data[PM10_con_index_high],
                       PMS5003_raw_data[PM10_con_index_low]);
}

Status_PMS5003_t PMS5003_checksum(void) {

  uint16_t checksum =
      PMS5003_START_BYTE0 +
      PMS5003_START_BYTE1; // starting bytes are included in checksum

  /* The last 2 bytes are not checked in checksum  */
  for (uint8_t i = 0; i < sizeof(PMS5003_raw_data) - 2; i++) {
    checksum += PMS5003_raw_data[i];
  }

  /* Convert 16 bit value to high and low 8-bit versions  */
  uint8_t checksum_low = checksum & 0xFF;
  uint8_t checksum_high = checksum >> 8;

  /* Is the PMS5003 Alive?*/
  if (checksum_low == PMS5003_Health.prev_checksum_low &&
      checksum_high == PMS5003_Health.prev_checksum_high) {
    PMS5003_Health.reads_without_change++;
  } else {
    PMS5003_Health.reads_without_change = 0;
    PMS5003_Health.prev_checksum_high = checksum_high;
    PMS5003_Health.prev_checksum_low = checksum_low;
  }

  /* Compare with checksum values on PMS5003 with calculated checksum.
  If they do not match, return error. */
  if (PMS5003_raw_data[RAW_CHECK_BIT_HIGH] == checksum_high &&
      PMS5003_raw_data[RAW_CHECK_BIT_LOW] == checksum_low) {
    return PMS5003_OK;
  } else {
    // PMS5003_data_ok = 0;
    SEGGER_RTT_printf(0, "\r\n ERROR: Checksums do not match");
    return PMS5003_CHECKSUM_ERROR;
  }
}
