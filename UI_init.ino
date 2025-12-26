#pragma once
#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define LED_PIN 23

// create an OLED display object connected to I2C
#define SCREEN_WIDTH 128 // OLED width,  in pixels
#define SCREEN_HEIGHT 64 // OLED height, in pixels


// Default Pins
constexpr int OLED_SDA = 21;
constexpr int OLED_SCL = 22;
constexpr int OLED_RESET = -1;  // -1 = not using a reset pin
constexpr int OLED_ADDR  = 0x3C; // default I2C address for most SSD1306 displays
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- KEYPAD SETUP ----------
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {19, 18, 5, 17};
byte colPins[COLS] = {16, 4, 0, 2};

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// ---------- VARIABLES ----------
bool durationSet = false;
int duration;

void setupOled(){
  Wire.begin(OLED_SDA, OLED_SCL);
  // initialize OLED display with I2C address 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.print(F("failed to start SSD1306 OLED"));
    // while (1);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();
}

void displayMsg(const String &msg, int x = 0, int y = 0) {
  // display.clearDisplay();
  display.setCursor(x, y);
  display.println(msg);
  display.display();

  //add formatting dependency on length of msg
}

int getDuration(){
  int tempDuration;
  String durationInput = "";
  while (!durationSet) {
    char key = keypad.getKey();

    if (key >= '0' && key <= '9') {  //*********need to implement continuous printing w function
      durationInput += key;
      display.clearDisplay();
      displayMsg("Enter duration");
      displayMsg(durationInput, 0, 16);
    }
    else if (key == 'A'){
      durationInput += '2';
      durationInput += '5';
      displayMsg("Default selected: 25", 0, 16);
    }

    else if (key == '#') { //enter function
      int enteredduration = durationInput.toInt();

      if (enteredduration <= 0 || enteredduration > 100) {
        displayMsg("Invalid! (100 max)");
        displayMsg("Enter new duration: ", 0, 16);
        durationInput = "";
      } else {
        duration = enteredduration;
        durationSet = true;
        displayMsg("duration: ");
        display.print(duration);
        display.display();
        delay(2000);
      }
    }

    else if (key == '*') {
      durationInput = "";
      displayMsg("Enter duration");
      display.println("# to confirm");
      display.display();
    }
  }
  return duration;
}
void getPrompts(){
  displayMsg("Enter Focus Duration: ");
  getDuration();
}
// ---------- MAIN ----------
void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);

  setupOled();

  // oled.setTextSize(2);

  display.print("Ready");
  display.display();
  delay(3000);
  display.clearDisplay();
}

void loop() {
  getPrompts();

}

