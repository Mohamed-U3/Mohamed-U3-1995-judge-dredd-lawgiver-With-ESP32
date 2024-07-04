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
    if (digitalRead(TRIGGER_PIN) == HIGH) //If here is for checking if it holded or just clicked
    {
      if (!isButtonPressed) //No action is done here
      {
        // Button press detected
        buttonPressStartTime = xTaskGetTickCount();
        isButtonPressed = true;
        isButtonHeld = false;
      }
      else
      {
        // Check if the button is held and do some actions
        if (!isButtonHeld && (xTaskGetTickCount() - buttonPressStartTime) > (BUTTON_HOLD_THRESHOLD / portTICK_PERIOD_MS)) //action is done here
        {
          isButtonHeld = true;
          // Do action for button hold
          //change ammo type for now (for testing)
          selectedAmmoMode++;
          if (selectedAmmoMode == 6) selectedAmmoMode = 0;
          playSelectedTrack(AMMO_MODE_IDX_CHGE);
          Serial.print("ammo:");
          Serial.println(selectedAmmoMode);
        }
      }
    }
    else //Else here is For checking the result of clicking only and then do some actions (After release but didn't reach hold time)
    {
      if (isButtonPressed)
      {
        //action is done here
        if (!isButtonHeld) // Button is just clicked not holded
        {
          if (ammo_counters[selectedAmmoMode] > 0)
          {
            // Do action for button click
            playSelectedTrack(AMMO_MODE_IDX_FIRE);
            muzzleFlash(flashColorGreen, 3);
            ammo_counters[selectedAmmoMode] -= 1;
          }
          else
          {
            playSelectedTrack(AMMO_MODE_IDX_EMTY);
          }
          Serial.print("Ammo Count: ");
          Serial.println(ammo_counters[selectedAmmoMode]);
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
    if (digitalRead(RELOAD_PIN) == HIGH)
    {
      if (selectedAmmoMode == VR_CMD_AMMO_MODE_FMJ || selectedAmmoMode == VR_CMD_AMMO_MODE_RAPID)
      {
        ammo_counters[selectedAmmoMode] = 20;
      }
      else
      {
        ammo_counters[selectedAmmoMode] = 5;
      }
      audio.playTrack(AUDIO_TRACK_AMMO_RELOAD);
      muzzleFlash(flashColorBlue, 2);
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
    Serial.print("selectedAmmoMode: ");
    Serial.println(selectedAmmoMode);
    //    for (signed char i = 20; i >= 0 ; i--)
    //    {
    //      FS_LED_Animation4FMJ(i, flashColorRed);
    //      vTaskDelay(1000 / portTICK_PERIOD_MS);
    //    }
    //    All_LEDs_is_Blue();
    //    for(int i=1; i<21;i++)
    //    {
    //      Serial.print("AUDIO: ");
    //      Serial.println(i);
    //      audio.playTrack(i);
    //      vTaskDelay(3000 / portTICK_PERIOD_MS);
    //    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
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
