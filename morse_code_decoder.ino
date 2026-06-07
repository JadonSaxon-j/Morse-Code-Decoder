#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ── LCD Setup ──
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change 0x27 to 0x3F if screen stays blank

// ── Pin ──
const int BUTTON_PIN = 2;

// ── Timing thresholds (milliseconds) ──
const unsigned long DOT_MAX     = 1000;  // under 1 second = dot
const unsigned long DASH_MAX    = 3000;  // 1–3 seconds = dash
// over 3 seconds = space

const unsigned long LETTER_GAP  = 2000;  // 2 seconds silence = decode letter

// ── Morse state ──
String morseInput    = "";   // current dots/dashes being built
String currentWord   = "";   // word being built from decoded letters
bool   buttonWasDown = false;
unsigned long pressStart   = 0;
unsigned long lastReleaseTime = 0;

// ── Morse code table ──
struct MorseCode {
  const char* code;
  char letter;
};

MorseCode morseTable[] = {
  {".-",   'A'}, {"-...", 'B'}, {"-.-.", 'C'}, {"-..",  'D'},
  {".",    'E'}, {"..-.", 'F'}, {"--.",  'G'}, {"....", 'H'},
  {"..",   'I'}, {".---", 'J'}, {"-.-",  'K'}, {".-..", 'L'},
  {"--",   'M'}, {"-.",   'N'}, {"---",  'O'}, {".--.", 'P'},
  {"--.-", 'Q'}, {".-.",  'R'}, {"...",  'S'}, {"-",    'T'},
  {"..-",  'U'}, {"...-", 'V'}, {".--",  'W'}, {"-..-", 'X'},
  {"-.--", 'Y'}, {"--..", 'Z'},
  {"-----",'0'}, {".----",'1'}, {"..---",'2'}, {"...--",'3'},
  {"....-",'4'}, {".....", '5'},{"-....","6"},  {"--...","7"},
  {"---..","8"}, {"----.", '9'}
};

const int MORSE_TABLE_SIZE = sizeof(morseTable) / sizeof(morseTable[0]);

// ── Decode a morse string to a character ──
char decodeMorse(String code) {
  for (int i = 0; i < MORSE_TABLE_SIZE; i++) {
    if (code == morseTable[i].code) {
      return morseTable[i].letter;
    }
  }
  return '?';  // unrecognized pattern
}

// ── Display a word on the LCD ──
void displayWord(String word) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Word:");
  lcd.setCursor(0, 1);
  lcd.print(word);
}

// ── Show live morse input on top row while typing ──
void showLiveMorse() {
  lcd.setCursor(0, 0);
  lcd.print("                ");  // clear top row
  lcd.setCursor(0, 0);
  String display = ">" + morseInput;
  if (display.length() > 16) display = display.substring(display.length() - 16);
  lcd.print(display);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Morse Decoder");
  lcd.setCursor(0, 1);
  lcd.print("Ready...");
  delay(2000);
  lcd.clear();
  Serial.begin(9600);
}

void loop() {
  bool buttonDown = (digitalRead(BUTTON_PIN) == LOW);
  unsigned long now = millis();

  // ── Button just pressed ──
  if (buttonDown && !buttonWasDown) {
    pressStart = now;
    buttonWasDown = true;
  }

  // ── Button just released ──
  if (!buttonDown && buttonWasDown) {
    unsigned long pressDuration = now - pressStart;
    buttonWasDown = false;
    lastReleaseTime = now;

    if (pressDuration < DOT_MAX) {
      morseInput += ".";
      Serial.print("DOT | morse so far: ");
      Serial.println(morseInput);
    } else if (pressDuration < DASH_MAX) {
      morseInput += "-";
      Serial.print("DASH | morse so far: ");
      Serial.println(morseInput);
    } else if (pressDuration >= 8000) {
      // held over 8 seconds = full reset
      morseInput   = "";
      currentWord  = "";
      lastReleaseTime = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Reset!");
      lcd.setCursor(0, 1);
      lcd.print("Ready...");
      Serial.println("RESET triggered");
      delay(1500);
      lcd.clear();

      } else {
      // held over 3 seconds = space between words
      if (morseInput.length() > 0) {
        char letter = decodeMorse(morseInput);
        currentWord += letter;
        morseInput = "";
        Serial.print("SPACE triggered | word so far: ");
        Serial.println(currentWord);
      }
      currentWord += " ";
      displayWord(currentWord);
      currentWord = "";
    }

    showLiveMorse();
  }

  // ── Silence for 2 seconds = end of letter ──
  if (!buttonDown && !buttonWasDown &&
      morseInput.length() > 0 &&
      lastReleaseTime > 0 &&
      (now - lastReleaseTime) >= LETTER_GAP) {

    char letter = decodeMorse(morseInput);
    currentWord += letter;
    morseInput = "";
    lastReleaseTime = 0;

    Serial.print("LETTER decoded: ");
    Serial.println(letter);
    Serial.print("Word so far: ");
    Serial.println(currentWord);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Building word:");
    lcd.setCursor(0, 1);
    lcd.print(currentWord);
  }
}