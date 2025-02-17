#include <Servo.h>

Servo myServo;
#define SERVO_PIN D4  // Connect Servo signal pin to D4 (GPIO2)

void setup() {
    myServo.attach(SERVO_PIN);
    Serial.begin(115200);
}

void loop() {
    Serial.println("Moving Servo to 0°");
    myServo.write(0);
    delay(1000);

    Serial.println("Moving Servo to 90°");
    myServo.write(90);
    delay(1000);

    Serial.println("Moving Servo to 180°");
    myServo.write(180);
    delay(1000);
}
