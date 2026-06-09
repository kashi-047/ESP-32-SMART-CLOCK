#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>
#include <Preferences.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define BUZZER 4
#define TOUCH_PIN 15

#define DHTPIN 5
#define DHTTYPE DHT11

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);
Preferences prefs;

const char* ssid = "Keralavision-Wifi-2.4G";
const char* password = "04af8f9a";

WebServer server(80);

String alarmTime = "07:00";

bool alarmTriggered = false;
bool alarmRinging = false;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;

// =================================================
// INNO Animation
// =================================================
void showInnoAnimation() {

  for (int x = -60; x < 25; x += 4) {

    display.clearDisplay();

    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(x, 20);
    display.print("INNO");

    display.display();

    delay(80);
  }

  delay(1000);
}

// =================================================
// Save Alarm
// =================================================
void saveAlarm() {

  prefs.begin("clock", false);
  prefs.putString("alarm", alarmTime);
  prefs.end();
}

// =================================================
// Load Alarm
// =================================================
void loadAlarm() {

  prefs.begin("clock", true);
  alarmTime = prefs.getString("alarm", "07:00");
  prefs.end();
}

// =================================================
// Web Page
// =================================================
void handleRoot() {

  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>INNO Smart Clock</title>

<style>
body{
margin:0;
padding:0;
height:100vh;
display:flex;
justify-content:center;
align-items:center;
font-family:Arial;
background:linear-gradient(135deg,#0f2027,#203a43,#2c5364);
}

.card{
background:white;
padding:25px;
width:320px;
border-radius:20px;
text-align:center;
box-shadow:0 0 25px rgba(0,0,0,.4);
}

h1{
color:#203a43;
}

input{
width:90%;
padding:12px;
font-size:18px;
border-radius:10px;
border:1px solid #ccc;
}

button{
margin-top:15px;
padding:12px 25px;
border:none;
border-radius:10px;
background:#203a43;
color:white;
cursor:pointer;
}

button:hover{
background:#2c5364;
}
</style>

</head>
<body>

<div class="card">

<h1>🕒 INNO Smart Clock</h1>

<form action="/set">

<input type="time" name="alarm">

<br>

<button type="submit">
Set Alarm
</button>

</form>

<h3>Current Alarm:</h3>
)rawliteral";

  page += alarmTime;

  page += R"rawliteral(
</div>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", page);
}

// =================================================
// Set Alarm
// =================================================
void handleSet() {

  if (server.hasArg("alarm")) {

    alarmTime = server.arg("alarm");

    saveAlarm();

    alarmTriggered = false;
  }

  server.sendHeader("Location", "/");
  server.send(303);
}

// =================================================
// WiFi Connect
// =================================================
void connectWiFi() {

  WiFi.begin(ssid, password);

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startTime < 15000) {

    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {

    configTime(
      gmtOffset_sec,
      daylightOffset_sec,
      ntpServer
    );
  }
}

// =================================================
// Alarm Ring
// =================================================
void ringAlarm() {

  alarmRinging = true;

  while (alarmRinging) {

    digitalWrite(BUZZER, HIGH);
    delay(200);

    digitalWrite(BUZZER, LOW);
    delay(200);

    server.handleClient();

    if (digitalRead(TOUCH_PIN) == HIGH) {

      alarmRinging = false;
      alarmTriggered = true;

      digitalWrite(BUZZER, LOW);

      delay(300);
    }
  }
}

// =================================================
// Setup
// =================================================
void setup() {

  Serial.begin(115200);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  pinMode(TOUCH_PIN, INPUT);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  dht.begin();

  loadAlarm();

  display.clearDisplay();
  display.display();

  showInnoAnimation();

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  connectWiFi();

  server.on("/", handleRoot);
  server.on("/set", handleSet);

  server.begin();

  Serial.println("System Started");
}

// =================================================
// Loop
// =================================================
void loop() {

  server.handleClient();

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 25);
    display.println("Waiting For Time");
    display.display();

    delay(1000);
    return;
  }

  char displayTime[12];
  strftime(displayTime,
           sizeof(displayTime),
           "%I:%M %p",
           &timeinfo);

  char alarmCheckTime[6];
  strftime(alarmCheckTime,
           sizeof(alarmCheckTime),
           "%H:%M",
           &timeinfo);

  static unsigned long screenTimer = 0;
  static bool showClockScreen = true;

  if (millis() - screenTimer >= 3000) {

    showClockScreen = !showClockScreen;
    screenTimer = millis();
  }

  display.clearDisplay();

  if (showClockScreen) {

    display.setTextSize(1);
    display.setCursor(98, 0);
    display.print("INNO");

    display.setTextSize(2);
    display.setCursor(5, 18);
    display.print(displayTime);

    display.setTextSize(1);
    display.setCursor(5, 50);
    display.print("Alarm: ");
    display.print(alarmTime);
  }
  else {

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    display.setTextSize(1);
    display.setCursor(30, 0);
    display.print("DHT11 DATA");

    display.setTextSize(2);
    display.setCursor(0, 18);
    display.print("T:");
    display.print(temperature, 1);
    display.print("C");

    display.setTextSize(2);
    display.setCursor(0, 42);
    display.print("H:");
    display.print(humidity, 0);
    display.print("%");
  }

  display.display();

  String currentTime = String(alarmCheckTime);

  if (currentTime == alarmTime &&
      !alarmTriggered) {

    ringAlarm();
  }

  if (currentTime != alarmTime) {
    alarmTriggered = false;
  }

  delay(200);
}