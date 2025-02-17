#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "OPPO A16";  // Replace with your WiFi SSID
const char* password = "omiomiom";  // Replace with your WiFi password

ESP8266WebServer server(80);  // Web server on port 80

#define MIC_PIN A0  // MIC module connected to A0
int micValue = 0;  // Variable to store MIC readings

// Serve the Webpage
void handleRoot() {
    String page = "<html><head>";
    page += "<script>";
    page += "function updateMicData() {";
    page += "  fetch('/micdata')";  // Request MIC data from server
    page += "    .then(response => response.text())";
    page += "    .then(data => { document.getElementById('mic_value').innerText = data; });";
    page += "}";
    page += "setInterval(updateMicData, 500);";  // Auto-refresh every 500ms
    page += "</script></head>";
    page += "<body><h1>MIC Sensor Data</h1>";
    page += "<p>Current Sound Level: <b id='mic_value'>Loading...</b></p>";
    page += "<br><a href='/trigger'><button>Trigger Action</button></a>";
    page += "</body></html>";

    server.send(200, "text/html", page);
}

// Serve MIC Data (Used by AJAX)
void handleMicData() {
    micValue = analogRead(MIC_PIN);
    Serial.print("🎤 MIC Value: ");
    Serial.println(micValue);
    server.send(200, "text/plain", String(micValue));
}

// Handle Button Click (Trigger Action)
void handleTrigger() {
    Serial.println("🚀 Trigger received! Performing action...");
    server.send(200, "text/plain", "Trigger received!");
}

void setup() {
    Serial.begin(115200);
    Serial.println("\nStarting...");
    
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\n✅ Connected to WiFi!");
    Serial.print("🌐 IP Address: ");
    Serial.println(WiFi.localIP());

    // Set up Web Server Routes
    server.on("/", handleRoot);
    server.on("/micdata", handleMicData);  // New endpoint for live MIC data
    server.on("/trigger", handleTrigger);

    server.begin();
    Serial.println("✅ HTTP Server Started!");
}

void loop() {
    server.handleClient();
}
