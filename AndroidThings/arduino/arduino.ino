#include <Adafruit_NeoPixel.h>

#define PIXELS_DATA_PIN 2

#define sp    Serial.print
#define spln  Serial.println

const int LEDS_COUNT = 144;
const int LEDS_SIDE = 12;
const char LED_SEPARATOR = ',';
const char START = '/';
const char RED = 'r';
const char GREEN = 'g';
const char BLUE = 'b';
const char BRIGHTNESS = 'a';
const char END = '!';

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LEDS_COUNT, PIXELS_DATA_PIN, NEO_GRB + NEO_KHZ800);
uint32_t colorOn;
uint32_t colorOff = pixels.Color(0,0,0);

String readString;
String wholeString;
char c;
boolean matrix[LEDS_COUNT];
int r = 0;
int g = 255;
int b = 255;
int brightness = 255;

void setup() {
  pixels.begin();
  pixels.setBrightness(brightness);
  Serial.begin(115200);
}

void loop() {
  while (Serial.available()) {
    delay(1);
    wholeString = Serial.readString();
    parseString(wholeString); 
  } 
}

void parseString(String input) {
  for(int i=0; i<input.length(); i++){
    c = input[i];
    if (c == LED_SEPARATOR) {
      if (readString.length() > 0) {
        int ledIndex = stringToInt(readString);
        if (ledIndex < LEDS_COUNT) {
          matrix[ledIndex] = true;
        }
      }
      readString="";
      continue;
    }
    else if (c == START) {
      reset();
      continue;
    } 
    else if (c == RED) {
      if (readString.length() > 0) {
        r = stringToInt(readString);
        readString="";
      }
      continue;
    }
    else if (c == GREEN) {
      if (readString.length() > 0) {
        g = stringToInt(readString);
        readString="";
      }
      continue;
    }
    else if (c == BLUE) {
      if (readString.length() > 0) {
        b = stringToInt(readString);
        readString="";
      }
      continue;
    }
    else if (c == BRIGHTNESS) {
      if (readString.length() > 0) {
        brightness = stringToInt(readString);
        readString="";
      }
      continue;
    }
    else if (c == END) {
      // logMatrix(); //Useful for debugging
      colorOn = pixels.Color(r, g, b);
      pixels.setBrightness(brightness);
      updateLEDs();
      pixels.show();
      c = ' ';
      break;
    } 
    readString += c;
  }
}

void reset() {
  for (int i=0; i<LEDS_COUNT; i++) {
    matrix[i] = false;
  }
}

int stringToInt(String string) {
  char carray[readString.length() + 1];
  readString.toCharArray(carray, sizeof(carray));
  return atoi(carray);
}

void logMatrix() {
  for (int i=0; i<LEDS_COUNT; i++) {
    sp(matrix[i] ? "X" : "_"); sp(" ");
    if (i % LEDS_SIDE == LEDS_SIDE-1) {
      spln();
    }
  }
}

// Our LEDs matrix is built with an Z pattern, 
// and the LEDs index order follows the following scheme:
// 
// > 0 1 2 3
//   7 6 5 4
//   8 9 10 11
//
// So we need to invert the column order in alternate rows from our logic matrix
void updateLEDs() {
  for (int i=0; i<LEDS_COUNT; i++) {
    int row = i / LEDS_SIDE;
    int ledId;
    if (row % 2 == 0) {
      // -->
      ledId = i;
    } else {
      // <--
      ledId = (row + 1) * LEDS_SIDE - (i - row * LEDS_SIDE) - 1;
    }
    updateLED(ledId, matrix[i]);  
  }
}

void updateLED(int index, boolean isOn) {
  if (isOn) {
    pixels.setPixelColor(index, colorOn); 
  } else {
    pixels.setPixelColor(index, colorOff); 
  }
}
