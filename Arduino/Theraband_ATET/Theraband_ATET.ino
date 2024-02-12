#include <Wire.h>
#include "Adafruit_VL6180X.h"

Adafruit_VL6180X vl = Adafruit_VL6180X();

#define LED_PIN_YELLOW      10
#define LED_PIN_GREEN       16
#define LED_PIN_RED         17

#define RANGE_LOW_THRESHOLD 65
#define RANGE_HIGH_THRESHOLD 85

#define PRINT_SERIAL(Text) if(isSerial) Serial.println(#Text);

bool isSerial = true;

void setup() {
  Serial.begin(115200);

  uint8_t serialCount = 0;
  while (!Serial) {
    if(serialCount++ > 100) {
      isSerial = false;
      break;
    }
    delay(1);
  }

  pinMode(LED_PIN_YELLOW, OUTPUT);
  pinMode(LED_PIN_GREEN, OUTPUT);
  pinMode(LED_PIN_RED, OUTPUT);

  PRINT_SERIAL("Adafruit VL6180x test!");
  if (! vl.begin()) {
    PRINT_SERIAL("Failed to find sensor");
    while (1);
  }
  PRINT_SERIAL("Sensor found!");
}

void loop() {
  float lux = vl.readLux(VL6180X_ALS_GAIN_5);

  Serial.print("Lux: "); PRINT_SERIAL(lux);
  
  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();

  if (status == VL6180X_ERROR_NONE) {
    Serial.print("Range: "); PRINT_SERIAL(range);
  }

  // Some error occurred, print it out!
  
  if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5)) {
    PRINT_SERIAL("System error");
  }
  else if (status == VL6180X_ERROR_ECEFAIL) {
    PRINT_SERIAL("ECE failure");
  }
  else if (status == VL6180X_ERROR_NOCONVERGE) {
    PRINT_SERIAL("No convergence");
  }
  else if (status == VL6180X_ERROR_RANGEIGNORE) {
    PRINT_SERIAL("Ignoring range");
  }
  else if (status == VL6180X_ERROR_SNR) {
    PRINT_SERIAL("Signal/Noise error");
  }
  else if (status == VL6180X_ERROR_RAWUFLOW) {
    PRINT_SERIAL("Raw reading underflow");
  }
  else if (status == VL6180X_ERROR_RAWOFLOW) {
    PRINT_SERIAL("Raw reading overflow");
  }
  else if (status == VL6180X_ERROR_RANGEUFLOW) {
    PRINT_SERIAL("Range reading underflow");
  }
  else if (status == VL6180X_ERROR_RANGEOFLOW) {
    PRINT_SERIAL("Range reading overflow");
  }

  if (range < RANGE_LOW_THRESHOLD) {
    digitalWrite(LED_PIN_YELLOW, HIGH);
    digitalWrite(LED_PIN_GREEN, LOW);
    digitalWrite(LED_PIN_RED, LOW);
  } else if (range > RANGE_HIGH_THRESHOLD) {
    digitalWrite(LED_PIN_YELLOW, LOW);
    digitalWrite(LED_PIN_GREEN, LOW);
    digitalWrite(LED_PIN_RED, HIGH);
  } else {
    digitalWrite(LED_PIN_YELLOW, LOW);
    digitalWrite(LED_PIN_GREEN, HIGH);
    digitalWrite(LED_PIN_RED, LOW);
  }


  delay(50);
}
