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

  //Butttons configration
  pinMode(TRIGGER_PIN,INPUT_PULLUP);
  pinMode(RELOAD_PIN,INPUT_PULLUP);

  SetupFreeRTOS();
}

void loop()
{
}

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
