import processing.net.*;

Client myClient;
String distanceVal = "0";

// Speed settings for manual control
int driveSpeed = 150;
int turnSpeed = 100;

void setup() {
  size(400, 350); // Made slightly taller for instructions
  
  // Connect to the Arduino WiFi Server
  myClient = new Client(this, "192.168.4.1", 80); 
}

void draw() {
  background(40);
  fill(255);
  
  // --- INSTRUCTIONS ---
  textSize(16);
  text("MANUAL CONTROL:", 20, 30);
  text("[ W ]  Forward", 20, 55);
  text("[ S ]  Backward", 20, 75);
  text("[ A ]  Spin Left", 20, 95);
  text("[ D ]  Spin Right", 20, 115);
  
  text("AUTONOMOUS MODES:", 200, 30);
  text("[ 1 ]  Wall Follow", 200, 55);
  text("[ 2 ]  Follow Me", 200, 75);
  text("[ SPACE ] Stop All", 200, 115);
   
  // --- STATUS INDICATOR ---
  if (myClient.active()) {
    fill(0, 255, 0); text("Connected", 280, 150);
  } else {
    fill(255, 0, 0); text("Disconnected", 280, 150);
  }
   
  // --- READING SENSOR DATA ---
  if (myClient.available() > 0) {
    String input = myClient.readStringUntil('\n');
    if (input != null) {
      distanceVal = input.trim(); 
    }
  }
   
  // --- DISPLAY DATA ---
  textSize(24);
  fill(0, 255, 255);
  text("Sensor Distance: " + distanceVal + " cm", 20, 280);
}

void keyPressed() {
  // Manual Movement (Uses the 'M' command we added to Arduino)
  // Format: "M leftSpeed rightSpeed\n"
  
  if (key == 'w' || key == 'W') myClient.write("M " + driveSpeed + " " + driveSpeed + "\n");
  if (key == 's' || key == 'S') myClient.write("M -" + driveSpeed + " -" + driveSpeed + "\n");
  
  // Turning (Spinning in place)
  if (key == 'a' || key == 'A') myClient.write("M -" + turnSpeed + " " + turnSpeed + "\n");
  if (key == 'd' || key == 'D') myClient.write("M " + turnSpeed + " -" + turnSpeed + "\n");

  // Autonomous Mode Selection
  if (key == '1') myClient.write("WALL\n");
  if (key == '2') myClient.write("FOLLOW\n");
  
  // Emergency Stop
  if (key == ' ') myClient.write("STOP\n");
}

void keyReleased() {
  // Safety: When you let go of WASD, the robot stops.
  if (key == 'w' || key == 'W' || key == 's' || key == 'S' || 
      key == 'a' || key == 'A' || key == 'd' || key == 'D') {
    myClient.write("STOP\n");
  }
}
