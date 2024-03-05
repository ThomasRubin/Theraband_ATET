#include <Wire.h>
#include "Adafruit_VL6180X.h"

Adafruit_VL6180X vl = Adafruit_VL6180X();

TwoWire *wire = new TwoWire(20,21);

#define LED_PIN_YELLOW      10
#define LED_PIN_GREEN       16
#define LED_PIN_RED         17

#define RANGE_LOW_THRESHOLD 65
#define RANGE_HIGH_THRESHOLD 85

#define SERIAL_PRINT_TEXT(text) if(isSerial) Serial.println(#text);

bool isSerial = true;

void setup() {
  Serial.begin(115200);

  uint16_t serialCount = 0;
  while (!Serial) {
    if(serialCount++ > 1000) {
      isSerial = false;
      break;
    }
    delay(1);
  }



  pinMode(LED_PIN_YELLOW, OUTPUT);
  pinMode(LED_PIN_GREEN, OUTPUT);
  pinMode(LED_PIN_RED, OUTPUT);

  SERIAL_PRINT_TEXT("Adafruit VL6180x test!"); 

  

  if (! vl.begin(wire)) {
    SERIAL_PRINT_TEXT("Failed to find sensor");
    while (1);
  }
  SERIAL_PRINT_TEXT("Sensor found!");
}

void loop() {
  float lux = vl.readLux(VL6180X_ALS_GAIN_5);

  if (isSerial) {Serial.print("Lux: "); Serial.println(lux);}
  
  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();

  if (isSerial) {
    if (status == VL6180X_ERROR_NONE) {
      Serial.print("Range: "); Serial.println(range);
    }

    // Some error occurred, print it out!
    
    if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5)) {
      Serial.println("System error");
    }
    else if (status == VL6180X_ERROR_ECEFAIL) {
      Serial.println("ECE failure");
    }
    else if (status == VL6180X_ERROR_NOCONVERGE) {
      Serial.println("No convergence");
    }
    else if (status == VL6180X_ERROR_RANGEIGNORE) {
      Serial.println("Ignoring range");
    }
    else if (status == VL6180X_ERROR_SNR) {
      Serial.println("Signal/Noise error");
    }
    else if (status == VL6180X_ERROR_RAWUFLOW) {
      Serial.println("Raw reading underflow");
    }
    else if (status == VL6180X_ERROR_RAWOFLOW) {
      Serial.println("Raw reading overflow");
    }
    else if (status == VL6180X_ERROR_RANGEUFLOW) {
      Serial.println("Range reading underflow");
    }
    else if (status == VL6180X_ERROR_RANGEOFLOW) {
      Serial.println("Range reading overflow");
    }
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
