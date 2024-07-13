#ifndef ARGBLIBRARY_H
#define ARGBLIBRARY_H
#include "config.h"
#include <Adafruit_NeoPixel.h>

#define RING_LED_PIN      MUZZLE_FLASH_LED_PIN
#define RING_NUM_LEDS     MUZZLE_FLASH_LED_CNT
#define F_LED_STRIP_PIN   FRONT_LED_STRIP_PIN
#define F_STRIP_NUM_LEDS  FRONT_LED_STRIP_CNT
#define R_LED_STRIP_PIN   REAR_LED_STRIP_PIN
#define R_STRIP_NUM_LEDS  REAR_LED_STRIP_CNT

// Create an instance of the Adafruit_NeoPixel class
Adafruit_NeoPixel Ring    = Adafruit_NeoPixel(RING_NUM_LEDS,        RING_LED_PIN        , NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel F_Strip = Adafruit_NeoPixel(FRONT_LED_STRIP_CNT , FRONT_LED_STRIP_PIN , NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel R_Strip = Adafruit_NeoPixel(REAR_LED_STRIP_CNT  , REAR_LED_STRIP_PIN  , NEO_GRB + NEO_KHZ800);


// Define the colors for the flash
const uint32_t flashColorRed    = Ring.Color(255, 0, 0);       // Red
const uint32_t flashColorGreen  = Ring.Color(0, 255, 0);       // Green
const uint32_t flashColorBlue   = Ring.Color(0, 0, 255);       // Blue
const uint32_t flashColorYellow = Ring.Color(255, 255, 0);     // Yellow
const uint32_t flashColorPurple = Ring.Color(128, 0, 128);     // Purple
const uint32_t flashColorCyan   = Ring.Color(0, 255, 255);     // Cyan
const uint32_t flashColorWhite  = Ring.Color(255, 255, 255);   // White
const uint32_t flashColorOrange = Ring.Color(255, 165, 0);     // Orange
const uint32_t flashColorPink   = Ring.Color(255, 192, 203);   // Pink
const uint32_t flashColorViolet = Ring.Color(238, 130, 238);   // Violet

void setupmuzzleFlash(uint8_t Brightness)
{
  Ring.begin();            // Initialize the strip
  Ring.show();             // Initialize all pixels to 'off'
  Ring.setBrightness(Brightness); // Set maximum brightness
}

void flashRing(uint32_t color)
{
  // Turn all LEDs to the flash color
  for (int i = 0; i < RING_NUM_LEDS; i++)
  {
    Ring.setPixelColor(i, color);
  }
  Ring.show();
  vTaskDelay(1 / portTICK_PERIOD_MS); // Flash duration
}

void fadeOutRing()
{
  for (int brightness = 255; brightness >= 0; brightness -= 5)
  {
    for (int i = 0; i < RING_NUM_LEDS; i++)
    {
      uint32_t color = Ring.getPixelColor(i);
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = color & 0xFF;
      Ring.setPixelColor(i, Ring.Color(r * brightness / 255, g * brightness / 255, b * brightness / 255));
    }
    Ring.show();
    vTaskDelay(1 / portTICK_PERIOD_MS); // Adjust this for faster or slower fading
  }
}

void muzzleFlash(uint32_t color, uint8_t flashes_count)
{
  for (int i = 0; i < flashes_count; i++)
  { // Repeat n_flashes times
    flashRing(color);
    fadeOutRing();
    vTaskDelay(10 / portTICK_PERIOD_MS); // Short delay between flashes
  }
}

void turnOnRingLEDS(uint8_t numLEDS, uint32_t color)
{
  // Limit numLEDS to the maximum number of LEDs in the strip
  if (numLEDS > RING_NUM_LEDS)
  {
    numLEDS = RING_NUM_LEDS;
  }

  // Clear all LEDs to 'off' (black color)
  for (int i = 0; i < RING_NUM_LEDS; i++)
  {
    Ring.setPixelColor(i, 0);  // Set color of each LED to black (off)
  }
  for (int i = 0; i < numLEDS; i++)
  {
    Ring.setPixelColor(i, color);  // Set color of each LED
  }
  Ring.show();                     // Display the updated colors
}

void turnOffRingLEDS()
{
  // Clear all LEDs to 'off' (black color)
  for (int i = 0; i < RING_NUM_LEDS; i++)
  {
    Ring.setPixelColor(i, 0);  // Set color of each LED to black (off)
  }
  Ring.show();                     // Display the updated colors
}

//****************Start**********************////****************Start**********************////****************Start**********************//
/////////Start Code of Front Strips///////////////////Start Code of Front Strips///////////////////Start Code of Front Strips////////////////
void setupFrontStrips(uint8_t Brightness)
{
  F_Strip.begin();            // Initialize the strip
  F_Strip.show();             // Initialize all pixels to 'off'
  F_Strip.setBrightness(Brightness); // Set maximum brightness
}
////////////////////////////////////////////////////////////////////////////////////////////

void flashFS(uint32_t color)
{
  // Turn all LEDs to the flash color
  for (int i = 0; i < FRONT_LED_STRIP_CNT; i++)
  {
    F_Strip.setPixelColor(i, color);
  }
  F_Strip.show();
  vTaskDelay(1 / portTICK_PERIOD_MS); // Flash duration
}

void fadeOutFS()
{
  for (int brightness = 255; brightness >= 0; brightness -= 5)
  {
    for (int i = 0; i < FRONT_LED_STRIP_CNT; i++)
    {
      uint32_t color = F_Strip.getPixelColor(i);
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = color & 0xFF;
      F_Strip.setPixelColor(i, F_Strip.Color(r * brightness / 255, g * brightness / 255, b * brightness / 255));
    }
    F_Strip.show();
    vTaskDelay(1 / portTICK_PERIOD_MS); // Adjust this for faster or slower fading
  }
}

void FrontStripFlash(uint32_t color, uint8_t flashes_count)
{
  for (int i = 0; i < flashes_count; i++)
  { // Repeat n_flashes times
    flashFS(color);
    fadeOutFS();
    vTaskDelay(10 / portTICK_PERIOD_MS); // Short delay between flashes
  }
}
////////////////////////////////////////////////////////////////////////////////////////////

void turnOnFrontLEDS(uint8_t numLEDS, uint32_t color)
{
  // Limit numLEDS to the maximum number of LEDs in the strip
  if (numLEDS > FRONT_LED_STRIP_CNT)
  {
    numLEDS = FRONT_LED_STRIP_CNT;
  }

  // Clear all LEDs to 'off' (black color)
  for (int i = 0; i < FRONT_LED_STRIP_CNT; i++)
  {
    F_Strip.setPixelColor(i, 0);  // Set color of each LED to black (off)
  }
  for (int i = 0; i < numLEDS; i++)
  {
    F_Strip.setPixelColor(i, color);  // Set color of each LED
  }
  F_Strip.show();                     // Display the updated colors
}

void turnOnFrontLEDs_1by1(uint8_t numLEDS, uint32_t color)
{
  // Limit numLEDS to the maximum number of LEDs in the strip
  if (numLEDS > FRONT_LED_STRIP_CNT)
  {
    numLEDS = FRONT_LED_STRIP_CNT;
  }

  // Clear all LEDs to 'off' (black color)
  for (int i = 0; i < FRONT_LED_STRIP_CNT; i++)
  {
    F_Strip.setPixelColor(i, 0);  // Set color of each LED to black (off)
  }
  for (int i = 0; i < numLEDS; i++)
  {
    F_Strip.setPixelColor(i, color);  // Set color of each LED
    F_Strip.show();                     // Display the updated colors
    vTaskDelay(550 / portTICK_PERIOD_MS);  // Delay to avoid busy-waiting
  }
  F_Strip.show();                     // Display the updated colors
}

void turnOffFrontLEDS()
{
  // Clear all LEDs to 'off' (black color)
  for (int i = 0; i < FRONT_LED_STRIP_CNT; i++)
  {
    F_Strip.setPixelColor(i, 0);  // Set color of each LED to black (off)
  }
  F_Strip.show();                     // Display the updated colors
}

void FS_LED_Animation4FMJ(int fmj, uint32_t color) //same algorithm of updateLEDBrightnessForFMJ() but smaller in the code excution 
{
  int brightness = 0;
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  // Limit fmj to maximum of 20
  if (fmj > 20) fmj = 20;

  int index = (20 - fmj) / 4;  // Determine the group (0-4)
  int step = (20 - fmj) % 4;   // Determine the step within the group (0-3)

  // Determine brightness based on the step within the group
  switch (step)
  {
    case 0: brightness = 255; break;
    case 1: brightness = 192; break;
    case 2: brightness = 128; break;
    case 3: brightness = 64; break;
  }

  // Turn off all LEDs first
  for (int i = 0; i < 5; i++)
  {
    F_Strip.setPixelColor(i, 0);
  }

  // Set the color and brightness for the LEDs based on the group and step
  for (int i = 0; i < 5 - index; i++)
  {
    if (i == 4 - index)
    {
      F_Strip.setPixelColor(i, F_Strip.Color(r * brightness / 255, g * brightness / 255, b * brightness / 255));
    }
    else
    {
      F_Strip.setPixelColor(i, color);
    }
  }

  F_Strip.show();  // Display the updated colors and brightness
}

///////// End  Code of Front Strips/////////////////// End  Code of Front Strips///////////////////End   Code of Front Strips////////////////
//****************End************************////****************End************************////****************End************************//

//****************Start**********************////****************Start**********************////****************Start**********************//
/////////Start Code of Rear Strips////////////////////Start Code of Rear Strips/////////////////////Start Code of Rear Strips////////////////
void setupRearStrips(uint8_t Brightness)
{
  R_Strip.begin();            // Initialize the strip
  R_Strip.show();             // Initialize all pixels to 'off'
  R_Strip.setBrightness(Brightness); // Set maximum brightness
}

//////////Flash///////Flash////////////Flash/////////////Flash//////////////Flash///////////

void flashRS(uint32_t color)
{
  // Turn all LEDs to the flash color
  for (int i = 0; i < REAR_LED_STRIP_CNT; i++)
  {
    R_Strip.setPixelColor(i, color);
  }
  R_Strip.show();
  vTaskDelay(1 / portTICK_PERIOD_MS); // Flash duration
}

void fadeOutRS()
{
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    for (int i = 0; i < REAR_LED_STRIP_CNT; i++) {
      uint32_t color = F_Strip.getPixelColor(i);
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = color & 0xFF;
      R_Strip.setPixelColor(i, R_Strip.Color(r * brightness / 255, g * brightness / 255, b * brightness / 255));
    }
    R_Strip.show();
    vTaskDelay(1 / portTICK_PERIOD_MS); // Adjust this for faster or slower fading
  }
}

void RearStripFlash(uint32_t color, uint8_t flashes_count)
{
  for (int i = 0; i < flashes_count; i++)
  { // Repeat n_flashes times
    flashRS(color);
    fadeOutRS();
    vTaskDelay(10 / portTICK_PERIOD_MS); // Short delay between flashes
  }
}
///////Flash//////////////Flash////////////Flash//////////////Flash//////////////Flash//////

void turnOnRearLEDS(uint8_t numLEDS, uint32_t color)
{
  // Limit numLEDS to the maximum number of LEDs in the strip
  if (numLEDS > REAR_LED_STRIP_CNT)
  {
    numLEDS = REAR_LED_STRIP_CNT;
  }

  // Clear all LEDs to 'off' (black color)
  for (int i = 0; i < REAR_LED_STRIP_CNT; i++)
  {
    R_Strip.setPixelColor(i, 0);  // Set color of each LED to black (off)
  }
  for (int i = 0; i < numLEDS; i++)
  {
    R_Strip.setPixelColor(i, color);  // Set color of each LED
  }
  R_Strip.show();                     // Display the updated colors
}
void turnOffRearLEDS()
{
  // Clear all LEDs to 'off' (black color)
  for (int i = 0; i < REAR_LED_STRIP_CNT; i++)
  {
    R_Strip.setPixelColor(i, 0);  // Set color of each LED to black (off)
  }
  R_Strip.show();                     // Display the updated colors
}

void turnOnRearLEDsRedBlue()
{
  R_Strip.setPixelColor(0, flashColorRed);  
  R_Strip.setPixelColor(1, flashColorBlue);  
  R_Strip.show();                     // Display the updated colors
}

///////// End  Code of Rear Strips//////////////////// End  Code of Rear Strips/////////////////////End   Code of Rear Strips////////////////
//****************End************************////****************End************************////****************End************************//

//****************Start**********************////****************Start**********************////****************Start**********************//
/////////Start Code of Common Functions///////////Start Code of  Common Functions/////////////Start Code of  Common Functions////////////////
void flashALL(uint32_t color)
{
  // Turn all LEDs to the flash color
  for (int i = 0; i < REAR_LED_STRIP_CNT; i++)
  {
    R_Strip.setPixelColor(i, color);
  }
  for (int i = 0; i < FRONT_LED_STRIP_CNT; i++)
  {
    F_Strip.setPixelColor(i, color);
  }
  for (int i = 0; i < RING_NUM_LEDS; i++)
  {
    Ring.setPixelColor(i, color);
  }
  Ring.show();
  F_Strip.show();
  R_Strip.show();
  vTaskDelay(1 / portTICK_PERIOD_MS); // Flash duration
}

void fadeOutALL()
{
  for (int brightness = 255; brightness >= 0; brightness -= 50)
  {
    for (int i = 0; i < REAR_LED_STRIP_CNT; i++)
    {
      uint32_t color = F_Strip.getPixelColor(i);
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = color & 0xFF;
      R_Strip.setPixelColor(i, R_Strip.Color(r * brightness / 255, g * brightness / 255, b * brightness / 255));
    }
    for (int i = 0; i < RING_NUM_LEDS; i++)
    {
      uint32_t color = Ring.getPixelColor(i);
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = color & 0xFF;
      Ring.setPixelColor(i, Ring.Color(r * brightness / 255, g * brightness / 255, b * brightness / 255));
    }
    for (int i = 0; i < FRONT_LED_STRIP_CNT; i++)
    {
      uint32_t color = F_Strip.getPixelColor(i);
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = color & 0xFF;
      F_Strip.setPixelColor(i, F_Strip.Color(r * brightness / 255, g * brightness / 255, b * brightness / 255));
    }
    F_Strip.show();
    Ring.show();
    R_Strip.show();
//    vTaskDelay(1 / portTICK_PERIOD_MS); // Adjust this for faster or slower fading
  }
  vTaskDelay(70 / portTICK_PERIOD_MS); // short delay after the fading
}

void fadeOutALLwithDelay(uint8_t duration)
{
  for (int brightness = 255; brightness >= 0; brightness -= 1)
  {
    for (int i = 0; i < REAR_LED_STRIP_CNT; i++)
    {
      uint32_t color = F_Strip.getPixelColor(i);
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = color & 0xFF;
      R_Strip.setPixelColor(i, R_Strip.Color(r * brightness / 255, g * brightness / 255, b * brightness / 255));
    }
    for (int i = 0; i < RING_NUM_LEDS; i++)
    {
      uint32_t color = Ring.getPixelColor(i);
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = color & 0xFF;
      Ring.setPixelColor(i, Ring.Color(r * brightness / 255, g * brightness / 255, b * brightness / 255));
    }
    for (int i = 0; i < FRONT_LED_STRIP_CNT; i++)
    {
      uint32_t color = F_Strip.getPixelColor(i);
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = color & 0xFF;
      F_Strip.setPixelColor(i, F_Strip.Color(r * brightness / 255, g * brightness / 255, b * brightness / 255));
    }
    F_Strip.show();
    Ring.show();
    R_Strip.show();
    if(brightness <= 210)
    {
      brightness = 0;
    }
    vTaskDelay(duration / portTICK_PERIOD_MS); // Adjust this for faster or slower fading
  }
}

void All_LEDs_Flash(uint32_t color, uint8_t flashes_count)
{
  for (int i = 0; i < flashes_count; i++)
  { // Repeat n_flashes times
    flashALL(color);
    fadeOutALL();
    vTaskDelay(1 / portTICK_PERIOD_MS); // Short delay between flashes
  }
}

void All_LEDs_is_Blue()
{
  turnOnRingLEDS(RING_NUM_LEDS, flashColorBlue);
  turnOnFrontLEDS(FRONT_LED_STRIP_CNT, flashColorBlue);
  turnOnRearLEDS(REAR_LED_STRIP_CNT, flashColorBlue);
  fadeOutALLwithDelay(50);
}

void All_LEDs_Flashes_Blue()
{
  All_LEDs_Flash(flashColorBlue, 20);
}
void All_LEDs_Flashes_Blue2()
{
  All_LEDs_Flash(flashColorBlue, 2);
}


/////////END   Code of Common Functions/////////// END  Code of  Common Functions///////////// END  Code of  Common Functions////////////////
//****************End************************////****************End************************////****************End************************//
#endif  //ARGBLIBRARY_H
