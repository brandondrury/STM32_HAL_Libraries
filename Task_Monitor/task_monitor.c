
#include "task_monitor.h"
#include "FreeRTOS.h"
#include "SEGGER_RTT.h"
#include "portable.h"
#include "portmacro.h"
#include "task.h"


#include "projdefs.h"
#include <stdint.h>

void TaskMonitor(void *argument) {
  vTaskDelay(pdMS_TO_TICKS(5000));

  while (1) {
    UBaseType_t taskCount = uxTaskGetNumberOfTasks();
    TaskStatus_t *taskArray = pvPortMalloc(taskCount * sizeof(TaskStatus_t));

    if (taskArray == NULL) {
      SEGGER_RTT_printf(0, "Monitor: malloc failed\r\n");
    } else {
      uint32_t totalRunTime;
      UBaseType_t filled =
          uxTaskGetSystemState(taskArray, taskCount, &totalRunTime);

      SEGGER_RTT_printf(0, "\r\n\r\n--- Task Status ---\r\n");
      for (UBaseType_t i = 0; i < filled; i++) {
        SEGGER_RTT_printf(
            0, "%-16s Pri:%-2lu HWM:%-5lu State:%d\r\n",
            taskArray[i].pcTaskName, taskArray[i].uxCurrentPriority,
            taskArray[i].usStackHighWaterMark, taskArray[i].eCurrentState);
      }
      SEGGER_RTT_printf(0, "Free heap: %lu bytes\r\n", xPortGetFreeHeapSize());

      vPortFree(taskArray);
    }
    vTaskDelay(pdMS_TO_TICKS(30000));
  }
}