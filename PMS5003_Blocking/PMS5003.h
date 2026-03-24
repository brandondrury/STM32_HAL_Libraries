#ifndef PMS5003
#define PMS5003

#include "main.h"
#include <stdint.h>


#define PM1_0_con_index_high  2
#define PM1_0_con_index_low  3
#define PM2_5_con_index_high  4
#define PM2_5_con_index_low  5
#define PM10_con_index_high  6
#define PM10_con_index_low  7
#define RAW_CHECK_BIT_HIGH  28
#define RAW_CHECK_BIT_LOW  29

#define PMS5003_START_BYTE0  0x42
#define PMS5003_START_BYTE1  0x4D

#define MAX_CONCENTRATION_VALUE 1000
#define MIN_CONCENTRATION_VALUE 0

#define MAX_READS_WITHOUT_CHANGE_THRESH 30 


typedef struct {
  uint16_t PM1_0_Concentration;
  uint16_t PM2_5_Concentration;
  uint16_t PM10_Concentration;  
} PMS5003_Data_t;



// typedef struct {
//     PMS5003_Data_t  raw;
//     uint8_t filter_on;
//     uint8_t consecutive_high;  // hysteresis counter
//     uint8_t consecutive_low;   
// } DustSensor_t;

typedef struct {
    uint8_t prev_checksum_high;
    uint8_t prev_checksum_low;
    uint8_t reads_without_change;
} PMS5003_Monitor_t;


/* Error Codes for PMS5003
PMS5003_OK -> All good.
PMS5003_CHECKSUM_ERROR -> Checksum calculated from the reading does not match the checksum provided by sensor on last 2 bytes.
PMS5003_ZERO_READING -> Sensor returns zeros for all 3 concentration values.
PMS5003_froze_READING -> Sensor values are not changing/updating
PMS5003_VALUE_BOUNDS_ERROR -> Sensor values are outside the expected bounds.

*/
typedef enum
{
  PMS5003_OK       = 0,
  PMS5003_CHECKSUM_ERROR    = 1,
  PMS5003_ZERO_READING_ERROR = 2,
  PMS5003_froze_READING = 3,
  PMS5003_VALUE_BOUNDS_ERROR = 4,
  PMS5003_DEAD = 5,
  PMS5003_UART_READ_ERROR = 6,
  PMS5003_FROZE = 7
  


} Status_PMS5003_t;




/* Main function of the library.  Contains calls to all the other big functions in sequence.*/
Status_PMS5003_t PMS5003_Measurement(UART_HandleTypeDef huart);


/* Read serial data from PMS5003*/
Status_PMS5003_t PMS5003_read_data(UART_HandleTypeDef huart);

/* Grab the PM1.0, PM2.5, and PM10 concentrations and save in structure*/
void PMS5003_parse_data(void);

/* Ensure the calculated checksum matches those provided by sensor on last 2 bytes*/
Status_PMS5003_t PMS5003_checksum(void);

/* Ensures sensor values are realistic.*/
Status_PMS5003_t PMS5003_data_plausible(void);

/* Checks for the case of all sensor data being zero.*/
Status_PMS5003_t PMS5003_all_zero_check(void);

/* Does the sensor return the exact same checksum MAX_READS_WITHOUT_CHANGE_THRESH times in a row?
If so, return PMS5003_FROZE */
Status_PMS5003_t PMS5003_froze_sensor(void);

#endif