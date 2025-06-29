#include "wled.h"

#define INPUT230_DEFAULT_PIN 4

volatile uint8_t pulseCount = 0;

static void IRAM_ATTR inputCounter() {
  pulseCount++;
}

class Input230 : public Usermod {

private:
  uint8_t pulsePin = INPUT230_DEFAULT_PIN;
  unsigned long lastCheck = 0;
  bool stripOn = (bri != 0);

public:
  void setup() {
    pinMode(pulsePin, INPUT);
    attachInterrupt(digitalPinToInterrupt(pulsePin), inputCounter, FALLING);
  }

  void loop() {
    unsigned long now = millis();
    if (now - lastCheck >= 60) {
      if (pulseCount >= 3 && !stripOn) {
        stripOn = true;
        bri = briLast;
      } else if (pulseCount <= 1 && stripOn) {
        stripOn = false;
        bri = 0;
      }
      pulseCount = 0;
      lastCheck = now;
    }
  }

  void addToJsonState(JsonObject &root) {
    root[F("pulse_on")] = stripOn;
    root[F("pulse_count")] = pulseCount;
  }

  void addToConfig(JsonObject &root) {
    JsonObject top = root.createNestedObject(FPSTR("Input230"));
    top[F("pulsePin")] = pulsePin;
  }

  bool readFromConfig(JsonObject &root) {
    JsonObject top = root[FPSTR("Input230")];
    if (!top.isNull()) {
      pulsePin = top[F("pulsePin")] | INPUT230_DEFAULT_PIN;
    }
    return true;
  }
};

// Define after the class
static Input230 input230;
REGISTER_USERMOD(input230);
