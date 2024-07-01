#include <Arduino.h>
#include "config.h"
#include "ARGBLibrary.h"
#include "easyvoice.h"
#include "FreeRTOS_Tasks.h"

/**
   All components are controlled or enabled by "config.h". Before running,
   review and change all configurations based on your setup.

   There's no need to change any of the following code or functions.
*/

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

  SetupFreeRTOS();
}

void loop()
{
  
}
