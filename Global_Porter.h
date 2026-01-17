#include "OLED.h"

// ----- ESP32 DEVKTIT V1 PIN ----- //
const int pinPWM[] = {13, 4, 14, 33};
const int pinDIR[] = {12, 2, 27, 32};

#define pinLifter1 25
#define pinLifter2 26

//#define servo 15

#define screenWidth 128 // sda 21
#define screenHeight 64 // scl 22

Adafruit_SSD1306 display(screenWidth, screenHeight, &Wire, -1);

void initialSetup() {
  // Used pinMode
  for(int i = 0; i < 4; i++) {
    pinMode(pinPWM[i], OUTPUT);
    pinMode(pinDIR[i], OUTPUT);
  }

  pinMode(pinLifter1, OUTPUT);
  pinMode(pinLifter2, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Failed to detect OLED"));
    for(;;);
  }

  display.clearDisplay();

  display.drawBitmap(32, 0, epd_bitmap_logoUKSW, 64, 64, WHITE);
  
  display.display();
}



ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id, properties.product_id);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
      }
    }

    if (!foundEmptySlot) {
      Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }

    if (!foundController) {
      Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

#include "Motion_Porter.h"

void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
         processGamepad(myController);
      }
      else {
        Serial.println("Unsupported controller");
      }
    }
  }
}