// =============================================================================
// This Arduino sketch monitors and displays plant watering status using:
// - LED strips for visual animations
// - LCD display for status messages
// - 7-segment displays for numeric water level values
// - Chainable LED for status indicator
// =============================================================================

// Required libraries for various displays and LED components
#include <ChainableLED.h>        // For chainable LED control
#include <Wire.h>                // For I2C communication
#include <rgb_lcd.h>             // For RGB LCD display
#include <TM1637Display.h>       // For 7-segment digit displays
#include <Adafruit_NeoPixel.h>   // For NeoPixel LED strips

// Configuration: Number of LEDs in each strip
int intA0LEDStripNumber = 8;     // Number of LEDs in first strip (A0)
int intA1LEDStripNumber = 10;    // Number of LEDs in second strip (A1)

// Configuration: Random number range for water level simulation
int intDigitBaseNumberMin = 20;  // Minimum value for random water level
int intDigitBaseNumberMax = 40;  // Maximum value for random water level

// Hardware component initialization
ChainableLED D2led(2, 3, 1);     // Chainable LED on pins 2 and 3, 1 LED
rgb_lcd I2CDisplay;               // I2C LCD display for status messages
TM1637Display D5Digit(5, 6);     // First 7-segment display (CLK=5, DIO=6)
TM1637Display D7Digit(7, 8);     // Second 7-segment display (CLK=7, DIO=8)
Adafruit_NeoPixel A0Strip = Adafruit_NeoPixel(intA0LEDStripNumber, A0, NEO_GRB + NEO_KHZ800);  // First NeoPixel strip
Adafruit_NeoPixel A1Strip = Adafruit_NeoPixel(intA1LEDStripNumber, A1, NEO_GRB + NEO_KHZ800);  // Second NeoPixel strip

// =============================================================================
// Function: animateStrip
// =============================================================================
/**
 * Animates an LED strip by lighting up LEDs one by one in red
 * Creates a "chaser" effect where each LED lights up then turns off
 * 
 * @param Strip Pointer to the NeoPixel strip to animate
 * @param intLEDStripNumber Number of LEDs in the strip
 */
void animateStrip(Adafruit_NeoPixel* Strip, int intLEDStripNumber) {
  // Iterate through each LED in the strip
  for (int i = 0; i <= intLEDStripNumber; i += 1)
  {
    Strip->setPixelColor(i, Strip->Color(200, 0, 0));        // Set current LED to red
    Strip->setPixelColor((i - 1), Strip->Color(0, 0, 0));    // Turn off previous LED
    Strip->show();                                            // Update the strip
    delay(400);                                               // Wait 400ms for visual effect
  }
  Strip->clear();    // Turn off all LEDs
  Strip->show();     // Update the strip
}

// =============================================================================
// Function: resetAll
// =============================================================================
/**
 * Resets all displays and LEDs to their default/idle state
 * Sets LCD to display "Keine neuen Daten" (No new data)
 * Clears all LED strips and digit displays
 */
void resetAll() {
  // Set chainable LED to a dim green/yellow color (idle state indicator)
  D2led.setColorRGB(0,100,20,0);

  // Configure LCD display with idle message
  I2CDisplay.clear();
  I2CDisplay.setRGB(150, 100, 0);        // Set to yellow/orange color
  I2CDisplay.setCursor(0, 0);            // First line
  I2CDisplay.print("Keine neuen");       // "No new"
  I2CDisplay.setCursor(0, 1);            // Second line
  I2CDisplay.print("Daten.");            // "data."

  // Reset first LED strip
  A0Strip.begin(); 
  A0Strip.clear();
  A0Strip.show();

  // Reset second LED strip
  A1Strip.begin(); 
  A1Strip.clear();
  A1Strip.show();

  // Turn off trigger pin
  digitalWrite(A3, LOW);

  // Clear both 7-segment displays
  D5Digit.clear();
  D7Digit.clear();
}

// =============================================================================
// Function: setup
// =============================================================================
/**
 * Setup function - runs once at startup
 * Initializes all hardware components and sets up pins
 */
void setup(){
  // Configure LED strip pins as outputs
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  
  // Initialize random seed from analog noise on pin 2
  randomSeed(analogRead(2)); 
  
  // Configure A3 as input (for touch sensor)
  pinMode(A3, INPUT); 

  // Initialize LCD display (16 columns, 2 rows)
  I2CDisplay.begin(16, 2);

  // Initialize and configure first LED strip
  A0Strip.begin(); 
  A0Strip.setBrightness(150);   // Set brightness level (0-255)
  A0Strip.show();               // Update the strip

  // Initialize and configure second LED strip
  A1Strip.begin(); 
  A1Strip.setBrightness(150);   // Set brightness level (0-255)
  A1Strip.show();               // Update the strip

  // Set all components to default idle state
  resetAll();
}

// =============================================================================
// Function: loop
// =============================================================================
/**
 * Main loop function - continuously checks for trigger and displays watering status
 * When triggered (A3 pin HIGH), simulates plant watering monitoring with:
 * - Random water level generation and display
 * - LED strip animations
 * - LCD status messages in German
 * - Visual feedback through colors and animations
 * 
 * Process Flow:
 * 1. Generate random water level with slot-machine effect
 * 2. Animate LED strips
 * 3. Display warning message (tree needs water)
 * 4. Show watering instruction
 * 5. Animate water level increase
 * 6. Display success message
 * 7. Reset to idle state
 */
void loop(){
  // Check if trigger is activated (e.g., touch sensor pressed)
  if (digitalRead(A3) == HIGH)
  {
    // =========================================================================
    // PHASE 1: Generate and display random water level
    // =========================================================================
    D5Digit.setBrightness(4);

    // Create "slot machine" effect by rapidly cycling through random numbers
    for (int i = 1; i <= 10; i+= 1) {
      D5Digit.showNumberDec(random(intDigitBaseNumberMin, intDigitBaseNumberMax+1), false);
      delay(100);
    }

    // Generate final random water level value
    int intRandomLEDValue = random(intDigitBaseNumberMin, intDigitBaseNumberMax+1);
    D5Digit.showNumberDec(intRandomLEDValue, false);
 
    delay(500);
 
    // Animate first LED strip to indicate measurement
    animateStrip(&A0Strip, intA0LEDStripNumber);

    // =========================================================================
    // PHASE 2: Fade in the value on second display
    // =========================================================================
    D7Digit.setBrightness(0);
    D7Digit.showNumberDec(intRandomLEDValue, false); 
    
    // Gradually increase brightness for fade-in effect
    for (int i = 1; i <= 4; i+= 1) {
      D7Digit.setBrightness(i);
      D7Digit.showNumberDec(intRandomLEDValue, false); 
      delay(500);
    }

    // Animate second LED strip
    animateStrip(&A1Strip, intA1LEDStripNumber); 

    // =========================================================================
    // PHASE 3: Display low water warning
    // =========================================================================
    I2CDisplay.clear();
    I2CDisplay.setRGB(200, 0, 0);          // Red color indicates warning
    I2CDisplay.setCursor(0, 0);
    I2CDisplay.print("Baum hat wenig");    // "Tree has little"
    I2CDisplay.setCursor(0, 1);
    I2CDisplay.print("Wasser.");           // "water."
    I2CDisplay.blinkLED();                 // Blink to draw attention

    delay(5000);                           // Display warning for 5 seconds

    I2CDisplay.noBlinkLED();               // Stop blinking

    // =========================================================================
    // PHASE 4: Display watering instruction
    // =========================================================================
    I2CDisplay.clear();
    I2CDisplay.setRGB(0, 0, 200);          // Blue color for instruction
    I2CDisplay.setCursor(0, 0);
    I2CDisplay.print("Bitte giessen.");    // "Please water."

    // =========================================================================
    // PHASE 5: Animate watering process
    // =========================================================================
    // Track position in LED strips
    int intA0LEDStripCounter = 0;
    int intA1LEDStripCounter = 0;    
    
    // Simulate watering by gradually changing colors and incrementing values
    for (int i = 0; i <= 59; i+= 2) {      
      // Gradually transition LCD from blue to green
      I2CDisplay.setRGB(0, i, 59-i);
      delay(150);
      
      // Increment displayed water level values
      D5Digit.showNumberDec(intRandomLEDValue+i, false);
      D7Digit.showNumberDec(intRandomLEDValue+i, false);   

      // Reset LED strip counters and clear when reaching the end
      if (intA0LEDStripCounter == (intA0LEDStripNumber)) {
        intA0LEDStripCounter = 0;
        A0Strip.clear();        
      }
      if (intA1LEDStripCounter == (intA1LEDStripNumber)) {
        intA1LEDStripCounter = 0;
        A1Strip.clear();        
      }

      // Create chasing LED effect on both strips
      A0Strip.setPixelColor(intA0LEDStripCounter-1, A0Strip.Color(0, 0, 0));  // Turn off previous
      A1Strip.setPixelColor(intA1LEDStripCounter-1, A1Strip.Color(0, 0, 0));  // Turn off previous
      A0Strip.setPixelColor(intA0LEDStripCounter, A0Strip.Color(200, 0, 0));  // Light up current (red)
      A1Strip.setPixelColor(intA1LEDStripCounter, A1Strip.Color(200, 0, 0));  // Light up current (red)
      A0Strip.show();     
      A1Strip.show();    
      
      // Move to next LED
      intA0LEDStripCounter += 1;
      intA1LEDStripCounter += 1;    
    }
    
    // Clear both LED strips after animation
    A0Strip.clear();    
    A0Strip.show();
    A1Strip.clear();    
    A1Strip.show();

    // =========================================================================
    // PHASE 6: Display success message
    // =========================================================================
    I2CDisplay.clear();
    I2CDisplay.setRGB(0, 200, 0);          // Green color indicates success
    I2CDisplay.setCursor(0, 0);
    I2CDisplay.print("Baum hat genug");    // "Tree has enough"
    I2CDisplay.setCursor(0, 1);
    I2CDisplay.print("Wasser.");           // "water."

    delay(2000);                           // Display success for 2 seconds

    // Set chainable LED to green to indicate completion
    D2led.setColorRGB(0,0,150,0);    

    delay(5000);                           // Hold success state for 5 seconds

    // =========================================================================
    // PHASE 7: Reset to idle state
    // =========================================================================
    resetAll();
  }
}
