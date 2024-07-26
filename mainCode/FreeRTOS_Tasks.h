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
TaskHandle_t Task4;
// Timer handle
TimerHandle_t xShutdownTimer;

//function declaration
void TaskShutdownTimerCallback(TimerHandle_t xTimer);

// Constants for detecting button click or hold
const static uint32_t BUTTON_HOLD_THRESHOLD = 1000; // Time in milliseconds to consider the button as held
const static uint32_t DEBOUNCE_DELAY = 50;          // Debounce delay in milliseconds
volatile bool timerFlag = false;

void TaskOfTriggerButton(void * parameter)
{
  uint32_t buttonPressStartTime = 0;
  bool isButtonPressed = false;
  bool isButtonHeld = false;

  for (;;)
  {
    if (digitalRead(TRIGGER_PIN) == HIGH) //If here is for checking if it holded or just clicked
    {
      xTimerReset(xShutdownTimer, 0); //rest the timer of the inactive function
      if (timerFlag == true)          //if we are at inactive state you pressed the button to go active again
      {
        turnOnFrontLEDs_1by1(5, flashColorRed);// Animation of the front LED strip.
        timerFlag = false;                     //get out of inactive state to active state
        vTaskDelay(500 / portTICK_PERIOD_MS);  // Delay to debounce button
      }
      else                            //if we are at active state
      {
        if (!isButtonPressed)         //if the button was not pressed before (because we are in superloop)
        {
          // Button press detected
          buttonPressStartTime = xTaskGetTickCount();   // Get time for the button has been pressed at
          isButtonPressed = true;                       // make the flag of the button pressed true
          isButtonHeld = false;                         // make the flag of button hold is false
        }
        else                          //if the button is pressed before (because we are in superloop that means the button is pressed and the function is called again but still pressed)
        {
          // Check if the button is held and do some actions
          if (!isButtonHeld && (xTaskGetTickCount() - buttonPressStartTime) > (BUTTON_HOLD_THRESHOLD / portTICK_PERIOD_MS)) //check if the hold of the button exeted the required time.(2second)
          {
            isButtonHeld = true;                        // make the flag of button hold is true
            // Do action for button hold
            selectedAmmoMode++;                         //change ammo type for now (for testing)
            if (selectedAmmoMode >= 6) selectedAmmoMode = 0;
            playSelectedTrack(AMMO_MODE_IDX_CHGE);      //play sound of mode change
            Serial.print("ammo:");                      //print the mode type as number form 0 to 6
            Serial.println(selectedAmmoMode);
            vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay to debounce button
          }
        }
      }
    }
    else //(after release)checking if the button is pressed after release but didn't reach hold time.
    {
      if (isButtonPressed)                              // button is pressed
      {
        //action is done here
        if (!isButtonHeld)                              // Button is just clicked not holded for required time.
        {
          if (ammo_counters[selectedAmmoMode] > 0)      // check if there is ammo in the magazine then Do action (Fire action)
          {
            playSelectedTrack(AMMO_MODE_IDX_FIRE);      // play audio related to ammo type
            switch (selectedAmmoMode)                   // make flash effect (muzzleFlash from the ring LED)
            {                
              case VR_CMD_AMMO_MODE_GRENADE:  muzzleFlash(flashColorRed,   2);        break;
              case VR_CMD_AMMO_MODE_AP:       muzzleFlash(flashColorPurple,2);        break;
              case VR_CMD_AMMO_MODE_DW:       muzzleFlash(flashColorOrange,2); 
                                              vTaskDelay(500 / portTICK_PERIOD_MS); 
                                              muzzleFlash(flashColorOrange,2);
                                              playSelectedTrack(AMMO_MODE_IDX_FIRE);  break;
              case VR_CMD_AMMO_MODE_SF:       muzzleFlash(flashColorYellow,2);        break;
              case VR_CMD_AMMO_MODE_FMJ:      muzzleFlash(flashColorWhite, 2);        break;
              case VR_CMD_AMMO_MODE_RAPID:    muzzleFlash(flashColorWhite, 10);       break;
            }
            ammo_counters[selectedAmmoMode] -= 1;       // decressed the ammo counter
          }
          else                                          // if the magazine is empty.
          {
            playSelectedTrack(AMMO_MODE_IDX_EMTY);      // play audio of empty magazine.
          }
          Serial.print("Ammo Count: ");                 // print ammo count.
          Serial.println(ammo_counters[selectedAmmoMode]);
        }
        isButtonPressed = false;                        // Reset button state
        isButtonHeld = false;                           // Reset button state
        vTaskDelay(DEBOUNCE_DELAY / portTICK_PERIOD_MS);  // Delay to debounce button
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);                // Delay to avoid busy-waiting
  }
}


// TaskOfReloadButton: Check button state then do action
void TaskOfReloadButton(void * parameter)
{
  for (;;)
  {
    if (digitalRead(RELOAD_PIN) == HIGH)    //if button of reload is pressed
    {
      xTimerReset(xShutdownTimer, 0);       //rest the timer of the inactive function
      if (timerFlag == true)                //if we are at inactive state
      {
        turnOnFrontLEDs_1by1(5, flashColorRed);// Animation of the front LED strip.
        timerFlag = false;                     //get out of inactive state to active state
        vTaskDelay(500 / portTICK_PERIOD_MS);  // Delay to debounce button
      }
      else                                  //if we are at active state.
      { //do reload action.
        if (selectedAmmoMode == VR_CMD_AMMO_MODE_FMJ || selectedAmmoMode == VR_CMD_AMMO_MODE_RAPID) //if ammo types is RAPID or FMJ
        {
          ammo_counters[selectedAmmoMode] = 20;             // reset counter with 20
        }
        else                                                                                        //if ammo is any other type
        {
          ammo_counters[selectedAmmoMode] = 5;              // reset counter with 5
        }
        audio.playTrack(AUDIO_TRACK_AMMO_RELOAD);           // play audio of reload
        vTaskDelay(DEBOUNCE_DELAY / portTICK_PERIOD_MS);    // Delay to debounce audio
      }
    }
    else                                    //if is not pressed
    {
      vTaskDelay(10 / portTICK_PERIOD_MS);  // Delay to avoid busy-waiting
    }
  }
}

// Task 2:
void TaskOfFrontStripAmmoCounter(void * parameter)
{
  for (;;)
  {
    if (timerFlag == false)                     //if we are at active state.
    {
      Serial.print("selectedAmmoMode: ");       //print selected ammo type as number from 0 to 6
      Serial.println(selectedAmmoMode);
      turnOnRearLEDsBlue();                     // turn on rear LEDs Cells as one for "Red" and the other is "Blue".
      if (selectedAmmoMode == VR_CMD_AMMO_MODE_FMJ || selectedAmmoMode == VR_CMD_AMMO_MODE_RAPID)           //check if the ammo is FMJ or Rapid
      {
        FS_LED_Animation4FMJ(ammo_counters[selectedAmmoMode], flashColorRed);       //refresh the Front LED strip with the ammo count with certain animation.
      }
      else                                                                                                  //if the selected ammo is any other type
      {
        turnOnFrontLEDS(ammo_counters[selectedAmmoMode], flashColorRed);            //refresh the Front LED strip with the ammo count.
      }
      if (ammo_counters[selectedAmmoMode] != 0)  turnOffRearLEDsRed();
      else                                      turnOnRearLEDsRed();
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);                                           // do it again every half second + Delay to avoid busy-waiting
  }
}

// Task 4:
void TaskOfVoiceRecognitionChecking(void * parameter)
{
  for (;;)
  {
    if (timerFlag == false)                   //if we are at active state.
    {
      checkVoiceCommands();                   //Check on Voice commands and change the ammo type if there is a command.
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);      // Delay to avoid busy-waiting
  }
}

void TaskShutdownTimerCallback(TimerHandle_t xTimer)
{
  // No button pressed for 2 minutes
  if ( timerFlag == false)                    //if we are at active state.
  {
    timerFlag = true;                         //get out of active state to inactive state
    vTaskDelay(500 / portTICK_PERIOD_MS);     // Delay to debounce
  }
  fadeOutALLwithDelay(50);                    // All LEDs makes fade animation till all are off.
  //  xTimerReset(xShutdownTimer, 0);
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
    TaskOfFrontStripAmmoCounter,
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

  xTaskCreatePinnedToCore(
    TaskOfVoiceRecognitionChecking, // Function to be called
    "Task3",                        // Name of task
    5000,                           // Stack size (bytes)
    NULL,                           // Parameter to pass
    2,                              // Task priority
    &Task4,                         // Task handle
    1);                             // Core to run the task on (0 or 1)

  // Create active/inactive timer
  xShutdownTimer = xTimerCreate("ShutdownTimer", pdMS_TO_TICKS(120000), pdFALSE, (void *)0, TaskShutdownTimerCallback);

  // Start the timer
  xTimerStart(xShutdownTimer, 0);
}

#endif //FREERTOS_TASKS_H
