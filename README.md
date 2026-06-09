**INNO Smart Clock** 

An ESP32-based Smart Clock featuring an OLED display, Wi-Fi time synchronization, web-controlled alarm setting, touch sensor alarm cancellation, and temperature/humidity monitoring.

**Features**

* Real-time clock using NTP
* SSD1306 OLED Display
* Web-based alarm setting
* Alarm stored in ESP32 flash memory
* Touch sensor to stop alarm
* DHT11 Temperature & Humidity monitoring
* 12-hour AM/PM clock display
* Automatic screen switching every 3 seconds
* INNO startup animation

**Hardware Required**

* ESP32 Dev Module
* SSD1306 OLED Display (128×64 I2C)
* DHT11 Sensor
* TTP223 Touch Sensor
* Active Buzzer
* Jumper Wires

**Pin Connections**

| Component        | ESP32 Pin |
| ---------------- | --------- |
| OLED SDA         | GPIO 21   |
| OLED SCL         | GPIO 22   |
| DHT11 DATA       | GPIO 5    |
| Buzzer           | GPIO 4    |
| Touch Sensor OUT | GPIO 15   |

**Libraries Used**

* WiFi.h
* WebServer.h
* Wire.h
* Adafruit_GFX.h
* Adafruit_SSD1306.h
* Preferences.h
* DHT.h
* time.h

**Working**

1. ESP32 connects to Wi-Fi.
2. Time is synchronized using NTP.
3. User sets an alarm from a web page.
4. Alarm is stored permanently in flash memory.
5. OLED alternates between:
   * Time + Alarm
   * Temperature + Humidity
6. When alarm time is reached, the buzzer rings.
7. Touching the TTP223 sensor stops the alarm.

**Future Improvements**

* Multiple alarms
* RTC backup using DS3231
* Weather updates
* Mobile app integration
* Battery backup support

**Author**

Kashinath A

INNO Projects
