#include <DFPlayerMini_Fast.h>

#include <SoftwareSerial.h>
SoftwareSerial mySerial(32, 33); // RX, TX

DFPlayerMini_Fast myMP3;

void setup()
{
  Serial.begin(115200);

  mySerial.begin(9600);
  myMP3.begin(mySerial, true);
  delay(1000);
  
  
  Serial.println("Setting volume to max");
  myMP3.volume(30);
  delay(1000);
  Serial.println("Looping track 1");
//  myMP3.loop(24);
}

void loop()
{
  for(int i=0 ;i<26;i++)
  {
    myMP3.play(i);
    delay(2000);
  }
  //do nothing
}
