# ESP32 GPS Tracker with NEO-M8N and SH1106 OLED

A simple GPS information display built with an **ESP32**, a **u-blox NEO-M8N GPS module** and a **1.3" SH1106 OLED (128x64)**. It shows the fix status, number of satellites, coordinates, speed, altitude, live GPS time and the time of the last successful synchronization.

---

## Features

- Live GPS fix status (`FIX` / `SEARCH`)
- Number of satellites in use
- Latitude and longitude (5 decimal places)
- Speed in km/h
- Altitude in meters
- Live GPS date/time (UTC)
- Last successful sync timestamp (kept on screen even if the fix is lost)
- Serial debug output at 115200 baud

---

## Hardware

| Component | Notes |
|-----------|-------|
| ESP32 DevKit (30-pin) | Any ESP32 board with UART2 works |
| GY-GPSV3 (NEO-M8N) | u-blox NEO-M8N with external active antenna |
| SH1106 OLED 1.3" | 128x64, I2C, address `0x3C` |
| Breadboard + jumper wires | |
| Micro USB cable | Power and programming |

---

## Wiring

### GPS module to ESP32

| GPS Pin | ESP32 Pin | Description |
|---------|-----------|-------------|
| VCC | 3V3 (or 5V) | Power (check your module's regulator) |
| GND | GND | Ground |
| TX | GPIO 16 (RX2) | GPS TX goes to ESP32 RX |
| RX | GPIO 17 (TX2) | GPS RX goes to ESP32 TX |

> TX and RX are crossed: the GPS **TX** connects to the ESP32 **RX**, and vice versa.

### OLED to ESP32

| OLED Pin | ESP32 Pin |
|----------|-----------|
| VCC | 3V3 |
| GND | GND |
| SCK (SCL) | GPIO 22 |
| SDA | GPIO 21 |

```
        ESP32                     NEO-M8N
     ┌─────────┐              ┌───────────┐
     │  GPIO16 │◄─────────────│ TX        │
     │  GPIO17 │─────────────►│ RX        │
     │     3V3 │──────────────│ VCC       │
     │     GND │──────────────│ GND       │
     │         │              └───────────┘
     │         │                 SH1106 OLED
     │  GPIO22 │──────────────│ SCK       │
     │  GPIO21 │──────────────│ SDA       │
     │     3V3 │──────────────│ VCC       │
     │     GND │──────────────│ GND       │
     └─────────┘
```

---

## Software Setup

### Arduino IDE

1. Install the **ESP32 board package** (Boards Manager, "esp32" by Espressif).
2. Select the board **ESP32 Dev Module**.
3. Install these libraries from the Library Manager:
   - `Adafruit GFX Library`
   - `Adafruit SH110X`
   - `TinyGPSPlus` (by Mikal Hart)
4. Open `esp32_gps_oled.ino`, select the correct COM port and upload.

---


## Display Layout

```
FIX                 SAT:7
LAT:31.96113
LON:35.90791
SPD:0.1km/h  ALT:898m
TIME:15:28:11
SYNC:15:28:11
```

| Row | Content |
|-----|---------|
| 1 | Fix status and satellite count |
| 2 | Latitude |
| 3 | Longitude |
| 4 | Speed and altitude |
| 5 | Current GPS time |
| 6 | Time of the last successful sync |

---

## How It Works

1. The NEO-M8N sends **NMEA sentences** over UART at 9600 baud.
2. The ESP32 reads them on **UART2** (`HardwareSerial(1)`, pins 16/17).
3. **TinyGPS++** parses the NMEA data into location, speed, altitude, date and time.
4. When a valid location, date and time arrive together, the values are saved as the **last sync**.
5. Every 500 ms the OLED is cleared and redrawn with the current values.

---

## Notes and Tips

- **Time is UTC.** GPS time has no time zone. For Jordan (UTC+3), add 3 hours in code if you want local time.
- **Cold start** can take from 30 seconds to several minutes. Keep the antenna outdoors or near a window with a clear sky view.
- **Speed** shows small values (like 0.1 km/h) when stationary because of GPS noise.
- If the OLED stays blank, check the I2C address with an I2C scanner (`0x3C` or `0x3D`).
- If you see no GPS data, swap the TX/RX wires and verify the baud rate (9600 default).
- Note: the code uses `gps.location.isUpdated()` to trigger a sync, so the date/time are only saved when a new position arrives.

---

## Troubleshooting

| Problem | Possible cause | Fix |
|---------|----------------|-----|
| `OLED initialization failed` | Wrong address or wiring | Check SDA/SCK, run an I2C scanner |
| Always `SEARCH`, `SAT:0` | No sky view or antenna issue | Move outdoors, wait a few minutes |
| `SAT` shows but no `FIX` | Not enough satellites yet | Wait for at least 4 satellites |
| Garbage on serial | Wrong baud rate | Use 115200 for the Serial Monitor |
| No data from GPS | TX/RX not crossed | Swap the GPS TX/RX wires |

---

## Ideas for Improvement

- Add local time zone offset
- Log coordinates to an SD card
- Show heading and a compass arrow
- Display multiple pages with a button
- Send position over Wi-Fi/MQTT or show it on a map

---

## Project Structure

```
esp32-gps-oled/
├── esp32_gps_oled.ino
├── images/
│   ├── breadboard.jpg
│   └── display.jpg
└── README.md
```

---

## License

MIT License. Free to use, modify and share.
