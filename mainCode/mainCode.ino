#include <Arduino.h>
#include "config.h"
#include "ARGBLibrary.h"
#include "easyvoice.h"
#include "easyaudio.h"
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

// Task 1: Check button state
void TaskOfTriggerButton(void * parameter)
{
  for (;;)
  {
    if (digitalRead(TRIGGER_PIN) == LOW)
    {
      if (xSemaphoreTake(mutex_Trigger_button, 0) == pdTRUE)
      {
        xSemaphoreGive(semaphore_Trigger_button);  // Give the semaphore when button is pressed
      }
    }
//    vTaskDelay(1 / portTICK_PERIOD_MS);  // Delay to debounce button
  }
}

void TaskOfReloadButton(void * parameter)
{
  for (;;)
  {
    if (digitalRead(RELOAD_PIN) == LOW)
    {
      if (xSemaphoreTake(mutex_Reload_button, 0) == pdTRUE)
      {
        xSemaphoreGive(semaphore_Reload_button);  // Give the semaphore when button is pressed
      }
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);  // Delay to debounce button
  }
}

// Task 2: Control LED blinking
void Task2code(void * parameter)
{
  for (;;)
  {
    if (xSemaphoreTake(semaphore_Reload_button, 0) == pdTRUE)
    {
      muzzleFlash(flashColorBlue, 2);
      xSemaphoreGive(mutex_Reload_button);
    }

    if (xSemaphoreTake(semaphore_Trigger_button, 0) == pdTRUE)
    {
      muzzleFlash(flashColorGreen, 3);
      xSemaphoreGive(mutex_Trigger_button);
    }
  }
}


/**
   All components are controlled or enabled by "config.h". Before running,
   review and change all configurations based on your setup.

   There's no need to change any of the following code or functions.
*/
// Audio setup
EasyAudio audio(AUDIO_RX_PIN, AUDIO_TX_PIN);

// VR module
EasyVoice<VOICE_CMD_ARR, VOICE_CMD_ARR_SZ> voice(VOICE_RX_PIN, VOICE_TX_PIN);

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) { }
  
  // init the voice recognition module
  voice.begin();

  //Initializing LED of Muzzle Flash
  setupmuzzleFlash(255); //the Brightness -> 255 max.

  //Butttons configration
  pinMode(TRIGGER_PIN,INPUT_PULLUP);
  pinMode(RELOAD_PIN,INPUT_PULLUP);


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
    10000,                  // Stack size (bytes)
    NULL,                   // Parameter to pass
    1,                      // Task priority
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

void loop()
{
  
}
