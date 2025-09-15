#include <WiFi.h>
#include <WebServer.h>

// --- ¡CONFIGURACIÓN DE RED! ---
const char* ssid = "MEGACABLE-2.4G-E8AA";
const char* password = "pD9vzFKzK2";

// Se crea el objeto servidor en el puerto 80 (estándar para HTTP)
WebServer server(80);

// Display Rojo (Unidades) {a,b,c,d,e,f,g}
const int redDisplayPins[7] = {4, 26, 27, 14, 12, 13, 16}; 

// Display Azul (Decenas) {a,b,c,d,e,f,g}
const int blueDisplayPins[7] = {23, 22, 21, 19, 18, 5, 17};


// --- Mapa de segmentos (sin cambios) ---
// Posiciones:                a, b, c, d, e, f, g
const byte segmentMap[10][7] = {
  {1, 1, 1, 1, 1, 1, 0},   // 0
  {0, 1, 1, 0, 0, 0, 0},   // 1
  {1, 1, 0, 1, 1, 0, 1},   // 2
  {1, 1, 1, 1, 0, 0, 1},   // 3
  {0, 1, 1, 0, 0, 1, 1},   // 4
  {1, 0, 1, 1, 0, 1, 1},   // 5
  {1, 0, 1, 1, 1, 1, 1},   // 6
  {1, 1, 1, 0, 0, 0, 0},   // 7
  {1, 1, 1, 1, 1, 1, 1},   // 8
  {1, 1, 1, 0, 0, 1, 1}    // 9
};

// --- Variables de estado (sin cambios) ---
int currentNumber = 0;
bool counting = false;
int countDirection = 1;
unsigned long lastUpdateTime = 0;
const int UPDATE_INTERVAL = 1000;

// --- Funciones "handler" para las rutas HTTP (sin cambios) ---
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

void handleStop() {
  counting = false;
  server.send(200, "text/plain", "Comando 'parar' recibido.");
}

void handleReset() {
  counting = false;
  currentNumber = 0;
  displayNumber(currentNumber);
  server.send(200, "text/plain", "Comando 'reset' recibido.");
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

// --- setup() y loop() (sin cambios) ---
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

// --- Funciones auxiliares para los displays (sin cambios) ---
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