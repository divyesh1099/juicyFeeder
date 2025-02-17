#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

const char* ssid = "networkbda0917cffgpiwr8645998746";
const char* password = "Omshree1225*";
const char* serverURL = "http://192.168.0.104:8080/catfeeder/server.php";

ESP8266WebServer server(80);
Servo myServo;

#define MIC_PIN A0  
#define MEOW_THRESHOLD 600  
#define SERVO_PIN D4  
#define CHECK_INTERVAL 5000  

bool meowDetected = false;
unsigned long lastCheckTime = 0;
String mode = "meow";
String scheduleTimes[2] = {"08:00", "20:00"};

void moveServo() {
    myServo.write(180);
    delay(1000);
    myServo.write(0);
}

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

void checkServerSettings() {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, String(serverURL));

    int httpCode = http.GET();
    if (httpCode > 0) {
        String payload = http.getString();
        Serial.println("Server Response: " + payload);
        
        int modeIndex = payload.indexOf("\"mode\":\"");
        if (modeIndex > 0) {
            mode = payload.substring(modeIndex + 7, payload.indexOf("\"", modeIndex + 7));
            Serial.println("📌 Mode: " + mode);
        }

        int scheduleIndex = payload.indexOf("\"schedule\":[\"");
        if (scheduleIndex > 0) {
            scheduleTimes[0] = payload.substring(scheduleIndex + 12, payload.indexOf("\"", scheduleIndex + 12));
            scheduleTimes[1] = payload.substring(payload.indexOf("\"", scheduleIndex + 13) + 2, payload.indexOf("\"", scheduleIndex + 25));
            Serial.println("📌 Schedule: " + scheduleTimes[0] + " & " + scheduleTimes[1]);
        }
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
        sendMeowToServer();

        if (mode == "meow") {
            moveServo();
        }
    } else {
        meowDetected = false;
    }

    server.send(200, "text/plain", String(micValue));
}

void handleTrigger() {
    Serial.println("🚀 Manual Trigger Activated!");
    moveServo();
    server.send(200, "text/plain", "Trigger received!");
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\n✅ Connected!");
    Serial.print("🌐 IP Address: ");
    Serial.println(WiFi.localIP());

    myServo.attach(SERVO_PIN);
    myServo.write(0);

    server.on("/", []() { server.send(200, "text/plain", "ESP8266 Cat Feeder!"); });
    server.on("/micdata", handleMicData);
    server.on("/trigger", handleTrigger);

    server.begin();
    Serial.println("✅ HTTP Server Started!");
}

void loop() {
    server.handleClient();
    unsigned long currentTime = millis();
    if (currentTime - lastCheckTime > CHECK_INTERVAL) {
        checkServerSettings();
        lastCheckTime = currentTime;
    }
}
