String arduinoIP = "192.168.4.1";  // <-- CHANGE THIS

String statusText = "STOP";

// Motor values
int speedFull = 200;
int speedStop = 128;
int speedRev  = 28;

void setup() {
  size(400, 300);
  textSize(32);
  textAlign(CENTER, CENTER);
}

void draw() {
  background(50);
  fill(255);
  text("WASD to Drive", width/2, height/3);

  fill(0, 255, 100);
  text(statusText, width/2, height/2);
}

void keyPressed() {

  if (key == 'w' || key == 'W') {
    send("A" + speedFull);
    send("B" + speedFull);
    statusText = "FORWARD";
  }
  else if (key == 's' || key == 'S') {
    send("A" + speedRev);
    send("B" + speedRev);
    statusText = "BACKWARD";
  }
  else if (key == 'a' || key == 'A') {
    send("A" + speedStop);
    send("B" + speedFull);
    statusText = "LEFT";
  }
  else if (key == 'd' || key == 'D') {
    send("A" + speedFull);
    send("B" + speedStop);
    statusText = "RIGHT";
  }
}

void keyReleased() {
  send("A" + speedStop);
  send("B" + speedStop);
  statusText = "STOP";
}

void send(String cmd) {
  try {
    String url = "http://" + arduinoIP + "/" + cmd;
    loadStrings(url);
    println("Sent: " + url);
  }
  catch (Exception e) {
    println("Failed to send command");
  }
}
