#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_SSD1306.h>

// === Pin & Komponen ===
#define POT_BPM_PIN    33    // Simulasi detak jantung
#define POT_TIME_PIN   32    // Simulasi waktu belajar
#define ONEWIRE_PIN    2
#define BUZZER_PIN     19
#define LED_PIN        18
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MPU6050 mpu;
OneWire oneWire(ONEWIRE_PIN);
DallasTemperature ds18b20(&oneWire);

// Status
bool posture_bad = false, fatigue = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Inisialisasi MPU6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 gagal!");
    while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Inisialisasi OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED gagal!");
    while (1);
  }

  ds18b20.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  // MPU6050
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float pitch = atan2(a.acceleration.x, sqrt(a.acceleration.z * a.acceleration.z + a.acceleration.y * a.acceleration.y)) * 57.3;

  // Potensiometer simulasi detak jantung
  int potBPM = analogRead(POT_BPM_PIN);
  int bpm = map(potBPM, 0, 4095, 40, 120);

  // Potensiometer simulasi waktu belajar
  int potTime = analogRead(POT_TIME_PIN);
  int belajarMenit = map(potTime, 0, 4095, 0, 60);

  // DS18B20
  ds18b20.requestTemperatures();
  float neckTemp = ds18b20.getTempCByIndex(0);

  // Status suhu
  String Stat_Suhu = "";
  if (neckTemp >= 31.0 && neckTemp <= 33.0) {
    Stat_Suhu = "Brain Fog!";
  } else if (neckTemp >= 33.1 && neckTemp <= 37.9) {
    Stat_Suhu = "Aman";
  } else if (neckTemp >= 38.0 && neckTemp <= 40.0) {
    Stat_Suhu = "Istirahat!";
  } else {
    Stat_Suhu = "Periksa ulang";
  }

  // Logika postur
  if (abs(pitch) > 60) {
    posture_bad = true;
  } else if (abs(pitch) < 20) {
    posture_bad = false;
  }

  // Kelelahan
  fatigue = bpm > 100;

  // OLED Output
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Waktu: ");
  display.print(belajarMenit);
  display.println(" min");

  display.print("Pitch: ");
  display.println(pitch, 1);
  display.print("BPM: ");
  display.println(bpm);
  display.print("Suhu: ");
  display.println(neckTemp);
  display.print("Status: ");
  display.println(Stat_Suhu);

  // Logika istirahat
  if (belajarMenit > 45) {
    tone(BUZZER_PIN, 2000, 200);
    display.println("Istirahat!");
  } else if (belajarMenit > 30) {
    display.println("Waspada kelelahan");
  }

  // Deteksi Brain Fog
  if (posture_bad && belajarMenit > 30 && bpm > 100) {
    display.println("Brain Fog Detected!");
    digitalWrite(LED_PIN, HIGH);
    for (int i = 0; i < 3; i++) {
      tone(BUZZER_PIN, 2000, 200);
      delay(300);
    }
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  display.display();

  // Serial monitor log
  Serial.print("Pitch: "); Serial.print(pitch);
  Serial.print(" | BPM: "); Serial.print(bpm);
  Serial.print(" | Waktu: "); Serial.print(belajarMenit);
  Serial.print(" | Suhu: "); Serial.print(neckTemp);
  Serial.print(" | Status: "); Serial.println(Stat_Suhu);

  delay(1000);
}
