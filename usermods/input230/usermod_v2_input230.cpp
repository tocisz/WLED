#include "wled.h"

volatile uint8_t pulseCount = 0;
static void IRAM_ATTR inputCounter() {
  pulseCount++;
}

class Input230 : public Usermod {

private:
  const uint8_t pulsePin = 14; // input pin (normally HIGH, LOW pulses)
  unsigned long lastCheck = 0;
  bool stripOn = false;

public:
  void setup() {
    pinMode(pulsePin, INPUT); // Changed from INPUT_PULLUP to INPUT
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
      pulseCount = 0; // reset for next 60ms window
      lastCheck = now;
    }
  }

  // optional: provide JSON state for debugging
  void addToJsonState(JsonObject &root) {
    root["pulse_on"] = stripOn;
    root["pulse_count"] = pulseCount;
  }
};

static Input230 input230;
REGISTER_USERMOD(input230);