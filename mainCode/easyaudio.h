/* 
 *  This File depends on some Libraries all are found in Library manager of Arduino IDE
 *  1- SoftwareSerial.h named EspSoftwareSerial by Dirk Kaar Version 8.1.0
 *  2- DFPlayerMini_Fast.h by Powerbroker2        Version 1.2.4 >> For (DFplayer Mini)
 *  3- DFRobot_DF1201S.h   by fengli and qsjhyy   Version 1.0.2 >> this is the official library of (DFplayer pro)
 */
#ifndef easyaudio_h
#define easyaudio_h

#define MINI_BAUD_RATE 9600
#define PRO_BAUD_RATE 115200
// uncomment if you are using the DFPlayer Pro
#define ENABLE_EASY_AUDIO_PRO

#ifdef ENABLE_EASY_AUDIO_PRO
#include <DFRobot_DF1201S.h>
#define _mySerial Serial1
#else
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
#endif

/**
   EasyAudio is based on DF Player components that provide simple setup and easy track playback.

   Constructor takes rx and tx pins as inputs, but will default to 0 and 1.
   eg: EasyAudio audio(0, 1);

   Call the begin function to initialize the serial comms, and set the volume.
   Volume range is 0 - 30, default is 25.
   eg.audio.begin(15);

   Playback is by track index. The track index is determined by the order in which
   the files are loaded/copied onto the SD card.
   eg. audio.playTrackNow(1);

   When using in main loop, better to queue tracks for playback and eliminate using delays.
   eg. audio.queuePlayback(1);

   In the main loop, playback the next queued track:
   eg. audio.playQueuedTrack();
*/
class EasyAudio {
  private:

#ifdef ENABLE_EASY_AUDIO_PRO
    DFRobot_DF1201S DF1201S;
#else
    SoftwareSerial _mySerial;
    DFPlayerMini_Fast _player; // set to true if you have a chip variant of the DF PlayerMini
#endif
    unsigned long _lastPlaybackTime = 0;
    long _playbackDelay = 100;

  public:
    EasyAudio(uint8_t rxPin, uint8_t txPin) 
      #ifdef ENABLE_EASY_AUDIO_PRO          ///////////Pro Version//////////////Pro Version//////////Pro Version//////////////
      {}
      #else                               ///////////Mini Version///////////////Mini Version///////////////Mini Version////
      : _mySerial(rxPin, txPin) {};
      #endif

    bool begin(uint8_t vol)
    {
#if ENABLE_EASY_AUDIO == 1
      DBGLN(F("setup audio"));
#ifdef ENABLE_EASY_AUDIO_PRO          ///////////Pro Version//////////////Pro Version//////////Pro Version//////////////
      _mySerial.begin(PRO_BAUD_RATE, SERIAL_8N1, /*rx =*/32, /*tx =*/33);

      while (!DF1201S.begin(_mySerial))
      {
        Serial.println("Init failed, please check the wire connection!");
        delay(1000);
      }
      /*Set volume to 20*/
      DF1201S.setVol(/*VOL = */vol);
      Serial.print("VOL:");
      /*Get volume*/
      Serial.println(DF1201S.getVol());
      /*Enter music mode*/
      DF1201S.switchFunction(DF1201S.MUSIC);
      /*Wait for the end of the prompt tone */
      delay(2000);
      /*Set playback mode to "repeat all"*/
      DF1201S.setPlayMode(DF1201S.SINGLE);
      Serial.print("PlayMode:");
      /*Get playback mode*/
      Serial.println(DF1201S.getPlayMode());
#else                               ///////////Mini Version///////////////Mini Version///////////////Mini Version////
      _mySerial.begin(MINI_BAUD_RATE);
      _player.begin(_mySerial, false);  //set Serial for DFPlayer-mini mp3 module
      delay(1000);
      _player.volume(vol);                    //initial volume, 30 is max, 3 makes the wife not angry
#endif
      delay(1000);
#endif
      return true;
    }

    /**
       Poor version of checking playback instead of adding delays.
       THe proper solution would be to check whether the component is busy.
       Our wiring doesn't support it at the moment
    */
    bool isBusy() {
      if (millis() > (_lastPlaybackTime + _playbackDelay)) {
        return false;
      }
      return true;
    }

    /**
       play a track by number.
    */
    void playTrack(int track) {
      playTrack(track, 100);
    }

    /**
       play a track by number, with a specific busy delay
    */
    void playTrack(int track, long busyDelay)
    {
      _playbackDelay = busyDelay;
      _lastPlaybackTime = millis();
#if ENABLE_EASY_AUDIO == 1
#ifdef ENABLE_EASY_AUDIO_PRO

      DF1201S.playFileNum(track);

#else
      uint8_t statuS = false;
      do
      {
        _player.play(track);
        delay(200);
        statuS = _player.isPlaying();
        if (statuS == -1)
        {
          Serial.println(".................................Error Happened");
        }
        else if (statuS == true)
        {
          Serial.println(".................................playing track");
        }
        else if (statuS == false)
        {
          Serial.println(".................................trying again");
        }
      }
      while (statuS == false);
#endif
#endif
    }

    void playTrackAndWait(int track)
    {
      _lastPlaybackTime = millis();
#if ENABLE_EASY_AUDIO == 1
#ifdef ENABLE_EASY_AUDIO_PRO
      DF1201S.playFileNum(track);
#else
      uint8_t statuS = false;
      do
      {
        _player.play(track);
        delay(200);
        statuS = _player.isPlaying();
        if (statuS == -1)
        {
          Serial.println(".................................Error Happened");
        }
        else if (statuS == true)
        {
          Serial.println(".................................playing track");
        }
        else if (statuS == false)
        {
          Serial.println(".................................trying again");
        }
      }
      while (statuS == false);
#endif
      delay(_playbackDelay);
#endif
    }

};
#endif
