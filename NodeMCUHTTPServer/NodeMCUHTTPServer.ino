#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "networkbda0917cffgpiwr8645998746";
const char* password = "Omshree1225*";

ESP8266WebServer server(80);

void handleRoot() {
    server.send(200, "text/html", "<h1>ESP8266 is Running!</h1>");
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    
    Serial.println("\nConnecting to WiFi...");
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ Connected!");
        Serial.print("🌐 IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n❌ Connection Failed! Restarting...");
        ESP.restart();
    }

    server.on("/", handleRoot);
    server.begin();
    Serial.println("✅ HTTP Server Started!");
}

void loop() {
    server.handleClient();
}
