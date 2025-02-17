#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "networkbda0917cffgpiwr8645998746";
const char* password = "Omshree1225*";
const char* serverURL = "http://192.168.0.104:8080/catfeeder/server.php";  // Use your PC’s IP

ESP8266WebServer server(80);
#define MIC_PIN A0  
#define MEOW_THRESHOLD 600  
bool meowDetected = false;

void sendMeowToServer() {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, String(serverURL));
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST("action=log_meow");

    if (httpCode > 0) {
        Serial.println("✅ Meow Logged to Server!");
    } else {
        Serial.println("❌ Failed to log meow.");
    }

    http.end();
}

void handleMicData() {
    int micValue = analogRead(MIC_PIN);
    Serial.print("🎤 MIC Value: ");
    Serial.println(micValue);

    if (micValue > MEOW_THRESHOLD) {
        Serial.println("🐱 Meow Detected!");
        meowDetected = true;
        sendMeowToServer();  // Send meow event to PHP server
    } else {
        meowDetected = false;
    }

    server.send(200, "text/plain", String(micValue));
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\n✅ Connected!");
    Serial.print("🌐 IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", []() { server.send(200, "text/plain", "ESP8266 Cat Feeder!"); });
    server.on("/micdata", handleMicData);
    server.begin();
    Serial.println("✅ HTTP Server Started!");
}

void loop() {
    server.handleClient();
}
