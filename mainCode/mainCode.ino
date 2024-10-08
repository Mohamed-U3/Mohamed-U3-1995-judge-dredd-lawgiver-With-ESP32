#include <Arduino.h>
#include "config.h"
#include "ARGBLibrary.h"
#include "easyvoice.h"
#include "easyaudio.h"
#include <Ticker.h>

Ticker ticker;
volatile bool tickFlag = false;

void tickHandler()
{
  tickFlag = true;
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

void playSelectedTrack(uint8_t trackIdx);
uint8_t getSelectedTrack(uint8_t idx);
void checkVoiceCommands(void);
void changeAmmoMode(int Mode);
//Variable related to Ammunition
volatile uint8_t selectedAmmoMode = VR_CMD_AMMO_MODE_FMJ; // sets the ammo mode to start
uint8_t ammo_counters[6] = {5, 5, 5, 5, 20, 20}; //Ammunition types in order ->  #0- Grenade   #1- Armor Piercing    #2- Double Whammy    #3- Signal Flare    #4-FMJ      #5-RAPID
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
  while (bootAttempts < 3 && !audio.begin(20))
  {
    bootAttempts++;
    delay(3000);
  }
  audio.playTrack(AUDIO_TRACK_SILENCE);
  delay(1000);  
  if (bootAttempts == 3)
    loopStage = LOOP_STATE_ERROR;
  
  //Initializing LED of Muzzle Flash
  setupmuzzleFlash(255); //the Brightness -> 255 max.
  //Initializing LED of Front that indicate the ammo
  setupFrontStrips(255); //the Brightness -> 255 max.
  //Initializing LED of Rear that indicate the ammo
  setupRearStrips(255); //the Brightness -> 255 max.

  ticker.attach(3.0, tickHandler);  // Call tickHandler every 1 second

  //Butttons configration
  pinMode(TRIGGER_PIN,INPUT);
  pinMode(RELOAD_PIN,INPUT);
  pinMode(FLASH_BUTTON_PIN,INPUT);
  audio.playTrack(AUDIO_TRACK_DNA_CHK);
  turnOnFrontLEDs_1by1(5, flashColorRed);

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
          // Button held for 2 seconds
          doActionA();
          turnOffRearLEDsRed();
          turnOnRearLEDsBlue();
          return;
        }
      }
    }
    else
    {
      turnOnRearLEDsRed();
      turnOffRearLEDsBlue();
      //Elss means we failed to check or Failed to press and Hold 2 seconds and there is 2 scenarios 
      if (isButtonPressed)       // scenarios 1: Button was pressed but not held for 5 seconds
      {
        // Button was pressed but not held for 5 seconds
        doActionB();
        isButtonPressed = false;
      }
      else                      // scenarios 2: Button was not pressed at all
      {
        // Wait for the button to be clicked again
        doActionC();
        delay(10); // Small delay to avoid busy-waiting
      }
    }
    turnOnFrontLEDS(5, flashColorRed);
  }
}

void doActionA()    // Action to perform when the button is held for 5 seconds
{
  //- start up success and start up sounds
  audio.playTrack(AUDIO_TRACK_ID_OK);
//  All_LEDs_is_Blue();
  Serial.println("- start up success and start up sounds -> ID Ok");
}

void doActionB()    // Action to perform when the button is not held for 5 seconds
{
  // start up fail, all leds flash blue and fail sound.
  audio.playTrack(AUDIO_TRACK_ID_FAIL);
  All_LEDs_Flashes_Blue();
  Serial.println("- start up fail, all leds flash blue and fail sound. -> ID fail");
}

void doActionC()    // Action to perform when the button is not held for 5 seconds
{
  // start up fail, button not pressed all leds flash blue and fail sound.
  if (tickFlag)
  {
    tickFlag = false;
    audio.playTrack(AUDIO_TRACK_ID_FAIL);
  }
  All_LEDs_Flashes_Blue2();
  Serial.println("- start up fail, all leds flash blue and fail sound. -> ID fail");
}
///////////////////End OF Code Of the StartUP Check For ID Checking///////////////////////End OF Code Of the StartUP Check For ID Checking//////////////////////////
//**********END*************//StartUP Check//************************END**************************************//StartUP Check//*************END*******************//


//*********Start*********//VoiceCommands//**************************Start************************************//VoiceCommands//**************Start*****************//
///////////////////Start OF Code Of the VoiceCommands and Change Ammo Types ///////////Start OF Code Of the VoiceCommands and Change Ammo Types ////////////////////
/**
 *  Checks the voice recognition module for new voice commands
 *  1. change the selected ammo mode
 *  2. initialize the LED sequence
 *  3. queue playback
 *. 4. set screen refresh
 */
void checkVoiceCommands(void)
{
  int cmd = voice.readCommand();
  static bool FlashFlag = false;
  if (cmd > -1 || digitalRead(FLASH_BUTTON_PIN) == HIGH)
  {
    if(cmd == 6 || digitalRead(FLASH_BUTTON_PIN) == HIGH) //if the command is number 6 (flash ring2)
    {
      if(FlashFlag == false)  //if the flash is OFF
      {
        turnOnRing2LEDS(7, flashColorWhite);  //turn it ON
        FlashFlag = true;                     //change the flag to ON
      }
      else                    //if the flash is ON
      {
        turnOffRing2LEDS();   //turn it OFF
        FlashFlag = false;    //change the flag to OFF
      }
      vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay to debounce button 
    }
    else changeAmmoMode(cmd);
  }
}

/**
 *  1. Set the selected ammo mode
 *  2. Initialize the LED sequence
 *  3. Queue playback
 *  4. Set screen refresh
 */
void changeAmmoMode(int Mode)
{
  if (Mode > -1 && Mode < 6)
  {
    selectedAmmoMode = Mode;
    // Check for Switching modes
    switch (selectedAmmoMode)
    {
      case VR_CMD_AMMO_MODE_GRENADE:    /* Action */      break;  //"Grenade"
      case VR_CMD_AMMO_MODE_AP:         /* Action */      break;  //"Armor Piercing" or just "Armor"
      case VR_CMD_AMMO_MODE_DW:         /* Action */      break;  //"Double Whammy"
      case VR_CMD_AMMO_MODE_SF:         /* Action */      break;  //"Signal Flare"
      case VR_CMD_AMMO_MODE_FMJ:        /* Action */      break;  //"Full Metal"
      case VR_CMD_AMMO_MODE_RAPID:      /* Action */      break;  //"Rapid"
      default:                          /* Action */      break;
    }
    
    playSelectedTrack(AMMO_MODE_IDX_CHGE);
  }
}
/////////////////// END  OF Code Of the VoiceCommands and Change Ammo Types /////////// END  OF Code Of the VoiceCommands and Change Ammo Types ////////////////////
//*********END***********//VoiceCommands//**************************END**************************************//VoiceCommands//**************END*******************//
