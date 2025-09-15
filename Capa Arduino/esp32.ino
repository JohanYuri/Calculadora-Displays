#include <WiFi.h>
#include <WebServer.h>

// --- ¡CONFIGURACIÓN DE RED! ---
const char* ssid = "TU_NOMBRE_DE_RED";
const char* password = "TU_CONTRASEÑA";

// Se crea el objeto servidor en el puerto 80 (estándar para HTTP)
WebServer server(80);

// --- Tu configuración de pines y mapa de segmentos (sin cambios) ---
const int redDisplayPins[7] = {25, 26, 27, 14, 12, 13, 33}; // Display Izquierda (Unidades)
const int blueDisplayPins[7] = {23, 22, 21, 19, 18, 5, 17}; // Display Derecha (Decenas)
const byte segmentMap[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, {0, 1, 1, 0, 0, 0, 0}, {1, 1, 0, 1, 1, 0, 1},
  {1, 1, 1, 1, 0, 0, 1}, {0, 1, 1, 0, 0, 1, 1}, {1, 0, 1, 1, 0, 1, 1},
  {1, 0, 1, 1, 1, 1, 1}, {1, 1, 1, 0, 0, 0, 0}, {1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 0, 1, 1}
};

// Variables de estado (sin cambios)
int currentNumber = 0;
bool counting = false;
int countDirection = 1;
unsigned long lastUpdateTime = 0;
const int UPDATE_INTERVAL = 1000;

// --- FUNCIONES "HANDLER" PARA LAS RUTAS HTTP ---
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

void setup() {
  Serial.begin(115200);

  // Configuración de pines (sin cambios)
  for (int i = 0; i < 7; i++) {
    pinMode(redDisplayPins[i], OUTPUT);
    pinMode(blueDisplayPins[i], OUTPUT);
  }
  displayNumber(0);

  // --- Conexión a la red Wi-Fi ---
  WiFi.begin(ssid, password);
  Serial.print("Conectando a la red Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Conectado!");
  Serial.print("Dirección IP del ESP32: ");
  Serial.println(WiFi.localIP()); // ¡ESTA IP ES LA QUE USARÁS EN PYTHON!

  // --- Definición de las rutas del servidor ---
  server.on("/up", handleUp);
  server.on("/down", handleDown);
  server.on("/stop", handleStop);
  server.on("/reset", handleReset);
  server.on("/calculate", handleCalculate);
  server.onNotFound(handleNotFound);

  server.begin(); // Inicia el servidor
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  server.handleClient(); // Atiende las peticiones de los clientes

  // Lógica de conteo no bloqueante (sin cambios)
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