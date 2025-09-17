#include <WiFi.h>
#include <WebServer.h>

// --- CONFIGURACIÓN DE RED---
const char* ssid = "MEGACABLE-2.4G-E8AA";
const char* password = "pD9vzFKzK2";

// --- Variables y pines ---
WebServer server(80);
const int redDisplayPins[7] = {4, 26, 27, 14, 12, 13, 16}; 
const int blueDisplayPins[7] = {23, 22, 21, 19, 18, 5, 17};
const byte segmentMap[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, {0, 1, 1, 0, 0, 0, 0}, {1, 1, 0, 1, 1, 0, 1},
  {1, 1, 1, 1, 0, 0, 1}, {0, 1, 1, 0, 0, 1, 1}, {1, 0, 1, 1, 0, 1, 1},
  {1, 0, 1, 1, 1, 1, 1}, {1, 1, 1, 0, 0, 0, 0}, {1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 1, 1}
};
int currentNumber = 0;
bool counting = false;
int countDirection = 1;
unsigned long lastUpdateTime = 0;
const int UPDATE_INTERVAL = 1000;

// --- Funciones "handler" para las rutas HTTP ---
void handleUp() {
  counting = true;
  countDirection = 1;
  server.send(200, "text/plain", "Comando 'ascendente' recibido.");
}

void handleDown() {
  counting = true;
  countDirection = -1;
  server.send(200, "text/plain", "Comando 'descendente' recibido.");
}

// Función 'stop' devuelve el número actual ---
void handleStop() {
  counting = false;
  // Devuelve el número en el que se detuvo el conteo
  server.send(200, "text/plain", String(currentNumber)); 
}

void handleReset() {
  counting = false;
  currentNumber = 0;
  displayNumber(currentNumber);
  server.send(200, "text/plain", "Comando 'reset' recibido.");
}

//Ruta para consultar el número en cualquier momento ---
void handleGetNumber() {
    server.send(200, "text/plain", String(currentNumber));
}

void handleCalculate() {
  if (server.hasArg("value")) {
    String valueStr = server.arg("value");
    int receivedNumber = valueStr.toInt();
    if (receivedNumber >= 0 && receivedNumber <= 99) {
      counting = false;
      currentNumber = receivedNumber;
      displayNumber(currentNumber);
      server.send(200, "text/plain", "Resultado " + valueStr + " mostrado.");
    } else {
      server.send(400, "text/plain", "Error: El valor debe estar entre 0 y 99.");
    }
  } else {
    server.send(400, "text/plain", "Error: Falta el parametro 'value'.");
  }
}

void handleNotFound() {
  server.send(404, "text/plain", "404: No encontrado");
}

// --- setup() y loop() ---
void setup() {
  Serial.begin(115200);

  for (int i = 0; i< 7; i++) {
    pinMode(redDisplayPins[i], OUTPUT);
    pinMode(blueDisplayPins[i], OUTPUT);
  }
  displayNumber(0);

  WiFi.begin(ssid, password);
  Serial.print("Conectando a la red Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Conectado!");
  Serial.print("Dirección IP del ESP32: ");
  Serial.println(WiFi.localIP());

  server.on("/up", handleUp);
  server.on("/down", handleDown);
  server.on("/stop", handleStop);
  server.on("/reset", handleReset);
  server.on("/calculate", handleCalculate);
  server.on("/getnumber", handleGetNumber);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  server.handleClient();

  if (counting && (millis() - lastUpdateTime > UPDATE_INTERVAL)) {
    lastUpdateTime = millis();
    currentNumber += countDirection;
    if (currentNumber > 99) currentNumber = 0;
    if (currentNumber < 0) currentNumber = 99;
    displayNumber(currentNumber);
  }
}

// --- Funciones auxiliares para los displays ---
void displayNumber(int number) {
  if (number < 0 || number > 99) return;
  int tens = number / 10;
  int units = number % 10;
  displayDigit(tens, blueDisplayPins);
  displayDigit(units, redDisplayPins);
}

void displayDigit(int digit, const int displayPins[]) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(displayPins[i], segmentMap[digit][i]);
  }
}