#ifndef TASKS_H
#define TASKS_H

#include "stdio.h"
#include "hardware/pwm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern SemaphoreHandle_t xDisplayMut;
extern SemaphoreHandle_t xStopSem;
extern SemaphoreHandle_t xADCMutex;

void vTaskWiFi(void *params);
void vTaskSensors(void *params);
void vTaskAlert(void *params);
void vTaskDisplay(void *params);

#endif