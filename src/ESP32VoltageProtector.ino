#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WebServer.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
WebServer server(80);

// Wi-Fi Access Point
const char* ssid = "VoltageProtector";
const char* password = "Protect2026";

// GPIO pins
#define VOLTAGE_PIN 34
#define CURRENT_PIN 35
#define GREEN_LED 25
#define RED_LED 26

#define MODE_BTN 18
#define UP_BTN 19
#define DOWN_BTN 23

// Protection settings
float maxVoltage = 250.0;
float maxCurrent = 8.0;

float voltage = 0;
float current = 0;

enum State { INIT, NORMAL, TRIPPED, RESET_READY };
State state = INIT;

String faultReason = "NONE";

int page = 0;
unsigned long lastSample = 0;
unsigned long lastLCD = 0;
unsigned long safeSince = 0;

bool lastMode = false;
bool lastUp = false;
bool lastDown = false;
bool longPressHandled = false;

unsigned long modeStart = 0;
unsigned long lastUpPress = 0;
unsigned long lastDownPress = 0;

// ---------------- SENSOR ----------------

void readSensors() {
  voltage = analogRead(VOLTAGE_PIN) * 300.0 / 4095.0;
  current = analogRead(CURRENT_PIN) * 15.0 / 4095.0;
}

bool hasFault() {
  return voltage > maxVoltage || current > maxCurrent;
}

bool isSafe() {
  return voltage <= maxVoltage - 5.0 &&
         current <= maxCurrent - 0.5;
}

String detectFault() {
  bool ov = voltage > maxVoltage;
  bool oc = current > maxCurrent;

  if (ov && oc) return "BOTH";
  if (ov) return "OVERVOLTAGE";
  if (oc) return "OVERCURRENT";
  return "NONE";
}

// ---------------- FSM ----------------

void updateFSM() {
  switch (state) {
    case INIT:
      if (hasFault()) {
        faultReason = detectFault();
        state = TRIPPED;
      } else {
        state = NORMAL;
      }
      break;

    case NORMAL:
      if (hasFault()) {
        faultReason = detectFault();
        state = TRIPPED;
        safeSince = 0;

        Serial.println("TRIPPED: " + faultReason);
      }
      break;

    case TRIPPED:
      if (isSafe()) {
        if (safeSince == 0) safeSince = millis();

        if (millis() - safeSince >= 1000) {
          state = RESET_READY;
          Serial.println("RESET READY");
        }
      } else {
        safeSince = 0;
      }
      break;

    case RESET_READY:
      if (!isSafe()) {
        state = TRIPPED;
        safeSince = 0;
      }
      break;
  }
}

bool resetProtection() {
  if (state == RESET_READY && isSafe()) {
    state = NORMAL;
    faultReason = "NONE";
    safeSince = 0;
    Serial.println("PROTECTION RESET");
    return true;
  }
  return false;
}

// ---------------- BUTTONS ----------------

bool buttonPressed(int pin, bool &previous,
                   unsigned long &lastPress) {
  bool down = digitalRead(pin) == LOW;

  bool pressed = down && !previous &&
                 millis() - lastPress >= 180;

  previous = down;
  if (pressed) lastPress = millis();

  return pressed;
}

void handleButtons() {
  bool modeDown = digitalRead(MODE_BTN) == LOW;

  if (modeDown && !lastMode) {
    modeStart = millis();
    longPressHandled = false;
  }

  if (modeDown && !longPressHandled &&
      millis() - modeStart >= 1200) {
    longPressHandled = true;
    resetProtection();
  }

  if (!modeDown && lastMode && !longPressHandled &&
      millis() - modeStart >= 40) {
    page = (page + 1) % 3;
  }

  lastMode = modeDown;

  if (buttonPressed(UP_BTN, lastUp, lastUpPress)) {
    if (page == 1 && maxVoltage < 290)
      maxVoltage += 5;

    if (page == 2 && maxCurrent < 14)
      maxCurrent += 0.5;
  }

  if (buttonPressed(DOWN_BTN, lastDown, lastDownPress)) {
    if (page == 1 && maxVoltage > 200)
      maxVoltage -= 5;

    if (page == 2 && maxCurrent > 1)
      maxCurrent -= 0.5;
  }
}

// ---------------- LED ----------------

void updateLEDs() {
  digitalWrite(GREEN_LED, state == NORMAL);

  bool redOn = state == TRIPPED ||
    (state == RESET_READY && (millis() / 400) % 2 == 0);

  digitalWrite(RED_LED, redOn);
}

// ---------------- LCD ----------------

void lcdLine(int row, String text) {
  lcd.setCursor(0, row);

  if (text.length() > 16)
    text = text.substring(0, 16);

  lcd.print(text);

  for (int i = text.length(); i < 16; i++)
    lcd.print(" ");
}

void updateLCD() {
  if (page == 0) {
    lcdLine(0, "V:" + String(voltage, 0) +
               " I:" + String(current, 1));

    if (state == NORMAL)
      lcdLine(1, "NORMAL LOAD ON");
    else if (state == TRIPPED)
      lcdLine(1, "TRIPPED LOAD OFF");
    else if (state == RESET_READY)
      lcdLine(1, "READY HOLD MODE");
    else
      lcdLine(1, "INITIALIZING");
  }

  else if (page == 1) {
    lcdLine(0, "SET MAX VOLTAGE");
    lcdLine(1, String(maxVoltage, 0) + "V  UP/DOWN");
  }

  else if (page == 2) {
    lcdLine(0, "SET MAX CURRENT");
    lcdLine(1, String(maxCurrent, 1) + "A  UP/DOWN");
  }
}

// ---------------- WEB PAGE ----------------

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 Voltage Protector</title>
<style>
body {
  font-family: Arial, sans-serif;
  background: #eef2f7;
  color: #172033;
  text-align: center;
  margin: 0;
  padding: 20px;
}
.card {
  background: white;
  border-radius: 12px;
  padding: 18px;
  margin: 14px auto;
  max-width: 420px;
  box-shadow: 0 2px 8px #0002;
}
.value { font-size: 30px; font-weight: bold; }
input {
  padding: 10px;
  width: 90px;
  margin: 8px;
  font-size: 16px;
}
button {
  padding: 12px 22px;
  margin: 8px;
  border: none;
  border-radius: 7px;
  background: #1769dd;
  color: white;
  font-size: 15px;
  cursor: pointer;
}
.reset { background: #d63b37; }
</style>
</head>
<body>

<h2>ESP32 Voltage Protector</h2>

<div class="card">
  <h3>Live Measurements</h3>
  <p>Voltage</p>
  <div class="value"><span id="voltage">--</span> V</div>
  <p>Current</p>
  <div class="value"><span id="current">--</span> A</div>
</div>

<div class="card">
  <h3>Protection Status</h3>
  <h2 id="state">Connecting...</h2>
  <p>Load: <b id="load">--</b></p>
  <p>Last fault: <span id="fault">--</span></p>
</div>

<div class="card">
  <h3>Protection Settings</h3>
  <p>Maximum Voltage (V)</p>
  <input type="number" id="vmax" min="200" max="290" step="5">

  <p>Maximum Current (A)</p>
  <input type="number" id="imax" min="1" max="14" step="0.5">

  <br>
  <button onclick="saveSettings()">Save Settings</button>
  <p id="message"></p>
</div>

<div class="card">
  <h3>Fault Reset</h3>
  <button class="reset" onclick="resetDevice()">
    RESET PROTECTION
  </button>
</div>

<script>
async function updateData() {
  try {
    const r = await fetch('/status');
    const d = await r.json();

    document.getElementById('voltage').textContent = d.voltage;
    document.getElementById('current').textContent = d.current;
    document.getElementById('state').textContent = d.state;
    document.getElementById('load').textContent = d.load;
    document.getElementById('fault').textContent = d.fault;

    const v = document.getElementById('vmax');
    const i = document.getElementById('imax');

    if (document.activeElement !== v) v.value = d.vmax;
    if (document.activeElement !== i) i.value = d.imax;

    document.getElementById('state').style.color =
      d.state === "NORMAL" ? "green" : "red";

  } catch (e) {
    document.getElementById('state').textContent = "Disconnected";
  }
}

async function saveSettings() {
  const v = document.getElementById('vmax').value;
  const i = document.getElementById('imax').value;

  const body = new URLSearchParams({v: v, i: i});

  const r = await fetch('/set', {
    method: 'POST',
    body: body
  });

  document.getElementById('message').textContent = await r.text();
  updateData();
}

async function resetDevice() {
  const r = await fetch('/reset', {method: 'POST'});
  document.getElementById('message').textContent = await r.text();
  updateData();
}

setInterval(updateData, 500);
updateData();
</script>

</body>
</html>
)rawliteral";

// ---------------- WEB API ----------------

String stateName() {
  if (state == NORMAL) return "NORMAL";
  if (state == TRIPPED) return "TRIPPED";
  if (state == RESET_READY) return "RESET READY";
  return "INIT";
}

void handleStatus() {
  String json = "{";

  json += "\"voltage\":" + String(voltage, 1);
  json += ",\"current\":" + String(current, 2);
  json += ",\"vmax\":" + String(maxVoltage, 1);
  json += ",\"imax\":" + String(maxCurrent, 1);
  json += ",\"state\":\"" + stateName() + "\"";
  json += ",\"fault\":\"" + faultReason + "\"";
  json += ",\"load\":\"";
  json += (state == NORMAL ? "ON" : "OFF");
  json += "\"}";

  server.send(200, "application/json", json);
}

void handleSet() {
  if (!server.hasArg("v") || !server.hasArg("i")) {
    server.send(400, "text/plain", "Missing parameters");
    return;
  }

  float v = server.arg("v").toFloat();
  float i = server.arg("i").toFloat();

  if (v < 200 || v > 290 || i < 1 || i > 14) {
    server.send(400, "text/plain", "Invalid limits");
    return;
  }

  maxVoltage = v;
  maxCurrent = i;

  updateFSM();

  Serial.println("Settings updated via WiFi");
  server.send(200, "text/plain", "Settings saved!");
}

void handleReset() {
  if (resetProtection())
    server.send(200, "text/plain", "Reset successful!");
  else
    server.send(409, "text/plain", "Not safe or not reset-ready");
}

// ---------------- SETUP ----------------

void setup() {
  Serial.begin(115200);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  pinMode(MODE_BTN, INPUT_PULLUP);
  pinMode(UP_BTN, INPUT_PULLUP);
  pinMode(DOWN_BTN, INPUT_PULLUP);

  analogReadResolution(12);

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  lcdLine(0, "ESP32 PROTECTOR");
  lcdLine(1, "STARTING...");

  readSensors();
  updateFSM();

  // Start ESP32 Wi-Fi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  // Web routes
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", webpage);
  });

  server.on("/status", HTTP_GET, handleStatus);
  server.on("/set", HTTP_POST, handleSet);
  server.on("/reset", HTTP_POST, handleReset);

  server.begin();

  Serial.println();
  Serial.println("WiFi Started!");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());
}

// ---------------- MAIN LOOP ----------------

void loop() {
  handleButtons();

  if (millis() - lastSample >= 30) {
    lastSample = millis();

    readSensors();
    updateFSM();
  }

  updateLEDs();

  server.handleClient();

  if (millis() - lastLCD >= 250) {
    lastLCD = millis();
    updateLCD();
  }
}
