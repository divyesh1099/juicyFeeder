#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>  // For storing user-selected mode
#include <Servo.h>

// WiFi Credentials
const char* ssid = "networkbda0917cffgpiwr8645998746";
const char* password = "Omshree1225*";

ESP8266WebServer server(80);
Servo myServo;

#define MIC_PIN A0   // Microphone sensor
#define MEOW_THRESHOLD 600  // Adjust as needed
#define SERVO_PIN D4  // Servo Pin
#define CHECK_INTERVAL 100  // Check MIC every 100ms

bool meowDetected = false;
unsigned long lastCheckTime = 0;
String mode = "button";  // Default mode

void saveModeToEEPROM(String selectedMode) {
    EEPROM.begin(10);
    EEPROM.write(0, selectedMode == "mic" ? 1 : 0);
    EEPROM.commit();
}

void loadModeFromEEPROM() {
    EEPROM.begin(10);
    int storedMode = EEPROM.read(0);
    mode = (storedMode == 1) ? "mic" : "button";
}

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
    html += "<style>body{text-align:center;font-family:Arial;}button{font-size:20px;padding:10px;margin:10px;}</style>";
    html += "</head><body>";
    html += "<h1>ESP8266 Servo Control</h1>";
    html += "<p>Current Mode: <b>" + mode + "</b></p>";
    
    html += "<button onclick=\"fetch('/moveServo')\">Move Servo</button>";
    
    html += "<h3>Select Control Mode:</h3>";
    html += "<button onclick=\"fetch('/setMode?mode=button')\">Button Only</button> ";
    html += "<button onclick=\"fetch('/setMode?mode=mic')\">MIC + Button</button>";

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

    // Move servo if MIC + Button mode is enabled and sound threshold is exceeded
    if (mode == "mic" && micValue > MEOW_THRESHOLD && !meowDetected) {
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

void handleSetMode() {
    if (server.hasArg("mode")) {
        String selectedMode = server.arg("mode");
        if (selectedMode == "button" || selectedMode == "mic") {
            mode = selectedMode;
            saveModeToEEPROM(mode);
            Serial.println("✅ Mode Changed to: " + mode);
        }
    }
    server.send(200, "text/plain", "Mode updated to " + mode);
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

    loadModeFromEEPROM();  // Load last used mode from EEPROM

    server.on("/", handleRoot);
    server.on("/moveServo", handleMoveServo);
    server.on("/micData", handleMicData);
    server.on("/setMode", handleSetMode);

    server.begin();
    Serial.println("✅ HTTP Server Started!");
}

void loop() {
    server.handleClient();

    // Check MIC sensor every CHECK_INTERVAL ms (only if in MIC mode)
    unsigned long currentTime = millis();
    if (mode == "mic" && currentTime - lastCheckTime > CHECK_INTERVAL) {
        lastCheckTime = currentTime;
        handleMicData();
    }
}
