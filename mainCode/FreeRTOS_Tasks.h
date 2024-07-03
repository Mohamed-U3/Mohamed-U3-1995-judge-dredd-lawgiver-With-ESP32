#ifndef FREERTOS_TASKS_H
#define FREERTOS_TASKS_H

//FreeRTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// Semaphore handle
SemaphoreHandle_t semaphore_Trigger_button;
SemaphoreHandle_t semaphore_Reload_button;
SemaphoreHandle_t mutex_Trigger_button;
SemaphoreHandle_t mutex_Reload_button;

// Task handles
TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;

// Constants for detecting button click or hold
const static uint32_t BUTTON_HOLD_THRESHOLD = 1000; // Time in milliseconds to consider the button as held
const static uint32_t DEBOUNCE_DELAY = 50;          // Debounce delay in milliseconds

void TaskOfTriggerButton(void * parameter)
{
  uint32_t buttonPressStartTime = 0;
  bool isButtonPressed = false;
  bool isButtonHeld = false;

  for (;;)
  {
    if (digitalRead(TRIGGER_PIN) == LOW)
    {
      if (!isButtonPressed)
      {
        // Button press detected
        buttonPressStartTime = xTaskGetTickCount();
        isButtonPressed = true;
        isButtonHeld = false;
      }
      else
      {
        // Check if the button is held
        if (!isButtonHeld && (xTaskGetTickCount() - buttonPressStartTime) > (BUTTON_HOLD_THRESHOLD / portTICK_PERIOD_MS))
        {
          isButtonHeld = true;
          // Do action for button hold
          playSelectedTrack(AMMO_MODE_IDX_FIRE);
          muzzleFlash(flashColorRed, 5);
        }
      }
    }
    else
    {
      if (isButtonPressed)
      {
        if (!isButtonHeld)
        {
          // Do action for button click
          playSelectedTrack(AMMO_MODE_IDX_FIRE);
          muzzleFlash(flashColorGreen, 3);
        }
        // Reset button state
        isButtonPressed = false;
        isButtonHeld = false;
        vTaskDelay(DEBOUNCE_DELAY / portTICK_PERIOD_MS);  // Delay to debounce button
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);  // Delay to avoid busy-waiting
  }
}


// TaskOfReloadButton: Check button state then do action
void TaskOfReloadButton(void * parameter)
{
  for (;;)
  {
    if (digitalRead(RELOAD_PIN) == LOW)
    {
      selectedAmmoMode++;
      if (selectedAmmoMode == 7) selectedAmmoMode = 0;
      Serial.print("ammo:");
      Serial.println(selectedAmmoMode);
      muzzleFlash(flashColorBlue, 2);
      //      if (xSemaphoreTake(mutex_Reload_button, 0) == pdTRUE)
      //      {
      //        xSemaphoreGive(semaphore_Reload_button);  // Give the semaphore when button is pressed
      //      }
    }
    else
    {
      vTaskDelay(10 / portTICK_PERIOD_MS);  // Delay to debounce button
    }
  }
}

// Task 2:
void Task2code(void * parameter)
{
  for (;;)
  {
    Serial.println("Task2");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    //    if (xSemaphoreTake(semaphore_Reload_button, 0) == pdTRUE)
    //    {
    //      muzzleFlash(flashColorBlue, 2);
    //      xSemaphoreGive(mutex_Reload_button);
    //    }
    //
    //    if (xSemaphoreTake(semaphore_Trigger_button, 0) == pdTRUE)
    //    {
    //      muzzleFlash(flashColorGreen, 3);
    //      xSemaphoreGive(mutex_Trigger_button);
    //    }
  }
}

void SetupFreeRTOS()
{
  //FreeRTOS configration
  // Create binary semaphore
  semaphore_Trigger_button = xSemaphoreCreateBinary();
  semaphore_Reload_button = xSemaphoreCreateBinary();
  mutex_Trigger_button = xSemaphoreCreateMutex();
  mutex_Reload_button = xSemaphoreCreateMutex();
  if (semaphore_Trigger_button == NULL)
  {
    Serial.println("Failed to create semaphore_Trigger_button");
    while (1); // Halt if semaphore creation fails
  }
  if (semaphore_Reload_button == NULL)
  {
    Serial.println("Failed to create semaphore_Reload_button");
    while (1); // Halt if semaphore creation fails
  }

  // Create tasks
  xTaskCreatePinnedToCore(
    TaskOfTriggerButton,    // Function to be called
    "Task1",                // Name of task
    5000,                  // Stack size (bytes)
    NULL,                   // Parameter to pass
    3,                      // Task priority
    &Task1,                 // Task handle
    0);                     // Core to run the task on (0 or 1)

  xTaskCreatePinnedToCore(
    Task2code,
    "Task2",
    10000,
    NULL,
    1,
    &Task2,
    1);

  xTaskCreatePinnedToCore(
    TaskOfReloadButton,    // Function to be called
    "Task3",                // Name of task
    5000,                  // Stack size (bytes)
    NULL,                   // Parameter to pass
    2,                      // Task priority
    &Task3,                 // Task handle
    0);                     // Core to run the task on (0 or 1)
}

#endif //FREERTOS_TASKS_H
