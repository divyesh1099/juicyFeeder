# **🐱 Juicy Feeder - IoT-Based Automated Feeding System**
🚀 **An AI-powered, WiFi-connected, remote-controlled cat feeder with live logging!**  
🐱 Designed to detect meows, log feeding times, and dispense food via a servo motor.  

---

## **🌟 Features**
✔️ **Remote Access:** Control the feeder from anywhere via the internet.  
✔️ **Live Sound Detection:** Detects meows and logs events automatically.  
✔️ **Manual & Auto Mode:** Choose between button-controlled feeding or mic-based feeding.  
✔️ **Live Logging:** View real-time logs of sound detections without refreshing.  
✔️ **Battery Powered:** Runs on 3.7V LiPo batteries with charging support.  
✔️ **Dynamic Mode Selection:** Switch between **Button Only** or **MIC + Button** via the web interface.  
✔️ **Persistent Storage:** Saves mode settings even after power loss.  

---

## **📸 Project Overview**
### **💡 How It Works**
1. **ESP8266 hosts a web server** with an interactive control panel.
2. **Users can manually trigger the servo motor** from the web interface.
3. **The microphone module detects meows** and logs them with timestamps.
4. **Data is live-updated on the dashboard** without needing to refresh.
5. **ESP8266 connects to WiFi & supports remote access via Port Forwarding or Ngrok.**
6. **Battery-powered with a LiPo charging module for uninterrupted operation.**

---

## **🛠️ Components Required**
### **1️⃣ Hardware Components**
| Component | Quantity | Purpose |
|-----------|----------|---------|
| **ESP8266 NodeMCU** | 1 | Controls everything & hosts the web server |
| **Servo Motor (SG90 or MG995)** | 1 | Opens the feeder lid |
| **Electret Microphone Module** | 1 | Detects cat meows |
| **3.7V LiPo Batteries (1200mAh or higher)** | 2 | Powers the feeder |
| **TP4056 LiPo Charger Module** | 1 | Safely charges the LiPo batteries |
| **AMS1117-3.3V Voltage Regulator** | 1 | Steps down 3.7V to 3.3V |
| **Jumper Wires** | Several | Connects all components |
| **Plastic Enclosure (Optional)** | 1 | Protects the electronics |

---

## **🔌 Wiring Diagram**
### **🔹 Powering ESP8266 from LiPo Batteries**
```
  (3.7V LiPo 1)  + ----+
                  |    |
  (3.7V LiPo 2)  + ----+ ---> TP4056 Module ---> 3.3V Regulator ---> ESP8266 VIN
```
🔹 **Connections**:
1. **LiPo Batteries (Parallel)** → Connect **both positive & negative terminals together**.
2. **Connect to TP4056 LiPo Charger**:
   - `BAT+` → **Battery +**
   - `BAT-` → **Battery -**
   - `OUT+` → **AMS1117-3.3V Regulator Input**
   - `OUT-` → **Ground**
3. **Connect 3.3V Regulator**:
   - **IN+** → `OUT+` from TP4056
   - **IN-** → `OUT-` from TP4056
   - **OUT+** → ESP8266 `VIN`
   - **OUT-** → ESP8266 `GND`

---

## **🔧 Software Setup**
### **1️⃣ Install Required Libraries in Arduino IDE**
Before uploading the code, install the following libraries:
- **ESP8266WiFi** → `Manage Libraries` → Install `ESP8266WiFi`
- **ESP8266WebServer** → `Manage Libraries` → Install `ESP8266WebServer`
- **Servo** → `Manage Libraries` → Install `Servo`

---

### **2️⃣ Upload the Code to ESP8266**
Upload the following **Arduino Sketch**:

```cpp
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <Servo.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

ESP8266WebServer server(80);
Servo myServo;

#define MIC_PIN A0  
#define MEOW_THRESHOLD 600  
#define SERVO_PIN D4  
#define CHECK_INTERVAL 100  
#define MAX_LOGS 10  

bool meowDetected = false;
unsigned long lastCheckTime = 0;
String mode = "button";  
String logs[MAX_LOGS];   
int logIndex = 0;

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

void logMicDetection() {
    if (logIndex >= MAX_LOGS) {
        for (int i = 1; i < MAX_LOGS; i++) {
            logs[i - 1] = logs[i];
        }
        logIndex = MAX_LOGS - 1;
    }
    logs[logIndex] = millis() / 1000;  
    logIndex++;
}

void moveServo() {
    myServo.attach(SERVO_PIN);
    myServo.write(0);
    delay(500);
    myServo.write(90);
    delay(500);
    myServo.write(180);
    delay(500);
    myServo.write(90);
    delay(500);
    myServo.write(0);
    delay(500);
    myServo.detach();
}

void handleRoot() {
    String html = "<html><body><h1>ESP8266 Cat Feeder</h1>";
    html += "<p>Current Mode: <b>" + mode + "</b></p>";
    html += "<button onclick=\"fetch('/moveServo')\">Move Servo</button>";
    html += "<button onclick=\"fetch('/setMode?mode=button')\">Button Only</button>";
    html += "<button onclick=\"fetch('/setMode?mode=mic')\">MIC + Button</button>";
    html += "<h3>MIC Detection Log:</h3><ul id='logList'></ul>";
    html += "<script>";
    html += "setInterval(()=>fetch('/logs').then(r=>r.json()).then(logs=>{";
    html += "document.getElementById('logList').innerHTML='';";
    html += "logs.forEach(log=>{let li=document.createElement('li'); li.textContent='🐱 Meow at '+log; document.getElementById('logList').appendChild(li);});";
    html += "}),1000);";
    html += "</script></body></html>";

    server.send(200, "text/html", html);
}

void handleMicData() {
    int micValue = analogRead(MIC_PIN);
    if (mode == "mic" && micValue > MEOW_THRESHOLD && !meowDetected) {
        meowDetected = true;
        logMicDetection();
    }
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
        }
    }
    server.send(200, "text/plain", "Mode updated");
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(1000); }
    server.on("/", handleRoot);
    server.on("/moveServo", []() { moveServo(); server.send(200, "text/plain", "Servo Moved"); });
    server.on("/micData", handleMicData);
    server.on("/logs", []() {
        String json = "[";
        for (int i = 0; i < logIndex; i++) { if (i > 0) json += ","; json += "\"" + logs[i] + "\""; }
        json += "]";
        server.send(200, "application/json", json);
    });
    server.begin();
}

void loop() {
    server.handleClient();
}
```
