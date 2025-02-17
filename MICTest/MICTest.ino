#define MIC_PIN A0  // MIC module connected to A0

void setup() {
    Serial.begin(115200);
    pinMode(MIC_PIN, INPUT);
}

void loop() {
    int micValue = analogRead(MIC_PIN);
    Serial.println(micValue);  // Print MIC sensor readings
    delay(200);
}
