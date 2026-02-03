#include <WiFiS3.h>

// ---------- WiFi Credentials ----------
// This will be the name of the network the Arduino creates
char ssid[] = "teampab";
// The password to join the Arduino's network (min 8 chars)
char pass[] = "1234567890";

WiFiServer server(80);

// ---------- Motor A ----------
int enA = 9;
int in1 = 2;
int in2 = 3;

// ---------- Motor B ----------
int enB = 10;
int in3 = 4;
int in4 = 5;

void setup() {
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  Serial.begin(9600);
  delay(2000);

  // *** CHANGED: Creating Access Point ***
  Serial.println("Creating Access Point...");
  
  // WiFi.beginAP creates the network instead of connecting to one
  if (WiFi.beginAP(ssid, pass) != WL_AP_LISTENING) {
    Serial.println("Creating Access Point failed");
    // Don't continue if AP fails
    while (true); 
  }

  Serial.println("Access Point Created!");
  Serial.print("Connect to SSID: ");
  Serial.println(ssid);
  
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); 
  // Note: The IP is usually 192.168.4.1 in AP mode

  server.begin();
  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    Serial.println(request);

    // Motor A Logic
    if (request.indexOf("/A") != -1) {
      int val = request.substring(request.indexOf("/A") + 2).toInt();
      val = constrain(val, 0, 255);
      setMotor(val, enA, in1, in2);
    }

    // Motor B Logic
    if (request.indexOf("/B") != -1) {
      int val = request.substring(request.indexOf("/B") + 2).toInt();
      val = constrain(val, 0, 255);
      setMotor(val, enB, in3, in4);
    }

    // Stop Logic
    if (request.indexOf("/STOP") != -1) {
      setMotor(128, enA, in1, in2);
      setMotor(128, enB, in3, in4);
    }

    // Send Response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("OK");

    client.stop();
  }
}

// ---------- Motor Logic (UNCHANGED) ----------
void setMotor(int val, int enPin, int pin1, int pin2) {

  if (val > 128) {
    int speed = map(val, 128, 255, 0, 255);
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
    analogWrite(enPin, speed);
  }
  else if (val < 128) {
    int speed = map(val, 128, 0, 0, 255);
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
    analogWrite(enPin, speed);
  }
  else {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    analogWrite(enPin, 0);
  }
}