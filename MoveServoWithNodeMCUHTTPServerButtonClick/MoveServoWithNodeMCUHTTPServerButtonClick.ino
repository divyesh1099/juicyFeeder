#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// WiFi Credentials
const char* ssid = "networkbda0917cffgpiwr8645998746";  
const char* password = "Omshree1225*";  

ESP8266WebServer server(80);
Servo myServo;

#define SERVO_PIN D4  // GPIO Pin for Servo

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
    String html = "<html><head><title>ESP8266 Servo Control</title>";
    html += "<style>body{text-align:center;font-family:Arial;}button{font-size:20px;padding:10px;margin-top:20px;}</style>";
    html += "</head><body>";
    html += "<h1>ESP8266 Servo Control</h1>";
    html += "<button onclick=\"fetch('/moveServo')\">Move Servo</button>";
    html += "</body></html>";

    server.send(200, "text/html", html);
}

void handleMoveServo() {
    Serial.println("🚀 Button Pressed! Moving Servo...");
    moveServo();
    server.send(200, "text/plain", "Servo Moved!");
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

    server.begin();
    Serial.println("✅ HTTP Server Started!");
}

void loop() {
    server.handleClient();
}
