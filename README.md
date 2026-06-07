# Morse Code Decoder

An Arduino Uno based morse code decoder using a tactile push button and I2C LCD1602 display.

## How It Works
- Press button under 1 second = Dot (.)
- Press button 1–3 seconds = Dash (-)
- Hold button over 3 seconds = Space between words
- 2 seconds of silence = letter is decoded and added to word
- Decoded text displays live on the LCD screen

## Hardware Required
- Arduino Uno
- Momentary tactile push button module
- I2C LCD1602 display (16x2)
- Jumper wires

## Wiring
| Component | Pin |
|---|---|
| Button S | Arduino Pin 2 |
| Button (-) | Arduino GND |
| LCD VCC | Arduino 5V |
| LCD GND | Arduino GND |
| LCD SDA | Arduino A4 |
| LCD SCL | Arduino A5 |

## Libraries Required
Install via Arduino IDE → Sketch → Include Library → Manage Libraries:
- LiquidCrystal I2C by Frank de Brabander

## Languages & Tools
- C++ (Arduino)
- Arduino IDE
- I2C Protocol

## Author
Jadon Saxon — Electrical Engineering Graduate, Mississippi State University
