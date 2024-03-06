#include <Wire.h>
#include "Adafruit_VL6180X.h"

Adafruit_VL6180X vl = Adafruit_VL6180X();

TwoWire *wire = new TwoWire(20,21);

#define LED_PIN_YELLOW      0
#define LED_PIN_GREEN       1
#define LED_PIN_RED         2

#define PIN_START_SAMPLE_BUTTON 16

#define RANGE_LOW_THRESHOLD 65
#define RANGE_HIGH_THRESHOLD 85

#define SERIAL_PRINT_TEXT(text) if(isSerial) Serial.println(#text);

bool isSerial = true;
bool isCalibrating = false;
uint8_t sample_count = 0;
#define MAX_SAMPLES 20

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

  pinMode(PIN_START_SAMPLE_BUTTON, INPUT);

  SERIAL_PRINT_TEXT("Adafruit VL6180x test!"); 
  if (! vl.begin(wire)) {
    SERIAL_PRINT_TEXT("Failed to find sensor");
    while (1);
  }
  SERIAL_PRINT_TEXT("Sensor found!");

  digitalWrite(LED_PIN_GREEN, HIGH);

}

void loop() {
  if(digitalRead(PIN_START_SAMPLE_BUTTON)) {
    isCalibrating = true;
    digitalWrite(LED_PIN_RED, HIGH);
    digitalWrite(LED_PIN_GREEN, LOW);

    SERIAL_PRINT_TEXT("Start new sample series.")
  }
  
  while(isCalibrating) {
    uint8_t range = vl.readRange();
    uint8_t status = vl.readRangeStatus();

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

    sample_count++;

    if(sample_count == MAX_SAMPLES){
      isCalibrating = false;
      sample_count = 0;
      digitalWrite(LED_PIN_RED, LOW);
      digitalWrite(LED_PIN_GREEN, HIGH);
    }

    delay(50);
  }


  delay(50);
}
