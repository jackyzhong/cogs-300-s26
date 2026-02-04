#include "WiFiS3.h"

char ssid[] = "RobotControl";
char pass[] = "12345678";
WiFiServer server(80);
WiFiClient client; // Keep the client global to maintain connection

// Pins
const int trigPin = 9, echoPin = 10;
const int motorL_PWM = 5, motorL_IN1 = 2, motorL_IN2 = 3;
const int motorR_PWM = 6, motorR_IN3 = 4, motorR_IN4 = 7;

int mode = 0; 
int baseSpeed = 80; 
unsigned long lastSensorRead = 0; // For non-blocking timer
long currentDistance = 0;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); pinMode(echoPin, INPUT);
  pinMode(motorL_PWM, OUTPUT); pinMode(motorL_IN1, OUTPUT); pinMode(motorL_IN2, OUTPUT);
  pinMode(motorR_PWM, OUTPUT); pinMode(motorR_IN3, OUTPUT); pinMode(motorR_IN4, OUTPUT);

  WiFi.beginAP(ssid, pass);
  server.begin();
}

void loop() {
  // 1. Check for NEW clients if we don't have one
  if (!client) {
    client = server.available();
  }

  // 2. If we have a client and they sent data
  if (client && client.connected()) {
    if (client.available()) {
      String command = "";
      // Read until a newline character or timeout
      command = client.readStringUntil('\n'); 
      command.trim();      // Remove whitespace
      command.toUpperCase(); 

      if (command.indexOf("WALL")   != -1) mode = 1;
      if (command.indexOf("FOLLOW") != -1) mode = 2;
      if (command.indexOf("STOP")   != -1) mode = 0;
      
      // Send feedback but DO NOT STOP the client
      client.println(currentDistance); 
    }
  }

  // 3. Sensor Logic - ONLY run if needed and use a Timer (Non-blocking-ish)
  // Only check sensor every 100ms, not every loop
  if ((mode == 1 || mode == 2) && (millis() - lastSensorRead > 100)) {
     currentDistance = getDistance();
     lastSensorRead = millis();
  }

  // 4. Action Logic
  if (mode == 1) { // Wall Avoidance
    if (currentDistance < 15 && currentDistance != 0) moveRobot(-baseSpeed, baseSpeed);
    else moveRobot(baseSpeed, baseSpeed);
  } 
  else if (mode == 2) { // Follow
    if (currentDistance == 0) stopMotors(); // Safety if sensor fails
    else if (currentDistance > 20) moveRobot(baseSpeed, baseSpeed);
    else if (currentDistance < 12) moveRobot(-baseSpeed, -baseSpeed);
    else stopMotors();
  } 
  else {
    stopMotors();
  }
  
  // Removed delay(30) to make steering more responsive
}

long getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); 
  if (duration == 0) return 999; // Return "far" if timeout
  return duration * 0.034 / 2;
}

void moveRobot(int left, int right) {
  digitalWrite(motorL_IN1, left > 0); digitalWrite(motorL_IN2, left < 0);
  analogWrite(motorL_PWM, abs(left));
  digitalWrite(motorR_IN3, right > 0); digitalWrite(motorR_IN4, right < 0);
  analogWrite(motorR_PWM, abs(right));
}

void stopMotors() {
  analogWrite(motorL_PWM, 0); analogWrite(motorR_PWM, 0);
  digitalWrite(motorL_IN1, LOW); digitalWrite(motorL_IN2, LOW);
  digitalWrite(motorR_IN3, LOW); digitalWrite(motorR_IN4, LOW);
}