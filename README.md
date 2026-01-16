# Smart City Demonstrators using Arduino Platform

The City of Osnabrück developed several exhibits to demonstrate Smart City topics interactively. These demonstrators highlight the relationship between data and challenges in a playful manner. They have been showcased in museums and other exhibitions.

## Implementation Details

The demonstrators were implemented using the **Arduino platform**, which combines open-source hardware and software. To simplify the connection of sensors and switches, the **GROVE connector system** was utilized, though other systems can also be used.

### 1. Required Hardware

- **1x Arduino Uno REV3** (recommended version as some GROVE sensors may not work with newer versions)
- **1x GROVE Base Shield v2**
- **1x GROVE Touch Sensor**
- **2x GROVE 4-Digit Display**
- **1x GROVE RGB LED**
- **1x GROVE LCD-Display 16x2** with RGB-Backlight
- **2x GROVE WS2813 RGB LED Strips** (can be shortened with scissors)
- Multiple (5 in this case) **GROVE 4-Pin extension cables (female/female)**
- **12V/2.5A power supply** with a sufficiently long cable

#### Additional Materials
- **Printed Wood Panel:** Any service provider offering printed graphics on wood panels can be used.
- **Glue Gun:** Used to mount sensors and switches on the wood panel.
- **Drilling Holes:** Holes for cable routing were drilled manually.
- **Slipcover Box:** (= "Stülpschachtel") Mounted at the back to secure the Arduino and cables.
- **TV Stand:** Used to mount the wood panel with standard **VESA holes** for attachment.

---

### 2. Wiring Instructions

To operate the demonstrator with the provided software, precise pin connections are necessary as follows:

| Pin  | Component               |
|------|-------------------------|
| **A3** | Touch Sensor          |
| **D5** | 4-Digit Display       |
| **D2** | RGB LED               |
| **A0** | RGB LED Strip         |
| **D7** | 4-Digit Display       |
| **A1** | RGB LED Strip         |
| **I2C** | LCD Display          |

---

### 3. Software Setup

Follow the steps below:

1. Download and install the **Arduino IDE** from [www.arduino.cc](https://www.arduino.cc/).
2. Update the Arduino firmware if necessary.
3. Install the following libraries via the Arduino Library Manager:
   - **Adafruit NeoPixel**
   - **Grove Chainable RGB LED**
   - **Grove LCD RGB Backlight**
   - **Grove 4-Digit Display**
   - **TM1637**
   - **TM1637TinyDisplay**
4. Load and transfer the file `demonstrator_watering.ino` to the Arduino board.

---
