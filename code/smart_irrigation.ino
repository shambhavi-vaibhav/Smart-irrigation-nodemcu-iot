#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "secrets.h"   // copy secrets.h.example -> secrets.h

// OLED via U8G2 (I2C: D1 = SCL, D2 = SDA on NodeMCU)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// === Pins ===
#define DHTPIN   D4
#define DHTTYPE  DHT11
#define SOIL_PIN A0
#define IN1      D5
#define IN2      D6

// === WiFi ===
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;

ESP8266WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

// === Variables ===
bool  autoMode    = true;
bool  motorStatus = false;
float temperature = 0;
float humidity    = 0;
int   soilValue   = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  stopPump();            // Initial state OFF
  u8g2.begin();          // OLED init
  connectWiFi();
  setupWebServer();
}

void loop() {
  readSensors();
  handleMotorLogic();
  updateOLED();
  server.handleClient();
  delay(2000);
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect.");
  }
}

void readSensors() {
  temperature = dht.readTemperature();
  humidity    = dht.readHumidity();
  soilValue   = analogRead(SOIL_PIN);
}

void handleMotorLogic() {
  if (autoMode) {
    motorStatus = (soilValue > 700);   // higher reading = drier soil
  }
  motorStatus ? startPump() : stopPump();
}

void startPump() { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); }
void stopPump()  { digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW); }  // both LOW = OFF

void updateOLED() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setCursor(0, 10); u8g2.print("Temp: "); u8g2.print(temperature); u8g2.print(" C");
  u8g2.setCursor(0, 22); u8g2.print("Hum : "); u8g2.print(humidity);    u8g2.print(" %");
  u8g2.setCursor(0, 34); u8g2.print("Soil: "); u8g2.print(soilValue);
  u8g2.setCursor(0, 46); u8g2.print("Mode: "); u8g2.print(autoMode ? "Auto" : "Manual");
  u8g2.setCursor(0, 58); u8g2.print("Pump: "); u8g2.print(motorStatus ? "ON" : "OFF");
  u8g2.sendBuffer();
}

void setupWebServer() {
  server.on("/", HTTP_GET, []() {
    String html = R"rawliteral(
<html>
<head>
<meta http-equiv='refresh' content='5'>
<title>Smart Irrigation</title>
<style>
body { font-family: sans-serif; text-align: center; margin-top: 40px; }
button { padding: 10px 20px; margin: 10px; font-size: 16px; }
</style>
</head>
<body>
<h2>Smart Irrigation Dashboard</h2>
<p><b>Temperature:</b> %TEMP% &deg;C</p>
<p><b>Humidity:</b> %HUM% %</p>
<p><b>Soil Moisture:</b> %SOIL%</p>
<p><b>Motor:</b> %MOTOR%</p>
<p><b>Mode:</b> %MODE%</p>
<a href="/mode?set=%TOGGLEMODE%"><button>Switch to %TOGGLEMODE%</button></a>
%MANUALBTN%
</body>
</html>
)rawliteral";

    String manualBtn = "";
    if (!autoMode) {
      manualBtn += "<a href='/motor?set=" + String(motorStatus ? "off" : "on") + "'><button>Turn ";
      manualBtn += motorStatus ? "OFF" : "ON";
      manualBtn += " Motor</button></a>";
    }
    html.replace("%TEMP%", String(temperature));
    html.replace("%HUM%", String(humidity));
    html.replace("%SOIL%", String(soilValue));
    html.replace("%MOTOR%", motorStatus ? "ON" : "OFF");
    html.replace("%MODE%", autoMode ? "Auto" : "Manual");
    html.replace("%TOGGLEMODE%", autoMode ? "manual" : "auto");
    html.replace("%MANUALBTN%", manualBtn);
    server.send(200, "text/html", html);
  });

  server.on("/mode", HTTP_GET, []() {
    autoMode = (server.arg("set") == "auto");
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.on("/motor", HTTP_GET, []() {
    String cmd = server.arg("set");
    if (!autoMode) {
      motorStatus = (cmd == "on");
      motorStatus ? startPump() : stopPump();
    }
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.begin();
}
