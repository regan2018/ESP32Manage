//多线程工具类

#ifndef MULTITHREADING_H
#define MULTITHREADING_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t taskHandle;

void createTask(TaskFunction_t task, const char* taskName, uint32_t stackSize, void* parameter, UBaseType_t priority){
    xTaskCreate(task, taskName, stackSize, parameter, priority, &taskHandle);
}
void delayTask(uint32_t milliseconds){
    vTaskDelay(milliseconds / portTICK_PERIOD_MS);
}
void deleteTask(){
    vTaskDelete(taskHandle);
}


#endif