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

#define MAX_SAMPLES 10
#define MAX_DISTANCE 3

bool isCalibrating = false;
uint8_t sample_count = 0;
float refDistance_mm;
uint8_t samples[MAX_SAMPLES];


// functions
void evaluateError(uint8_t status); // Error of VL6180x
float averageValue(const uint8_t *samplesPtr);
float distance(float value1, float value2);

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(10);
  }

  pinMode(LED_PIN_YELLOW, OUTPUT);
  pinMode(LED_PIN_GREEN, OUTPUT);
  pinMode(LED_PIN_RED, OUTPUT);

  pinMode(PIN_START_SAMPLE_BUTTON, INPUT);

  Serial.println("Adafruit VL6180x test!"); 
  if (! vl.begin(wire)) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("Sensor found!");

  digitalWrite(LED_PIN_GREEN, HIGH);

  Serial.println("Initialisation for calibration done.");
}

void loop() {
  if(digitalRead(PIN_START_SAMPLE_BUTTON)) {
    isCalibrating = true;
    Serial.print("Please insert reference distance for calibration in mm: ");
    refDistance_mm = Serial.read();
    digitalWrite(LED_PIN_RED, HIGH);
    digitalWrite(LED_PIN_GREEN, LOW);

    Serial.println("Start calibration.");
  }
  
  while(isCalibrating) {
    uint8_t range = vl.readRange();
    uint8_t status = vl.readRangeStatus();

    if (status == VL6180X_ERROR_NONE) {
      samples[sample_count] = range;
      ++sample_count;
      Serial.print("Sample ");
      Serial.print(sample_count);
      Serial.print(": ");
      Serial.println(range);
    } else {
      // Some error occurred, print it out!
      evaluateError(status);
    }

    if(sample_count == MAX_SAMPLES){
      isCalibrating = false;

      float average = averageValue(samples);
      Serial.print("Average of samples is ");
      Serial.println(average);

      float avrgDistance = distance(refDistance_mm, average);
      Serial.print("Average distance to refence Value is ");
      Serial.println(avrgDistance);
      if (avrgDistance > MAX_DISTANCE) {
        Serial.println("Offset calibration needs to be done.");
      } else {
        Serial.println("Offset calibration needs to be done.");
      }

      sample_count = 0;
      digitalWrite(LED_PIN_RED, LOW);
      digitalWrite(LED_PIN_GREEN, HIGH);
    }

    delay(50);
  }


  delay(50);
}

void evaluateError(uint8_t status) {
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

float averageValue(const uint8_t *samplesPtr) {
  uint32_t sum = 0;

  for(uint8_t i = 0; i < MAX_SAMPLES; i++) {
    sum += samplesPtr[i];
  }

  return (float) sum/MAX_SAMPLES;
}

float distance(float value1, float value2) {
  if (value1 >= value2) {
    return value1 - value2;
  } else {
    return value2 - value1;
  }
}
