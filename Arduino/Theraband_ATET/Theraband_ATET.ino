#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_VL6180X.h"
#include "mbed.h"

Adafruit_VL6180X vl = Adafruit_VL6180X();


// Zeiger auf ein I2C Interface an den
// Pins 16 SDA und 17 SCL
#define PIN_VL6180_SDA    16
#define PIN_VL6180_SCL    17

TwoWire *wire = new TwoWire(PIN_VL6180_SDA,PIN_VL6180_SCL);

// Pins der LEDs
#define LED_PIN_YELLOW      14
#define LED_PIN_GREEN       13
#define LED_PIN_RED         15

// Pins für den Buzzer
const PinName pinBuzzer = p18;
float buzzerVolume = 0.5;
mbed::PwmOut buzzer(pinBuzzer);
#define BUZZER_LOW_FREQ    500
#define BUZZER_MID_FREQ     800
#define BUZZER_HIGH_FREQ    1300

// Schwellwerte für die Stufenauflösung
// der Entfernung -> Stärke der Dehnung
volatile uint8_t lowThreshold = 65;
volatile uint8_t highThreshold = 85;

// Hilfsmakro um nur Text über USB zu senden,
// wenn die USB Schnittstelle auch da ist.
#define SERIAL_PRINT_TEXT(text) if(isSerial) Serial.println(#text);

// USB Schnittstelle da?
bool isSerial = true;

// Zustand Speichern
enum state {
  high,
  low,
  mid,
  undefined
};

volatile state currentState = undefined;
volatile state oldState = undefined;

// Messwerte
volatile uint8_t range;
volatile float lux;
volatile uint8_t status;

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

  // init LEDs
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
  lux = vl.readLux(VL6180X_ALS_GAIN_5);

  if (isSerial) {Serial.print("Lux: "); Serial.println(lux);}
  
  // Entfernung lesen
  range = vl.readRange();
  status = vl.readRangeStatus();

  // Error Ausgabe nur über USB, falls 
  // USB vorhanden
  if (status == VL6180X_ERROR_NONE) {
    if(isSerial)
      Serial.print("Range: "); Serial.println(range);
  }

  // Some error occurred, print it out!
  
  if  ((status >= VL6180X_ERROR_SYSERR_1) && 
    (status <= VL6180X_ERROR_SYSERR_5)) {
    if(isSerial)
      Serial.println("System error");
  }
  else if (status == VL6180X_ERROR_ECEFAIL) {
    if(isSerial)
      Serial.println("ECE failure");
  }
  else if (status == VL6180X_ERROR_NOCONVERGE) {
    if(isSerial)
      Serial.println("No convergence");
  }
  else if (status == VL6180X_ERROR_RANGEIGNORE) {
    if(isSerial)
      Serial.println("Ignoring range");
  }
  else if (status == VL6180X_ERROR_SNR) {
    if(isSerial)
      Serial.println("Signal/Noise error");
  }
  else if (status == VL6180X_ERROR_RAWUFLOW) {
    if(isSerial)
      Serial.println("Raw reading underflow");
  }
  else if (status == VL6180X_ERROR_RAWOFLOW) {
    if(isSerial)
      Serial.println("Raw reading overflow");
  }
  else if (status == VL6180X_ERROR_RANGEUFLOW) {
    if(isSerial)
      Serial.println("Range reading underflow");
  }
  else if (status == VL6180X_ERROR_RANGEOFLOW) {
    if(isSerial)
      Serial.println("Range reading overflow");
  }

  if(status == VL6180X_ERROR_NONE) {
    if (range < lowThreshold) {
      currentState = state::low;
    } else if (range > highThreshold) {
      currentState = state::high;
    } else {
      currentState = state::mid;
    }

    // Stufenauflösung und Setzen der LEDs
    switch(currentState) {
      case state::high:
        if(currentState == oldState)
          break;
        buzzer.period(1.0 / BUZZER_HIGH_FREQ);
        buzzer.write(buzzerVolume);
        digitalWrite(LED_PIN_YELLOW, LOW);
        digitalWrite(LED_PIN_GREEN, LOW);
        digitalWrite(LED_PIN_RED, HIGH);
        break;
      case state::mid:
        if(currentState == oldState)
          break;
        buzzer.period(1.0 / BUZZER_MID_FREQ);
        buzzer.write(buzzerVolume);
        digitalWrite(LED_PIN_YELLOW, LOW);
        digitalWrite(LED_PIN_GREEN, HIGH);
        digitalWrite(LED_PIN_RED, LOW);
        break;
      case state::low:
        if(currentState == oldState)
          break;
        buzzer.period(1.0 / BUZZER_LOW_FREQ);
        buzzer.write(buzzerVolume);
        digitalWrite(LED_PIN_YELLOW, HIGH);
        digitalWrite(LED_PIN_GREEN, LOW);
        digitalWrite(LED_PIN_RED, LOW);
        break;
      default:
        break;
    }
    oldState = currentState;
  } else {
    digitalWrite(LED_PIN_YELLOW, HIGH);
    digitalWrite(LED_PIN_GREEN, HIGH);
    digitalWrite(LED_PIN_RED, HIGH);
  }

  // Messung alle ~50 ms
  delay(50);
}
