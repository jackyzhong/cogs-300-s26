#include "WiFiS3.h"

char ssid[] = "RobotControl";
char pass[] = "12345678";
WiFiServer server(80);
WiFiClient client; 

// Pins
const int trigPin = 9, echoPin = 10;
const int motorL_PWM = 5, motorL_IN1 = 2, motorL_IN2 = 3;
const int motorR_PWM = 6, motorR_IN3 = 4, motorR_IN4 = 7;

// Logic Variables
int mode = 0; 
// Mode 0: Stop
// Mode 1: Wall Follow
// Mode 2: Follow Me
// Mode 3: Manual / Debug

int baseSpeed = 80; 
int manualLeft = 0;  // Store manual speed for Left
int manualRight = 0; // Store manual speed for Right

unsigned long lastSensorRead = 0; 
long currentDistance = 0;

void setup() {
  Serial.begin(115200);
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

      // --- EXISTING MODES ---
      if (command.indexOf("WALL")   != -1) mode = 1;
      if (command.indexOf("FOLLOW") != -1) mode = 2;
      
      if (command.indexOf("STOP") != -1) {
        mode = 0;
        manualLeft = 0;
        manualRight = 0;
      }

      // --- NEW: MANUAL DEBUG COMMANDS ---
      // Format: "L 100" (Left forward 100), "R -100" (Right backward 100)
      
      if (command.startsWith("L ")) {
        mode = 3; // Switch to Manual Mode
        manualLeft = command.substring(2).toInt(); // Parse number after "L "
      }
      
      if (command.startsWith("R ")) {
        mode = 3; 
        manualRight = command.substring(2).toInt(); // Parse number after "R "
      }

      // Format: "M 100 100" (Set both at once)
      if (command.startsWith("M ")) {
        mode = 3;
        // Simple parsing for two integers
        int firstSpace = command.indexOf(' ');
        int secondSpace = command.lastIndexOf(' ');
        
        if (firstSpace != -1 && secondSpace != -1 && firstSpace != secondSpace) {
           String leftPart = command.substring(firstSpace + 1, secondSpace);
           String rightPart = command.substring(secondSpace + 1);
           manualLeft = leftPart.toInt();
           manualRight = rightPart.toInt();
        }
      }
      
      client.print("Mode: "); client.print(mode);
      client.print(" | Dist: "); client.println(currentDistance); 
    }
  }

  // 2. Sensor Logic (Every 100ms) - Only needed for auto modes
  if ((mode == 1 || mode == 2) && (millis() - lastSensorRead > 100)) {
     currentDistance = getDistance();
     lastSensorRead = millis();
  }

  // 3. Action Logic
  if (mode == 1) { // Wall Follow Logic
    int wallSpeed = 66; 
    if (currentDistance < 50 && currentDistance != 0) {
       moveRobot(0, wallSpeed); 
    } else {
      moveRobot(wallSpeed, wallSpeed); 
    }
  } 
  else if (mode == 2) { // Follow Me Logic
    if (currentDistance >= 10 && currentDistance <= 17) {
      moveRobot(baseSpeed, baseSpeed);
    } else {
      stopMotors();
    }
  }
  else if (mode == 3) { // MANUAL / DEBUG LOGIC
    moveRobot(manualLeft, manualRight);
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
  
  // Left Motor
  digitalWrite(motorL_IN1, left < 0);  
  digitalWrite(motorL_IN2, left > 0);  
  analogWrite(motorL_PWM, abs(left));
  
  // Right Motor
  digitalWrite(motorR_IN3, right < 0); 
  digitalWrite(motorR_IN4, right > 0); 
  analogWrite(motorR_PWM, abs(right));
}

void stopMotors() {
  analogWrite(motorL_PWM, 0); analogWrite(motorR_PWM, 0);
  digitalWrite(motorL_IN1, LOW); digitalWrite(motorL_IN2, LOW);
  digitalWrite(motorR_IN3, LOW); digitalWrite(motorR_IN4, LOW);
}