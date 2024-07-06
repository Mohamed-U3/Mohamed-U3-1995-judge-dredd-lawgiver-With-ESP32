#include <SoftwareSerial.h>
#include "dfplayer_pro.h"

// Pin configuration for MP3 Player
#define AUDIO_RX_PIN          32
#define AUDIO_TX_PIN          33

SoftwareSerial mySerial(AUDIO_RX_PIN, AUDIO_TX_PIN);
DFPlayerPro _player;

void setup(void) {
  Serial.begin(115200);
  mySerial.begin(115200);  // factory default is 115200
  while (!_player.begin(mySerial)) {
    Serial.println("Init failed, please check the wire connection or baud rate!");
    delay(1000);
  }

  //Turn on indicator LED (Power-down save)
  Serial.println("Config: Disable LED");
  _player.setLED(false);
  //Turn on the prompt tone (Power-down save)
  Serial.println("Config: Disable Prompt");
  _player.setPrompt(false);
}

void loop() {
  Serial.println("Power Down");
  while (1) delay(2000);
}
