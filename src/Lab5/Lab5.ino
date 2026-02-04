#include "WiFiS3.h"

char ssid[] = "RobotControl";
char pass[] = "12345678";
WiFiServer server(80);
WiFiClient client; 

// Pins
const int trigPin = 9, echoPin = 10;
const int motorL_PWM = 5, motorL_IN1 = 2, motorL_IN2 = 3;
const int motorR_PWM = 6, motorR_IN3 = 4, motorR_IN4 = 7;

int mode = 0; 
int baseSpeed = 80; 
unsigned long lastSensorRead = 0; 
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
  // 1. Connection Logic
  if (!client) client = server.available();

  if (client && client.connected()) {
    if (client.available()) {
      String command = client.readStringUntil('\n'); 
      command.trim();      
      command.toUpperCase(); 

      if (command.indexOf("WALL")   != -1) mode = 1;
      if (command.indexOf("FOLLOW") != -1) mode = 2;
      if (command.indexOf("STOP")   != -1) mode = 0;
      
      client.println(currentDistance); 
    }
  }

  // 2. Sensor Logic (Every 100ms)
  if ((mode == 1 || mode == 2) && (millis() - lastSensorRead > 100)) {
     currentDistance = getDistance();
     lastSensorRead = millis();
  }

  // 3. Action Logic
  if (mode == 1) { // Wall Follow Logic
    int wallSpeed = 100; 
    
    // Wall Threshold: 7cm
    if (currentDistance < 7 && currentDistance != 0) {
       moveRobot(0, wallSpeed); // Turn Left
    } 
    else {
      moveRobot(wallSpeed, wallSpeed); // Forward
    }
  } 
  else if (mode == 2) { // Follow Me Logic
    
    // REVISION: Only move if object is strictly between 5cm and 10cm
    if (currentDistance >= 5 && currentDistance <= 10) {
      moveRobot(baseSpeed, baseSpeed);
    }
    else {
      stopMotors();
    }
  } 
  else {
    stopMotors();
  }
}

long getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); 
  if (duration == 0) return 999; 
  return duration * 0.034 / 2;
}

void moveRobot(int left, int right) {
  // Positive = Forward, Negative = Backward
  digitalWrite(motorL_IN1, left < 0);  
  digitalWrite(motorL_IN2, left > 0);  
  analogWrite(motorL_PWM, abs(left));
  
  digitalWrite(motorR_IN3, right < 0); 
  digitalWrite(motorR_IN4, right > 0); 
  analogWrite(motorR_PWM, abs(right));
}

void stopMotors() {
  analogWrite(motorL_PWM, 0); analogWrite(motorR_PWM, 0);
  digitalWrite(motorL_IN1, LOW); digitalWrite(motorL_IN2, LOW);
  digitalWrite(motorR_IN3, LOW); digitalWrite(motorR_IN4, LOW);
}