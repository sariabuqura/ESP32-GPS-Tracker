#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <TinyGPS++.h>

// =========================
// OLED
// =========================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SH1106G display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// =========================
// GPS
// =========================
#define GPS_RX 16   // ESP32 RX <- GPS TX
#define GPS_TX 17   // ESP32 TX -> GPS RX

HardwareSerial GPS(1);
TinyGPSPlus gps;

// =========================
// Last successful sync
// =========================
int syncDay = 0;
int syncMonth = 0;
int syncYear = 0;

int syncHour = 0;
int syncMinute = 0;
int syncSecond = 0;

bool syncAvailable = false;

// =========================
// Setup
// =========================
void setup() {

  Serial.begin(115200);

  GPS.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

  if (!display.begin(SCREEN_ADDRESS, true)) {
    Serial.println("OLED initialization failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);

  display.setCursor(20, 25);
  display.print("GPS Starting...");

  display.display();

  delay(2000);
}

// =========================
// Main Loop
// =========================
void loop() {

  // Read GPS data
  while (GPS.available()) {

    char c = GPS.read();

    gps.encode(c);

    // Save last successful synchronization
    if (gps.location.isUpdated()) {

      if (gps.location.isValid() &&
          gps.date.isValid() &&
          gps.time.isValid()) {

        syncDay = gps.date.day();
        syncMonth = gps.date.month();
        syncYear = gps.date.year();

        syncHour = gps.time.hour();
        syncMinute = gps.time.minute();
        syncSecond = gps.time.second();

        syncAvailable = true;

        Serial.println("GPS DATA SYNCHRONIZED");
      }
    }
  }

  updateDisplay();

  delay(500);
}

// =========================
// OLED Display
// =========================
void updateDisplay() {

  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);

  // =========================
  // Row 1 - GPS status
  // =========================

  display.setCursor(0, 0);

  if (gps.location.isValid()) {
    display.print("FIX");
  }
  else {
    display.print("SEARCH");
  }

  display.setCursor(75, 0);
  display.print("SAT:");
  display.print(gps.satellites.value());

  // =========================
  // Row 2 - Latitude
  // =========================

  display.setCursor(0, 12);
  display.print("LAT:");

  if (gps.location.isValid()) {
    display.print(gps.location.lat(), 5);
  }
  else {
    display.print("---");
  }

  // =========================
  // Row 3 - Longitude
  // =========================

  display.setCursor(0, 24);
  display.print("LON:");

  if (gps.location.isValid()) {
    display.print(gps.location.lng(), 5);
  }
  else {
    display.print("---");
  }

  // =========================
  // Row 4 - Speed
  // =========================

  display.setCursor(0, 36);
  display.print("SPD:");

  if (gps.speed.isValid()) {
    display.print(gps.speed.kmph(), 1);
    display.print("km/h");
  }
  else {
    display.print("---");
  }

  // =========================
  // Current GPS Time + Altitude
  // =========================

  display.setCursor(70, 36);
  display.print("ALT:");

  if (gps.altitude.isValid()) {
    display.print(gps.altitude.meters(), 0);
    display.print("m");
  }
  else {
    display.print("---");
  }

  // Current GPS time
  display.setCursor(0, 48);
  display.print("TIME:");

  if (gps.time.isValid()) {

    if (gps.time.hour() < 10)
      display.print("0");

    display.print(gps.time.hour());
    display.print(":");

    if (gps.time.minute() < 10)
      display.print("0");

    display.print(gps.time.minute());
    display.print(":");

    if (gps.time.second() < 10)
      display.print("0");

    display.print(gps.time.second());

  }
  else {
    display.print("--:--:--");
  }

  // =========================
  // Last Sync
  // =========================

  display.setCursor(0, 60);
  display.print("SYNC:");

  if (syncAvailable) {

    if (syncHour < 10)
      display.print("0");

    display.print(syncHour);
    display.print(":");

    if (syncMinute < 10)
      display.print("0");

    display.print(syncMinute);
    display.print(":");

    if (syncSecond < 10)
      display.print("0");

    display.print(syncSecond);

  }
  else {
    display.print("--:--:--");
  }

  display.display();
}