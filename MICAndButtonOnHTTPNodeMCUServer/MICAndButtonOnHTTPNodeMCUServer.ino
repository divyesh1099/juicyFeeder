#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// WiFi Credentials (Your details)
const char* ssid = "networkbda0917cffgpiwr8645998746";
const char* password = "Omshree1225*";

ESP8266WebServer server(80);
Servo myServo;

#define MIC_PIN A0   // Microphone sensor
#define MEOW_THRESHOLD 1000  // Adjust as needed
#define SERVO_PIN D4  // Servo Pin
#define CHECK_INTERVAL 100  // Check MIC every 100ms

bool meowDetected = false;
unsigned long lastCheckTime = 0;

void moveServo() {
    Serial.println("🔄 Moving Servo to 0°");
    myServo.attach(SERVO_PIN);
    myServo.write(0);
    delay(500);

    Serial.println("🔄 Moving Servo to 90°");
    myServo.write(90);
    delay(500);

    Serial.println("🔄 Moving Servo to 180°");
    myServo.write(180);
    delay(500);

    Serial.println("🔄 Moving Servo to 90°");
    myServo.write(90);
    delay(500);

    Serial.println("🔄 Moving Servo back to 0°");
    myServo.write(0);
    delay(500);

    myServo.detach();
    Serial.println("✅ Servo Movement Completed");
}

void handleRoot() {
    String html = "<html><head><title>ESP8266 Servo & MIC Control</title>";
    html += "<style>body{text-align:center;font-family:Arial;}button{font-size:20px;padding:10px;margin-top:20px;}</style>";
    html += "</head><body>";
    html += "<h1>ESP8266 Servo Control</h1>";
    html += "<button onclick=\"fetch('/moveServo')\">Move Servo</button>";
    html += "<h3>MIC Value: <span id='micVal'>-</span></h3>";
    html += "<script>setInterval(()=>fetch('/micData').then(r=>r.text()).then(d=>document.getElementById('micVal').innerText=d),1000);</script>";
    html += "</body></html>";

    server.send(200, "text/html", html);
}

void handleMoveServo() {
    Serial.println("🚀 Button Pressed! Moving Servo...");
    moveServo();
    server.send(200, "text/plain", "Servo Moved!");
}

void handleMicData() {
    int micValue = analogRead(MIC_PIN);
    Serial.print("🎤 MIC Value: ");
    Serial.println(micValue);

    // Move servo if meow threshold is exceeded
    if (micValue > MEOW_THRESHOLD && !meowDetected) {
        Serial.println("🐱 Meow Detected! Triggering Servo...");
        meowDetected = true;
        moveServo();
    }

    // Reset detection if value goes below threshold
    if (micValue < MEOW_THRESHOLD - 50) {
        meowDetected = false;
    }

    server.send(200, "text/plain", String(micValue));
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
    delay(500);
    myServo.detach();

    server.on("/", handleRoot);
    server.on("/moveServo", handleMoveServo);
    server.on("/micData", handleMicData);

    server.begin();
    Serial.println("✅ HTTP Server Started!");
}

void loop() {
    server.handleClient();

    // Check MIC sensor every CHECK_INTERVAL ms
    unsigned long currentTime = millis();
    if (currentTime - lastCheckTime > CHECK_INTERVAL) {
        lastCheckTime = currentTime;
        handleMicData();
    }
}
