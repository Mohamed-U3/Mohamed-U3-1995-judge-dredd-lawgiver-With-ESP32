#ifndef ARGBLIBRARY_H
#define ARGBLIBRARY_H
#include "config.h"
#include <Adafruit_NeoPixel.h>

#define NUM_LEDS  MUZZLE_FLASH_LED_CNT
#define LED_PIN   MUZZLE_FLASH_LED_PIN
// Create an instance of the Adafruit_NeoPixel class
Adafruit_NeoPixel Ring = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void flash(uint32_t);
void fadeOut();

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

void muzzleFlash(uint32_t color, uint8_t flashes_count)
{
  for (int i = 0; i < flashes_count; i++)
  { // Repeat n_flashes times
    flash(color);
    fadeOut();
    vTaskDelay(1 / portTICK_PERIOD_MS); // Short delay between flashes
  }
}

void flash(uint32_t color)
{
  // Turn all LEDs to the flash color
  for (int i = 0; i < NUM_LEDS; i++) {
    Ring.setPixelColor(i, color);
  }
  Ring.show();
  vTaskDelay(1 / portTICK_PERIOD_MS); // Flash duration
}

void fadeOut()
{
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    for (int i = 0; i < NUM_LEDS; i++) {
      uint32_t color = Ring.getPixelColor(i);
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = color & 0xFF;
      Ring.setPixelColor(i, Ring.Color(r * brightness / 255, g * brightness / 255, b * brightness / 255));
    }
    Ring.show();
    delayMicroseconds(1); // Adjust this for faster or slower fading
  }
}


#endif  //ARGBLIBRARY_H
