/*
*  Created by Luca Soldi on 12/09/18.
*  Copyright © 2018 Luca Soldi. All rights reserved.
*/


/* By default, the APA102 library uses pinMode and digitalWrite
 * to write to the LEDs, which works on all Arduino-compatible
 * boards but might be slow.  If you have a board supported by
 * the FastGPIO library and want faster LED updates, then install
 * the FastGPIO library and uncomment the next two lines: */
#include <FastGPIO.h>
#define APA102_USE_FAST_GPIO

#include <APA102.h>
#include <SoftwareSerial.h>

#define MAX_BUFFER  10

// Define which pins to use.
const uint8_t dataPin = 11;
const uint8_t clockPin = 12;

// Create an object for writing to the LED strip.
APA102<dataPin, clockPin> ledStrip;

// Set the number of LEDs to control.
const uint16_t ledCount = 84;

// Create a buffer for holding the colors (3 bytes per color).
rgb_color colors[ledCount];

SoftwareSerial ble(2, 3); // RX, TX

rgb_color color;
uint8_t brightness;

int idx_buffer;
uint8_t buffer[MAX_BUFFER];

void setup() {
  // Open serial port
  Serial.begin(9600);
  // begin bluetooth serial port communication
  ble.begin(9600);
}

void loop()
{
  if (ble.available()) {
  
    buffer[idx_buffer] = ble.read();
    
    if (idx_buffer > 4) {
      if (buffer[idx_buffer] == 170 && (buffer[idx_buffer-5] == 85)) { // possibly end packet
        //correct packet
        color.red = buffer[idx_buffer-4];
        color.green = buffer[idx_buffer-3];
        color.blue = buffer[idx_buffer-2];
        brightness = buffer[idx_buffer-1];
        
        //clean buffer
        memset(buffer, 0, sizeof(buffer));
        idx_buffer = 0;
        
        for(uint16_t i = 0; i < ledCount; i++) {
          colors[i].red = color.red;
          colors[i].green = color.green;
          colors[i].blue = color.blue;
        }
        ledStrip.write(colors, ledCount, brightness);
      }
    }
            
    idx_buffer++;
    
    if (idx_buffer == MAX_BUFFER) {
      memset(buffer, 0, sizeof(buffer));
      idx_buffer = 0;
    }
  }
}
