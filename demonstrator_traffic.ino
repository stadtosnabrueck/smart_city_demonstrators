// =============================================================================
// This Arduino sketch monitors and displays traffic status using:
// - LED strips for visual animations
// - LCD display for status messages
// - 7-segment displays for numeric water level values
// - Chainable LED for status indicator
// =============================================================================

// Required libraries for various displays and LED components
#include <ChainableLED.h>      // For chainable RGB LED control
#include <Wire.h>               // For I2C communication
#include <rgb_lcd.h>            // For RGB backlit LCD display
#include <TM1637Display.h>      // For 7-segment display
#include <Adafruit_NeoPixel.h>  // For NeoPixel LED strip control

// Define the number of LEDs in each strip
int intA0LEDStripNumber = 11;   // Number of LEDs on strip connected to A0
int intA1LEDStripNumber = 11;   // Number of LEDs on strip connected to A1

// Light sensor variables for detecting changes
int LightDifference = 50;       // Threshold for light change detection
int LightCurrent = 0;           // Current light sensor reading
int LightPrevious = 0;          // Previous light sensor reading

// Initialize display and LED components
ChainableLED D2led(2, 3, 1);   // Chainable LED on pins 2 and 3, with 1 LED
rgb_lcd I2CDisplay;             // RGB LCD display via I2C
TM1637Display D7Digit(7, 8);    // 7-segment display on pins 7 and 8
Adafruit_NeoPixel A0Strip = Adafruit_NeoPixel(intA0LEDStripNumber, A0, NEO_GRB + NEO_KHZ800);  // First LED strip
Adafruit_NeoPixel A1Strip = Adafruit_NeoPixel(intA1LEDStripNumber, A1, NEO_GRB + NEO_KHZ800);  // Second LED strip

/**
 * Animate LED strip with a moving red light effect
 * @param Strip Pointer to the NeoPixel strip to animate
 * @param LEDStripNumber Number of LEDs in the strip
 */
void animateStrip(Adafruit_NeoPixel* Strip, int LEDStripNumber) {
  // Loop through each LED in the strip
  for (int i = 0; i <= LEDStripNumber; i += 1)
  {
    Strip->setPixelColor(i, Strip->Color(200, 0, 0));         // Set current LED to red
    Strip->setPixelColor((i - 1), Strip->Color(0, 0, 0));     // Turn off previous LED
    Strip->show();                                             // Update the strip
    delay(400);                                                // Wait 400ms between steps
  }
  Strip->clear();    // Clear all LEDs
  Strip->show();     // Update the strip
}

/**
 * Reset all displays and LEDs to initial state
 * Shows "50 Parkplaetze frei" (50 parking spaces available)
 */
void resetAll() {
  // Configure LCD display to show available parking
  I2CDisplay.clear();                      // Clear the display
  I2CDisplay.setRGB(0, 150, 0);            // Set backlight to green
  I2CDisplay.setCursor(0, 0);              // First line
  I2CDisplay.print("50 Parkplaetze");      // "50 parking spaces"
  I2CDisplay.setCursor(0, 1);              // Second line
  I2CDisplay.print("frei.");               // "available"
 
  // Reset first LED strip
  A0Strip.begin(); 
  A0Strip.clear();
  A0Strip.show();

  // Reset second LED strip
  A1Strip.begin(); 
  A1Strip.clear();
  A1Strip.show();

  // Clear 7-segment display
  D7Digit.clear();

  // Set chainable LED to green
  D2led.setColorRGB(0,0,150,0);      
}

/**
 * Initial setup - runs once at startup
 * Configures pins and initializes all displays and LED strips
 */
void setup(){
  // Configure pin modes
  pinMode(A0, OUTPUT);    // LED strip 1 output
  pinMode(A1, OUTPUT);    // LED strip 2 output

  pinMode(A3, INPUT);     // Light sensor input

  // Initialize LCD display (16 columns x 2 rows)
  I2CDisplay.begin(16, 2);

  // Initialize and configure first LED strip
  A0Strip.begin(); 
  A0Strip.setBrightness(150);  // Set brightness level
  A0Strip.show();  

  // Initialize and configure second LED strip
  A1Strip.begin(); 
  A1Strip.setBrightness(150);  // Set brightness level
  A1Strip.show();  

  // Reset all displays to initial state
  resetAll();
 }

/**
 * Main loop - continuously monitors light sensor
 * When significant light change is detected (e.g., hand wave),
 * triggers parking lot full sequence
 */
void loop(){

  delay(1000);  // Wait 1 second between readings

  // Read current light level from sensor on A3
  LightCurrent =  analogRead(A3);

  // Check if light level dropped significantly (interaction detected)
  if (LightCurrent <= (LightPrevious - LightDifference))
  {
    // === PARKING LOT FULL SEQUENCE START ===
    
    // Turn chainable LED to red (warning)
    D2led.setColorRGB(0,200,0,0);  

    // Animate first LED strip (cars entering)
    animateStrip(&A0Strip, intA0LEDStripNumber);

    // Fade in the number 50 on 7-segment display
    D7Digit.setBrightness(0);                    // Start with display off
    D7Digit.showNumberDec(50, false);            // Set number to 50
    for (int i = 1; i <= 4; i+= 1) {
      D7Digit.setBrightness(i);                  // Gradually increase brightness
      D7Digit.showNumberDec(50, false); 
      delay(500);                                // Wait between brightness steps
    }

    // Count down from 50 to 0 (parking spaces filling up)
    for (int i = 50; i >= 0; i-= 1) {
      D7Digit.showNumberDec(i, false);           // Display current count
      delay(10);                                 // Fast countdown
    }

    delay(500);  // Brief pause

    // Animate second LED strip (more cars entering)
    animateStrip(&A1Strip, intA1LEDStripNumber); 

    // Display "parking lot full" message with red backlight
    I2CDisplay.clear();
    I2CDisplay.setRGB(200, 0, 0);                // Red backlight (warning)
    I2CDisplay.setCursor(0, 0);
    I2CDisplay.print("Alle Park-");               // "All parking"
    I2CDisplay.setCursor(0, 1);  
    I2CDisplay.print("plaetze belegt.");          // "spaces occupied"
    I2CDisplay.blinkLED();                       // Blink backlight for attention

    delay(5000);  // Show message for 5 seconds  // Show message for 5 seconds

    // Stop blinking
    I2CDisplay.noBlinkLED();

    // Display public transport suggestion with blue backlight
    I2CDisplay.clear();
    I2CDisplay.setRGB(0, 0, 50);                 // Blue backlight
    I2CDisplay.setCursor(0, 0);
    I2CDisplay.print("Bitte Nahverkehr");         // "Please use public"
    I2CDisplay.setCursor(0, 1);
    I2CDisplay.print("nutzen. Danke!");           // "transport. Thanks!"

    delay(7000);  // Show message for 7 seconds

    // Reset everything back to initial state
    resetAll();
  }
  
  // Store current reading for next comparison
  LightPrevious = LightCurrent;
}