

## PMS5003 In Blocking Mode
Library written in STM32 HAL C code.  

## Description

Meat-n-taters (simple) PMS5003 sensor library for returning the PM1.0, PM2.5, and PM10 concentrations from. These are Data1, Data2, and Data3 in the datasheet and use the CF=1 approach. There are multiple levels of validations including checksum, range validation, and checking for a "frozen" sensor that is "stuck" sending the same data over and over.  

The only function to call from main.c or equivalent is PMS5003_Measurement.  Use your uart instance as an argument.  If successful, the function will update the DustSensor structure values at:

 - DustSensor.PM1_0_Concentration
 - DustSensor.PM2_5_Concentration
 - DustSensor.PM10_Concentration

These variables are available in main.c.

## In Use

In main.c heading

    #include "PMS5003.h"
    
    #include "SEGGER_RTT.h"
    
    extern PMS5003_Data_t DustSensor;


In main loop or equivalent.

    uint8_t status;
    
    status = PMS5003_Measurement(huart2);
    
      
    
    if (status == PMS5003_OK){
    
      SEGGER_RTT_printf(0, "\r\n PM1.0:, %d", DustSensor.PM1_0_Concentration);
    
      SEGGER_RTT_printf(0, "\r\n PM2.5:, %d", DustSensor.PM2_5_Concentration);
    
      SEGGER_RTT_printf(0, "\r\n PM10:, %d \r\n", DustSensor.PM10_Concentration);
    
    }else {
      SEGGER_RTT_printf(0, "\r\n PMS5003 ERROR: Code: %d", status);
    }
  

## Errors

If errors are returned, see PMS5003_Measurement. This function should explain all checks.

## Hardware

 - The pin1 labeling in the datasheet is opposite of Kicad for MPN: 0530480810 using the Molex_PicoBlade_53048-0810_1x08_P1.25mm_Horizontal footprint from the Kicad library. I had to flip the part over so that it's on the bottom side of my PCB, but it does work. Using PTH saved my azz on this one.

 - The PMS5003 seems to fluctuate between pulling 40-60mA based on the crude meter on my bench power supply. I'd consider adding some additional capacitance and factor that in to your board's current usage if using a custom PCB.
