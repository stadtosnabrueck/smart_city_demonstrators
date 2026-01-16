// =============================================================================
// This Arduino sketch monitors and displays heat status using:
// - LED strips for visual animations
// - LCD display for status messages
// - 7-segment displays for numeric water level values
// - Chainable LED for status indicator
// =============================================================================


#include <ChainableLED.h>
#include <Wire.h>
#include <rgb_lcd.h>
#include <TM1637Display.h>
#include <Adafruit_NeoPixel.h>
#include "Grove_Temperature_And_Humidity_Sensor.h"

// LED strip configuration
int intA0LEDStripNumber = 8;   // Number of LEDs on A0 strip
int intA1LEDStripNumber = 10;  // Number of LEDs on A1 strip

// Temperature monitoring variables
int MaxCelsiusIncrease = 1;    // Maximum allowed temperature increase in Celsius
float TemperatureCurrent;      // Current temperature reading
float TemperaturePrevious;     // Previous temperature reading for comparison
int TemperatureTarget;         // Target temperature threshold

// Hardware component objects
ChainableLED D2led(2, 3, 1);                                                          // Status LED on pins 2 & 3
rgb_lcd I2CDisplay;                                                                   // RGB LCD display via I2C
TM1637Display D5Digit(5, 6);                                                          // 7-segment display for current temp (pins 5 & 6)
TM1637Display D7Digit(7, 8);                                                          // 7-segment display for alert temp (pins 7 & 8)
Adafruit_NeoPixel A0Strip = Adafruit_NeoPixel(intA0LEDStripNumber, A0, NEO_GRB + NEO_KHZ800);  // First LED strip on A0
Adafruit_NeoPixel A1Strip = Adafruit_NeoPixel(intA1LEDStripNumber, A1, NEO_GRB + NEO_KHZ800);  // Second LED strip on A1
DHT A3DHT(A3, DHT11);                                                                 // DHT11 temperature sensor on A3  

/*
 * animateStrip - Creates a running light animation on an LED strip
 * 
 * @param Strip: Pointer to the NeoPixel strip object
 * @param LEDStripNumber: Number of LEDs in the strip
 */
void animateStrip(Adafruit_NeoPixel* Strip, int LEDStripNumber) {
  // Animate a red pixel moving across the strip
  for (int i = 0; i <= LEDStripNumber; i += 1)
  {
    Strip->setPixelColor(i, Strip->Color(200, 0, 0));        // Set current LED to red
    Strip->setPixelColor((i - 1), Strip->Color(0, 0, 0));    // Turn off previous LED
    Strip->show();
    delay(400);
  }
  Strip->clear();    // Clear all LEDs
  Strip->show();
}

/*
 * resetAll - Resets all displays and LEDs to their initial state
 * 
 * Called when system needs to return to normal/idle state
 */
void resetAll() {
  // Reset LCD display to normal state (green)
  I2CDisplay.clear();
  I2CDisplay.setRGB(0, 150, 0);      // Set display to green
  I2CDisplay.setCursor(0, 0);
  I2CDisplay.print("Temperatur");
  I2CDisplay.setCursor(0, 1);
  I2CDisplay.print("normal.");

  // Clear first LED strip
  A0Strip.begin(); 
  A0Strip.clear();
  A0Strip.show();

  // Clear second LED strip
  A1Strip.begin(); 
  A1Strip.clear();
  A1Strip.show();

  // Clear alert temperature display
  D7Digit.clear(); 

  // Set status LED to green
  D2led.setColorRGB(0,0,150,0);  

  // Reset temperature target
  TemperatureTarget = 0;  
}

/*
 * setup - Initialize all hardware components and sensors
 * 
 * Runs once when the Arduino starts
 */
void setup(){
  // Configure pin modes
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A3, INPUT); 

  // Initialize LCD display (16x2 characters)
  I2CDisplay.begin(16, 2);

  // Initialize and configure first LED strip
  A0Strip.begin(); 
  A0Strip.setBrightness(150);
  A0Strip.show();  

  // Initialize and configure second LED strip
  A1Strip.begin(); 
  A1Strip.setBrightness(150);
  A1Strip.show();

  // Initialize temperature/humidity sensor
  A3DHT.begin();   

  // Set brightness for current temperature display
  D5Digit.setBrightness(4); 

  // Reset all components to initial state
  resetAll();
 }

/*
 * loop - Main program loop that monitors temperature and triggers alerts
 * 
 * Continuously reads temperature, detects increases, and activates
 * visual warnings when temperature rises too quickly.
 */
void loop(){
  delay(1500);  // Wait 1.5 seconds between readings

  float TempAndHum[2] = {0};  // Array to store temperature and humidity

  // Read temperature and humidity from sensor (returns 0 on success)
  if (!A3DHT.readTempAndHumidity(TempAndHum)) {

    // Extract temperature from array and display it
    TemperatureCurrent  = TempAndHum[1];
    D5Digit.showNumberDec(TemperatureCurrent); 

    // Initialize previous temperature on first reading
    if (TemperaturePrevious == 0) {
      TemperaturePrevious = TemperatureCurrent;
    }

    // Check if temperature has increased by at least 0.2°C
    if (TemperatureCurrent >= (TemperaturePrevious + 0.2)) {
      
      // Set target temperature threshold on first increase
      if (TemperatureTarget == 0) {
        TemperatureTarget = (int)TemperatureCurrent + MaxCelsiusIncrease;
      }

      /*D2led.setColorRGB(0, 50 + (200 - (((TemperatureTarget - (int)TemperatureCurrent))*50)),0,0);  */

      // Trigger alert sequence when target temperature is reached
      if ((int)TemperatureCurrent >= TemperatureTarget) {
        
        // Blink status LED red 3 times
        for (int i = 0; i < 3; i+=1) {
          D2led.setColorRGB(0,10,0,0);  
          delay(200);
          D2led.setColorRGB(0,200,0,0);  
          delay(200);
        } 

        // Run animation on first LED strip
        animateStrip(&A0Strip, intA0LEDStripNumber);

        // Fade in alert temperature display
        D7Digit.setBrightness(0);
        D7Digit.showNumberDec(TemperatureCurrent, false); 
        for (int i = 1; i <= 4; i+= 1) {
          D7Digit.setBrightness(i);
          D7Digit.showNumberDec(TemperatureCurrent, false); 
          delay(500);
        }

        delay(500);

        // Run animation on second LED strip
        animateStrip(&A1Strip, intA1LEDStripNumber); 

        // Display warning message ("Strong temperature increase!")
        I2CDisplay.clear();
        I2CDisplay.setRGB(200, 0, 0);  // Set to red
        I2CDisplay.setCursor(0, 0);
        I2CDisplay.print("Starker Tempe-");
        I2CDisplay.setCursor(0, 1);  
        I2CDisplay.print("raturanstieg!");    
        I2CDisplay.blinkLED();

        delay(5000);

        I2CDisplay.noBlinkLED();

        // Display hydration reminder ("Please drink enough")
        I2CDisplay.clear();
        I2CDisplay.setRGB(0, 0, 50);  // Set to blue
        I2CDisplay.setCursor(0, 0);
        I2CDisplay.print("Bitte genug");
        I2CDisplay.setCursor(0, 1);
        I2CDisplay.print("trinken.");

        delay(7000);

        // Update temperature readings for new baseline
        A3DHT.readTempAndHumidity(TempAndHum);
        TemperatureCurrent  = TempAndHum[1];     
        TemperaturePrevious = TempAndHum[1]; 

        // Reset all displays to normal state
        resetAll();
      }      
    }
    // If temperature dropped or stayed stable, reset to normal state
    else if  (TemperatureCurrent < (TemperaturePrevious + 0.2)) {
      resetAll();
    }
    // Update previous temperature for next comparison
    TemperaturePrevious = TemperatureCurrent;

  }  
}