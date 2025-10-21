#include <WiFi.h>
#include <PubSubClient.h>

// -------- Wi-Fi Credentials --------
const char* ssid = "TUSHAR-PC";
const char* password = "6307778096";

// -------- ThingsBoard MQTT Configuration --------
const char* mqttServer = "thingsboard.cloud";
const int mqttPort = 1883;
const char* accessToken = "u0robmwfchhfm9di5xay";  // Your device token

// -------- Hardware Pin Configuration --------
#define LED_PIN 5
#define VOLTAGE_PIN 34   // ZMPT101B Analog OUT
#define CURRENT_PIN 35   // ACS712 Analog OUT

// -------- Global Variables --------
WiFiClient wifiClient;
PubSubClient client(wifiClient);
int ledState = 0;
float voltageRMS = 0;
float currentRMS = 0;
float power = 0;

// -------- Calibration Factors --------
// Tune these based on your calibration results
float ZMPT_SENSITIVITY = 0.012;  // For ZMPT101B module (adjust after testing)
float ACS_SENSITIVITY = 0.066;   // 30A → 0.066, 20A → 0.1, 5A → 0.185 (V/A)
float ADC_REF = 3.3;
int ADC_RES = 4095;

// -------- Function Prototypes --------
void setup_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
float readVoltageRMS();
float readCurrentRMS();

// -------- Wi-Fi Setup --------
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("✅ Wi-Fi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// -------- MQTT Callback (Handles RPC) --------
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) message += (char)payload[i];
  Serial.print("\n📩 Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  if (String(topic).startsWith("v1/devices/me/rpc/request/")) {
    int requestIdStart = String(topic).lastIndexOf('/') + 1;
    String requestId = String(topic).substring(requestIdStart);

    // --- Handle setState RPC ---
    if (message.indexOf("\"method\":\"setState\"") != -1) {
      bool newState = message.indexOf("true") != -1;
      ledState = newState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);

      Serial.print("💡 LED set to: ");
      Serial.println(ledState ? "ON" : "OFF");

      String responseTopic = "v1/devices/me/rpc/response/" + requestId;
      String response = String("{\"ledState\":") + (ledState ? "true" : "false") + "}";
      client.publish(responseTopic.c_str(), response.c_str());
    }

    // --- Handle getState RPC ---
    else if (message.indexOf("\"method\":\"getState\"") != -1) {
      String responseTopic = "v1/devices/me/rpc/response/" + requestId;
      String response = String("{\"ledState\":") + (ledState ? "true" : "false") + "}";
      client.publish(responseTopic.c_str(), response.c_str());
      Serial.println("📤 Sent current LED state to ThingsBoard.");
    }
  }
}

// -------- MQTT Reconnect --------
void reconnect() {
  while (!client.connected()) {
    Serial.print("🔄 Connecting to ThingsBoard... ");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), accessToken, "")) {
      Serial.println("✅ Connected to ThingsBoard.");
      client.subscribe("v1/devices/me/rpc/request/+");
    } else {
      Serial.print("❌ Failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5s.");
      delay(5000);
    }
  }
}

// -------- Measure RMS Voltage --------
float readVoltageRMS() {
  long sum = 0;
  const int samples = 300;
  for (int i = 0; i < samples; i++) {
    int adcValue = analogRead(VOLTAGE_PIN);
    float voltage = (adcValue * ADC_REF / ADC_RES) - (ADC_REF / 2.0);
    sum += voltage * voltage;
    delayMicroseconds(800);  // faster sampling for 2-sec interval
  }
  float mean = sum / (float)samples;
  float rms = sqrt(mean) / ZMPT_SENSITIVITY;
  return rms;
}

// -------- Measure RMS Current --------
float readCurrentRMS() {
  long sum = 0;
  const int samples = 300;
  for (int i = 0; i < samples; i++) {
    int adcValue = analogRead(CURRENT_PIN);
    float current = (adcValue * ADC_REF / ADC_RES) - (ADC_REF / 2.0);
    sum += current * current;
    delayMicroseconds(800);
  }
  float mean = sum / (float)samples;
  float rms = sqrt(mean) / ACS_SENSITIVITY;
  return rms;
}

// -------- Setup --------
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  reconnect();
  Serial.println("🚀 System Ready: LED + Power Monitoring");
}

// -------- Main Loop --------
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // Read sensors every 2 seconds
  static unsigned long lastRead = 0;
  if (millis() - lastRead > 2000) {
    lastRead = millis();

    voltageRMS = readVoltageRMS();
    currentRMS = readCurrentRMS();
    power = voltageRMS * currentRMS;

    Serial.println("📊 Sensor Readings:");
    Serial.print("Voltage: "); Serial.print(voltageRMS); Serial.println(" V");
    Serial.print("Current: "); Serial.print(currentRMS); Serial.println(" A");
    Serial.print("Power: "); Serial.print(power); Serial.println(" W");

    // Send telemetry to ThingsBoard
    String payload = "{";
    payload += "\"voltage\":" + String(voltageRMS, 2) + ",";
    payload += "\"current\":" + String(currentRMS, 3) + ",";
    payload += "\"power\":" + String(power, 2) + ",";
    payload += "\"ledState\":" + String(ledState ? "true" : "false");
    payload += "}";

    client.publish("v1/devices/me/telemetry", payload.c_str());
    Serial.println("📡 Sent to ThingsBoard: " + payload);
  }
}
