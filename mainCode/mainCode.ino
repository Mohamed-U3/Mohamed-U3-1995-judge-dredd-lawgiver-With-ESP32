#include <Arduino.h>
#include "config.h"
#include "ARGBLibrary.h"
#include "easyvoice.h"
#include "easyaudio.h"


/**
   All components are controlled or enabled by "config.h". Before running,
   review and change all configurations based on your setup.

   There's no need to change any of the following code or functions.
*/
// Audio setup
EasyAudio audio(AUDIO_RX_PIN, AUDIO_TX_PIN);

// VR module
EasyVoice<VOICE_CMD_ARR, VOICE_CMD_ARR_SZ> voice(VOICE_RX_PIN, VOICE_TX_PIN);

void playSelectedTrack(uint8_t trackIdx);
uint8_t getSelectedTrack(uint8_t idx);
volatile uint8_t selectedAmmoMode = VR_CMD_AMMO_MODE_FMJ; // sets the ammo mode to start

#include "FreeRTOS_Tasks.h"

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) { }
  
  // init the voice recognition module
  voice.begin();

  // initializes the audio player and sets the volume
  uint8_t loopStage = LOOP_STATE_START;
  int bootAttempts = 0;
  while (bootAttempts < 3 && !audio.begin(15))
  {
    bootAttempts++;
    delay(3000);
  }
  if (bootAttempts == 3)
    loopStage = LOOP_STATE_ERROR;
  
  //Initializing LED of Muzzle Flash
  setupmuzzleFlash(255); //the Brightness -> 255 max.
  //Initializing LED of Front that indicate the ammo
  setupFrontStrips(255); //the Brightness -> 255 max.

  //Butttons configration
  pinMode(TRIGGER_PIN,INPUT);
  pinMode(RELOAD_PIN,INPUT);

  checkButtonAtStartup(2000);
  while(digitalRead(TRIGGER_PIN) == HIGH)
  {
    Serial.println("Button still pressed plz release it");
  }
  delay(500); // for debuncing 
  SetupFreeRTOS();
}

void loop()
{
}

//************Start*************//Audio of DFPlayer//*************************Start*********************//Audio of DFPlayer//***************Start*****************//
///////////////////Start OF Code that is related to Playing Audio of DFPlayer////////////Start OF Code that is related to Playing Audio of DFPlayer/////////////////

/**
 * Convenience method for playing a track and specifying the wait time (delay)
 */
void playSelectedTrack(uint8_t trackIdx)
{
  if (selectedAmmoMode == VR_CMD_AMMO_MODE_RAPID && trackIdx == AMMO_MODE_IDX_FIRE)
    audio.playTrack(getSelectedTrack(trackIdx), 200L); // give the rapid fire a little extra time
  else
    audio.playTrack(getSelectedTrack(trackIdx));
}
/**
 *  Convenience method for selecting a track to playback based on the selected
 *  trigger mode and a state variable
 */
uint8_t getSelectedTrack(uint8_t trackIdx)
{
  return AUDIO_TRACK_AMMO_MODE_ARR[selectedAmmoMode][trackIdx];
}
///////////////////END OF Code that is related to Playing Audio of DFPlayer////////////////END OF Code that is related to Playing Audio of DFPlayer/////////////////
//********END*************//Audio of DFPlayer//********************************END*******************************//Audio of DFPlayer//*********END****************//

//*********Start************//StartUP Check//***********************Start*************************************//StartUP Check//*************Start*****************//
///////////////////Start OF Code Of the StartUP Check For ID Checking///////////////////Start OF Code Of the StartUP Check For ID Checking//////////////////////////

void checkButtonAtStartup(uint32_t BUTTON_HOLD_THRESHOLD_SETUP)
{
  uint32_t buttonPressStartTime = 0;
  bool isButtonPressed = false;

  while (true)
  {
    if (digitalRead(TRIGGER_PIN) == HIGH)
    {
      if (!isButtonPressed)
      {
        // Button press detected
        buttonPressStartTime = millis();
        isButtonPressed = true;
      }
      else
      {
        // Check if the button is held for the threshold duration
        if ((millis() - buttonPressStartTime) > BUTTON_HOLD_THRESHOLD_SETUP)
        {
          // Button held for 5 seconds
          doActionA();
          return;
        }
      }
    }
    else
    {
      //Elss means we failed to check or Failed to press and Hold 5 seconds and there is 2 scenarios 
      if (isButtonPressed)       // scenarios 1: Button was pressed but not held for 5 seconds
      {
        // Button was pressed but not held for 5 seconds
        doActionB();
        isButtonPressed = false;
      }
      else                      // scenarios 2: Button was not pressed at all
      {
        // Wait for the button to be clicked again
        delay(10); // Small delay to avoid busy-waiting
      }
    }
  }
}

void doActionA()    // Action to perform when the button is held for 5 seconds
{
  //- start up success and start up sounds
  audio.playTrack(AUDIO_TRACK_ID_OK);
  All_LEDs_is_Blue();
  Serial.println("- start up success and start up sounds -> ID Ok");
}

void doActionB()    // Action to perform when the button is not held for 5 seconds
{
  // start up fail, all leds flash blue and fail sound.
  audio.playTrack(AUDIO_TRACK_ID_FAIL);
  All_LEDs_Flashes_Blue();
  Serial.println("- start up fail, all leds flash blue and fail sound. -> ID fail");
}
///////////////////End OF Code Of the StartUP Check For ID Checking///////////////////////End OF Code Of the StartUP Check For ID Checking//////////////////////////
//**********END*************//StartUP Check//************************END**************************************//StartUP Check//*************END*******************//
