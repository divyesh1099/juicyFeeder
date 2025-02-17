#include <ArduinoWiFiServer.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "OPPO A16";  // Replace with your WiFi SSID
const char* password = "omiomiom";  // Replace with your WiFi password

ESP8266WebServer server(80);  // Create a web server on port 80

void handleRoot() {
    server.send(200, "text/html", "<h1>NodeMCU Web Server</h1><p>Click <a href='/trigger'>here</a> to trigger action.</p>");
}

void handleTrigger() {
    server.send(200, "text/plain", "Trigger received!");
    Serial.println("Trigger received!");
}

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nConnected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/trigger", handleTrigger);

    server.begin();
    Serial.println("HTTP Server Started!");
}

void loop() {
    server.handleClient();
}
