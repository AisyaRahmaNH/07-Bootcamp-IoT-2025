#include <Arduino.h>

// Tentukan pin
const int ledPin = 2;       // Ganti sesuai pin LED kamu
const int buzzerPin = 23;    // Ganti sesuai pin buzzer kamu

void setup() {
  // Inisialisasi pin sebagai output
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // Nyalakan LED dan buzzer
  digitalWrite(ledPin, HIGH);
  tone(buzzerPin, 1000);    // Bunyi buzzer 1 kHz
  delay(1000);              // Tunggu 1 detik

  // Matikan LED dan buzzer
  digitalWrite(ledPin, LOW);
  noTone(buzzerPin);        // Matikan suara buzzer
  delay(1000);              // Tunggu 1 detik
}
