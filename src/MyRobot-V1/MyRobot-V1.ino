// // MyRobot-V1.ino
// // This file must be named the same as your sketch folder

// init Motor A
int enA = 9;   
int in1 = 2;   
int in2 = 3;   

// init Motor B
int enB = 10;   
int in3 = 4;   
int in4 = 5;   

void setup() {
    // Set motor A control pins as outputs
    pinMode(enA, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);

    // Set motor B control pins as outputs
    pinMode(enB, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);

    Serial.begin(9600);
    Serial.println("System Ready.");
    Serial.println("Send commands (e.g., A255 for Full Fwd, B0 for Full Back, A128 for Stop)");
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read(); // Read the letter (A or B)
    
    // We only want to process if the character is a letter we expect
    if (command == 'A' || command == 'a' || command == 'B' || command == 'b') {
      
      int inputVal = Serial.parseInt(); // Read the number immediately following the letter

      // Constraint input to ensure it stays within 0-255 safety limits
      inputVal = constrain(inputVal, 0, 255);

      if (command == 'A' || command == 'a') {
        setMotor(inputVal, enA, in1, in2);
        Serial.print("Motor A set to: ");
        Serial.println(inputVal);
      } 
      else if (command == 'B' || command == 'b') {
        setMotor(inputVal, enB, in3, in4);
        Serial.print("Motor B set to: ");
        Serial.println(inputVal);
      }
    }
  }
}

// Helper function to handle the 0-128-255 logic
void setMotor(int val, int enPin, int pin1, int pin2) {
  
  if (val > 128) {
    // --- FORWARD ---
    // Map range 129-255 to PWM 0-255
    int speed = map(val, 128, 255, 0, 255);
    
    digitalWrite(pin1, LOW);   // Configured for Forward
    digitalWrite(pin2, HIGH);
    analogWrite(enPin, speed); // Send PWM speed
  } 
  else if (val < 128) {
    // --- BACKWARD ---
    // Map range 127-0 to PWM 0-255
    int speed = map(val, 128, 0, 0, 255);
    
    digitalWrite(pin1, HIGH);  // Configured for Backward
    digitalWrite(pin2, LOW);
    analogWrite(enPin, speed); // Send PWM speed
  } 
  else {
    // --- STOP (val is 128) ---
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    analogWrite(enPin, 0);     // Turn off motor
  }
}