#include <Wire.h>
#include "Adafruit_VL6180X.h"

Adafruit_VL6180X vl = Adafruit_VL6180X();

// Zeiger auf ein I2C Interface an den
// Pins 20 SDA und 21 SCL
TwoWire *wire = new TwoWire(20,21);

// Pins der LEDs
#define LED_PIN_YELLOW      0
#define LED_PIN_GREEN       1
#define LED_PIN_RED         2

// Schwellwerte für die Stufenauflösung
// der Entfernung -> Stärke der Dehnung
#define RANGE_LOW_THRESHOLD 65
#define RANGE_HIGH_THRESHOLD 85

// Hilfsmakro um nur Text über USB zu senden,
// wenn die USB Schnittstelle auch da ist.
#define SERIAL_PRINT_TEXT(text) if(isSerial) Serial.println(#text);

// USB Schnittstelle da?
bool isSerial = true;

void setup() {
  Serial.begin(115200);

  // Checken ob USB da ist
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

  // VL6180 initialisieren
  if (! vl.begin(wire)) {
    SERIAL_PRINT_TEXT("Failed to find sensor");
    while (1);
  }
  SERIAL_PRINT_TEXT("Sensor found!");
}

void loop() {
  // Lichtstärke in Lux lesen
  float lux = vl.readLux(VL6180X_ALS_GAIN_5);

  if (isSerial) {Serial.print("Lux: "); Serial.println(lux);}
  
  // Entfernung lesen
  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();

  // Error Ausgabe nur über USB, falls 
  // USB vorhanden
  if (isSerial) {
    if (status == VL6180X_ERROR_NONE) {
      Serial.print("Range: "); Serial.println(range);
    }

    // Some error occurred, print it out!
    
    if  ((status >= VL6180X_ERROR_SYSERR_1) && 
      (status <= VL6180X_ERROR_SYSERR_5)) {
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

  // Stufenauflösung und Setzen der LEDs
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

  // Messung alle ~50 ms
  delay(50);
}
