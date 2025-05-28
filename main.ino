#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>  // ESP32-compatible Servo library

// WiFi Credentials
const char* ssid = "TBH TOP FLOOR";
const char* password = "pakistan123";

// Web server on port 80
WebServer server(80);

// Pin Definitions
const int ledPins[] = {2, 4, 5, 12, 16};  
const int fanPin1 = 18;  // IN1 for L298
const int fanPin2 = 19;  // IN2 for L298
const int fanSpeedPin = 21; // ENA for PWM speed control
const int garageDoorPin = 22;
const int mainDoorPin = 23;
const int window1Pin = 14 ;
const int window2Pin = 25;
const int clothesHangerPin = 26;
const int irSensorPin = 27;   
const int rainSensorPin = 13;
const int rainbuz = 15;
const int rainSensorAnalogPin = 34; 
const int pianoLedPins[] = {32, 33, 35}; // 4 steps (expandable)

// Servo Positions
enum ServoPosition { SERVO_OPEN = 180, SERVO_CLOSED = 0, SERVO_WINDOW_OPEN = 90 };

// Device States
bool ledStates[9] = {false};     
bool fanState = false;           
bool garageDoorState = false;    
bool mainDoorState = false;      
bool window1State = false;       
bool window2State = false;       
bool clothesHangerState = false; 

// Sensor Tracking
bool lastIrState = false;        
bool lastRainState = false;      
unsigned long lastSensorCheck = 0;
const unsigned long SENSOR_CHECK_INTERVAL = 1000; // Check sensors every 1 sec


// Servo Objects
Servo garageDoorServo, mainDoorServo, window1Servo, window2Servo, clothesHangerServo;

// Function to generate the control page
String generateControlPage() {
  String html = R"(
  <!DOCTYPE html>
  <html>
  <head>
    <title>Smart Home Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial, sans-serif; margin: 20px; background: #f5f5f5; }
      .container { max-width: 800px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
      h1 { color: #333; text-align: center; }
      .control-group { margin-bottom: 15px; padding: 15px; background: #f9f9f9; border-radius: 5px; }
      button { padding: 8px 15px; background: #4CAF50; color: white; border: none; border-radius: 4px; cursor: pointer; }
      button:disabled { background: #cccccc; }
      .led-control { display: flex; align-items: center; margin-bottom: 5px; }
      .led-control span { flex: 1; }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>Smart Home Control Panel</h1>
  )";

  // LED Controls
  html += "<div class='control-group'><h2>LED Controls</h2>";
  for (int i = 0; i < 9; i++) {
    html += "<div class='led-control'><span>LED " + String(i + 1) + "</span>";
    html += "<button onclick='toggleLED(" + String(i) + ")'>" + String(ledStates[i] ? "ON" : "OFF") + "</button></div>";
  }
   html += "<button onclick='activatePianoEffect()'>Play Piano Effect</button>";
  html += "</div>";

  // Fan Control
  html += "<div class='control-group'><h2>Fan Control</h2>";
  html += "<button onclick='toggleFan()'>" + String(fanState ? "ON" : "OFF") + "</button></div>";

  // Door Controls
  html += "<div class='control-group'><h2>Door Controls</h2>";
  html += "<button onclick='toggleDoor(\"garage\")'>Garage Door " + String(garageDoorState ? "OPEN" : "CLOSED") + "</button><br><br>";
  html += "<button onclick='toggleDoor(\"main\")'>Main Door " + String(mainDoorState ? "OPEN" : "CLOSED") + "</button></div>";

  // Window Controls
  html += "<div class='control-group'><h2>Window Controls</h2>";
  html += "<button onclick='toggleWindow(1)'>Window 1 " + String(window1State ? "OPEN" : "CLOSED") + "</button><br><br>";
  html += "<button onclick='toggleWindow(2)'>Window 2 " + String(window2State ? "OPEN" : "CLOSED") + "</button></div>";

  // Clothes Hanger Control
  html += "<div class='control-group'><h2>Clothes Hanger</h2>";
  html += "<button onclick='toggleClothesHanger()'>" + String(clothesHangerState ? "OPEN" : "CLOSED") + "</button></div>";

  // Sensor Status
  html += "<div class='control-group'><h2>Sensor Status</h2>";
  html += "<p>IR Sensor: <span id='irStatus'>" + String(digitalRead(irSensorPin) ? "ACTIVE" : "INACTIVE") + "</span></p>";
  html += "<p>Rain Sensor: <span id='rainStatus'>" + String(digitalRead(rainSensorPin) ? "RAINING" : "DRY") + "</span></p></div>";

  // JavaScript for AJAX Control
  html += R"(
    <script>
      function toggleLED(ledNum) {
        fetch('/control', { method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({device: 'led', number: ledNum}) })
          .then(() => location.reload());
      }
      function toggleFan() {
        fetch('/control', { method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({device: 'fan'}) })
          .then(() => location.reload());
      }
      function toggleDoor(type) {
        fetch('/control', { method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({device: 'door', type: type}) })
          .then(() => location.reload());
      }
      function toggleWindow(num) {
        fetch('/control', { method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({device: 'window', number: num}) })
          .then(() => location.reload());
      }
      function toggleClothesHanger() {
        fetch('/control', { method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({device: 'clothes_hanger'}) })
          .then(() => location.reload());
      }
      function activatePianoEffect() {
        fetch('/control', { 
        method: 'POST', 
        body: JSON.stringify({device: 'piano_effect'}) 
          });
      }
      // Auto-refresh sensor status
      setInterval(() => {
        fetch('/status').then(r => r.json()).then(data => {
          document.getElementById('irStatus').textContent = data.irSensor ? 'ACTIVE' : 'INACTIVE';
          document.getElementById('rainStatus').textContent = data.rainSensor ? 'DRY' : 'Raining';
        });
      }, 5000);
    </script>
  </body></html>
  )";

  return html;
}

void playPianoEffect(int delayTime = 200) {
  for (int i = 0; i < sizeof(pianoLedPins); i++) {
    digitalWrite(pianoLedPins[i], HIGH);
    delay(delayTime);
    digitalWrite(pianoLedPins[i], LOW);
  }

}

// Function to handle control requests
void handleControlRequest() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  String body = server.arg("plain");
  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    server.send(400, "text/plain", "Bad Request");
    return;
  }

  const char* device = doc["device"];
  if (strcmp(device, "led") == 0) {
    int ledIndex = doc["number"].as<int>();
    if (ledIndex >= 0 && ledIndex < 9) {
      ledStates[ledIndex] = !ledStates[ledIndex];
      digitalWrite(ledPins[ledIndex], ledStates[ledIndex] ? HIGH : LOW);
    }
  } else if (strcmp(device, "fan") == 0) {
    fanState = !fanState;
    if (fanState) {
      // Rotate fan forward
      digitalWrite(fanPin1, HIGH);
      digitalWrite(fanPin2, LOW);
      // Optionally set speed
      ledcWrite(fanSpeedPin, 255); // Full speed
    } else {
      // Stop fan
      digitalWrite(fanPin1, LOW);
      digitalWrite(fanPin2, LOW);
    }
  } else if (strcmp(device, "door") == 0) {
    const char* type = doc["type"];
    if (strcmp(type, "garage") == 0) {
      garageDoorState = !garageDoorState;
      garageDoorServo.write(garageDoorState ? SERVO_OPEN : SERVO_CLOSED);
    } else if (strcmp(type, "main") == 0) {
      mainDoorState = !mainDoorState;
      mainDoorServo.write(mainDoorState ? SERVO_OPEN : SERVO_CLOSED);
    }
  } else if (strcmp(device, "window") == 0) {
    int windowNum = doc["number"].as<int>();
    if (windowNum == 1) {
      window1State = !window1State;
      window1Servo.write(window1State ? SERVO_WINDOW_OPEN : SERVO_CLOSED);
    } else if (windowNum == 2) {
      window2State = !window2State;
      window2Servo.write(window2State ? SERVO_WINDOW_OPEN : SERVO_CLOSED);
    }
  } else if (strcmp(device, "clothes_hanger") == 0) {
    clothesHangerState = !clothesHangerState;
    clothesHangerServo.write(clothesHangerState ? SERVO_OPEN : SERVO_CLOSED);
  } else if (strcmp(device, "piano_effect") == 0) {
    playPianoEffect();
}

  server.send(200, "application/json", "{\"status\":\"success\"}");
}

// Function to handle status requests
void handleStatusRequest() {
  DynamicJsonDocument doc(256);
  doc["irSensor"] = digitalRead(irSensorPin);
  doc["rainSensor"] = digitalRead(rainSensorPin);
  doc["rainAnalogValue"] = analogRead(rainSensorAnalogPin);  // Add analog reading

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void setup() {
  Serial.begin(115200);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  initializePins();
  initializeServos();
  connectToWiFi();
  setupWebServer();

  Serial.println("[SYSTEM] Smart Home Controller Ready");
}

void loop() {
  server.handleClient();  // Handle web requests

  // Check sensors every second
  if (millis() - lastSensorCheck >= SENSOR_CHECK_INTERVAL) {
    checkSensors();
    lastSensorCheck = millis();
  }
}

void initializePins() {
  for (int pin : ledPins) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW); // Start with LEDs off
  }
  pinMode(fanPin1, OUTPUT);
  pinMode(fanPin2, OUTPUT);
  pinMode(fanSpeedPin, OUTPUT);
  digitalWrite(fanPin1, LOW);
  digitalWrite(fanPin2, LOW);
  pinMode(irSensorPin, INPUT);
  pinMode(rainSensorPin, INPUT);
  pinMode(rainbuz, OUTPUT);
  for (int pin : pianoLedPins) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
}

void initializeServos() {
  garageDoorServo.attach(garageDoorPin);
  mainDoorServo.attach(mainDoorPin);
  window1Servo.attach(window1Pin);
  window2Servo.attach(window2Pin);
  clothesHangerServo.attach(clothesHangerPin);

  // Set initial positions (closed)
  setAllServos(SERVO_CLOSED);
}

void setAllServos(int position) {
  garageDoorServo.write(position);
  mainDoorServo.write(position);
  window1Servo.write(position);
  window2Servo.write(position);
  clothesHangerServo.write(position);
}

void connectToWiFi() {
  Serial.println("[WiFi] Connecting...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WiFi] Connected!");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
}

void setupWebServer() {
  server.on("/", HTTP_GET, []() {
    String html = generateControlPage();
    server.send(200, "text/html", html);
  });
  server.on("/control", HTTP_POST, handleControlRequest);
  server.on("/status", HTTP_GET, handleStatusRequest);
  server.begin();
  Serial.println("[HTTP] Web Server Started");
}

void checkSensors() {
  // Check IR Sensor
  bool currentIrState = digitalRead(irSensorPin);
  if (currentIrState == false) {
    if (!garageDoorState) {
      garageDoorState = true;
      garageDoorServo.write(SERVO_OPEN);
    }
    if (!mainDoorState) {
      mainDoorState = true;
      mainDoorServo.write(SERVO_OPEN);
    }
    delay(3000);
      garageDoorState = false;
      garageDoorServo.write(SERVO_CLOSED);
      mainDoorState = false;
      mainDoorServo.write(SERVO_CLOSED);
  }
  lastIrState = currentIrState;

  // Check Rain Sensor

  // Read analog value (0-4095 on ESP32)
  int rainAnalogValue = analogRead(rainSensorAnalogPin);
  bool isRaining = (rainAnalogValue < 2500);  // Adjust threshold (2000 = example)
  Serial.println(rainAnalogValue);

  if (isRaining && !lastRainState && clothesHangerState) {
    clothesHangerState = false;
    clothesHangerServo.write(SERVO_CLOSED);
    Serial.println("[RAIN] Detected - Closing Clothes Hanger");
    for (int i= 0; i < 4; i++){
      digitalWrite(rainbuz, HIGH);
      delay(150);
      digitalWrite(rainbuz, LOW);
      delay(150);
    }
    lastRainState = isRaining;
  }
  else if(!isRaining && lastRainState && !clothesHangerState){
    clothesHangerState = true;
    clothesHangerServo.write(SERVO_OPEN);
    Serial.println("[RAIN] Stopped - Opening Clothes Hanger");
  }
  lastRainState = isRaining;
  

  // Rest of your sensor checks (IR, etc.)
}


