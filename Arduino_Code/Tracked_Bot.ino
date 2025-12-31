#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>

// -----------------------------
// WiFi Credentials
// -----------------------------
const char* ssid = "Sanjeev"; 
const char* password = "98765432"; 

WebServer server(80);

// -----------------------------
// Motor Pins
// -----------------------------
#define IN1 5
#define IN2 18
#define IN3 19
#define IN4 21
#define ENA 14
#define ENB 32

// -----------------------------
// PWM
// -----------------------------
#define PWM_FREQ 1000
#define PWM_RES  8
#define PWM_CH_A 0
#define PWM_CH_B 1

// -----------------------------
// Encoder Pins
// -----------------------------
#define L_ENC_A 17
#define L_ENC_B 16
#define R_ENC_A 26
#define R_ENC_B 27

volatile long leftPulses = 0;
volatile long rightPulses = 0;

long leftStart = 0;
long rightStart = 0;

// -----------------------------
// Path Memory for Return to Home
// -----------------------------
struct Step {
  char action;   // F, B, L, R
  int duration;  // milliseconds
};

Step path[300];      // store up to 300 moves
int stepIndex = 0;
unsigned long moveStartTime = 0;
char currentAction = 'S'; // stopped

void recordAction(char act) {
  if (currentAction != act) {
    // store previous action with duration
    if (currentAction != 'S') {
      path[stepIndex].action = currentAction;
      path[stepIndex].duration = millis() - moveStartTime;
      stepIndex++;
    }
    // begin new action
    currentAction = act;
    moveStartTime = millis();
  }
}

// -----------------------------
// Quadrature Encoder ISR
// -----------------------------
void IRAM_ATTR leftEncoderISR() {
  int A = digitalRead(L_ENC_A);
  int B = digitalRead(L_ENC_B);
  if (A == B) leftPulses++; else leftPulses--;
}

void IRAM_ATTR rightEncoderISR() {
  int A = digitalRead(R_ENC_A);
  int B = digitalRead(R_ENC_B);
  if (A == B) rightPulses++; else rightPulses--;
}

// -----------------------------
// Motor Movement
// -----------------------------
void forward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  ledcWrite(PWM_CH_A, 200);
  ledcWrite(PWM_CH_B, 200);
}

void backward() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  ledcWrite(PWM_CH_A, 200);
  ledcWrite(PWM_CH_B, 200);
}

void left() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  ledcWrite(PWM_CH_A, 200);
  ledcWrite(PWM_CH_B, 200);
}

void right() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  ledcWrite(PWM_CH_A, 200);
  ledcWrite(PWM_CH_B, 200);
}

void stopAll() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  ledcWrite(PWM_CH_A, 0);
  ledcWrite(PWM_CH_B, 0);
}

// -----------------------------
// RETURN TO HOME (FIXED WITH PATH MEMORY)
// -----------------------------
void returnHome() {

  // finish last action
  if (currentAction != 'S') {
    path[stepIndex].action = currentAction;
    path[stepIndex].duration = millis() - moveStartTime;
    stepIndex++;
  }

  stopAll();
  delay(200);

  Serial.println("Returning Home...");
  Serial.println("---------------------");

  // play actions in reverse
  for (int i = stepIndex - 1; i >= 0; i--) {

    char act = path[i].action;
    int dur = path[i].duration;

    Serial.print("Undo: ");
    Serial.print(act);
    Serial.print("   Duration: ");
    Serial.println(dur);

    switch (act) {
      case 'F': backward(); break;
      case 'B': forward();  break;
      case 'L': right();    break;
      case 'R': left();     break;
    }

    delay(dur);
    stopAll();
    delay(120);
  }

  // Clear path memory
  stepIndex = 0;
  currentAction = 'S';

  Serial.println("Reached home!");
}

// -----------------------------
// HTML UI
// -----------------------------
String page = R"=====( 
<!DOCTYPE html>
<html>
<head>
<title>Rover</title>
<style>
button {
  width:140px; height:50px;
  margin:6px;
  font-size:20px;
}
</style>
</head>
<body>
<h2>ESP32 Rover Controller</h2>

<button onclick="fetch('/forward')">Forward</button><br>
<button onclick="fetch('/left')">Left</button>
<button onclick="fetch('/stop')">Stop</button>
<button onclick="fetch('/right')">Right</button><br>
<button onclick="fetch('/backward')">Backward</button><br><br>

<button onclick="fetch('/home')" style="background:red;color:white;">
Return to Home
</button>

</body>
</html>
)=====";

// -----------------------------
// Web Endpoints
// -----------------------------
void handleRoot() { server.send(200, "text/html", page); }
void handleF() { recordAction('F'); forward(); server.send(200, "text/plain", "Forward"); }
void handleB() { recordAction('B'); backward(); server.send(200, "text/plain", "Backward"); }
void handleL() { recordAction('L'); left(); server.send(200, "text/plain", "Left"); }
void handleR() { recordAction('R'); right(); server.send(200, "text/plain", "Right"); }
void handleS() { recordAction('S'); stopAll(); server.send(200, "text/plain", "Stopped"); }
void handleHome() { returnHome(); server.send(200, "text/plain", "Returning Home"); }

// -----------------------------
// Setup
// -----------------------------
void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  pinMode(L_ENC_A, INPUT_PULLUP);
  pinMode(L_ENC_B, INPUT_PULLUP);
  pinMode(R_ENC_A, INPUT_PULLUP);
  pinMode(R_ENC_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(L_ENC_A), leftEncoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(R_ENC_A), rightEncoderISR, CHANGE);

  leftStart = 0;
  rightStart = 0;

  // PWM Setup
  ledcSetup(PWM_CH_A, PWM_FREQ, PWM_RES);
  ledcSetup(PWM_CH_B, PWM_FREQ, PWM_RES);
  ledcAttachPin(ENA, PWM_CH_A);
  ledcAttachPin(ENB, PWM_CH_B);

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/forward", handleF);
  server.on("/backward", handleB);
  server.on("/left", handleL);
  server.on("/right", handleR);
  server.on("/stop", handleS);
  server.on("/home", handleHome);

  server.begin();
}

// -----------------------------
// Loop
// -----------------------------
void loop() {
  server.handleClient();
}
