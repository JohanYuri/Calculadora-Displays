//Librerias
#include <Arduino.h>

// Pines para el Display Rojo (Izquierda)
const int redDisplayPins[7] = {25, 26, 27, 14, 12, 13, 33};

// Pines para el Display Azul (Derecha)
const int blueDisplayPins[7] = {23, 22, 21, 19, 18, 5, 17};

// Mapa de segmentos para CÁTODO COMÚN (1 = ON) 
const byte segmentMap[10][7] = {
  {1, 1, 1, 1, 1, 1, 0},  // 0
  {0, 1, 1, 0, 0, 0, 0},  // 1
  {1, 1, 0, 1, 1, 0, 1},  // 2
  {1, 1, 1, 1, 0, 0, 1},  // 3
  {0, 1, 1, 0, 0, 1, 1},  // 4
  {1, 0, 1, 1, 0, 1, 1},  // 5
  {1, 0, 1, 1, 1, 1, 1},  // 6
  {1, 1, 1, 0, 0, 0, 0},  // 7
  {1, 1, 1, 1, 1, 1, 1},  // 8
  {1, 1, 1, 1, 0, 1, 1}   // 9
};

void setup() {
  for (int i = 0; i < 7; i++) {
    pinMode(redDisplayPins[i], OUTPUT);
    pinMode(blueDisplayPins[i], OUTPUT);
  }
}

void loop() {
  for (int number = 99; number >= 0; number--) {
    int tens = number / 10;
    int units = number % 10;

    // --- ¡AQUÍ ESTÁ EL CAMBIO! ---
    // Envía el dígito que cambia lento (decenas) al display de la derecha.
    displayDigit(tens, blueDisplayPins);
    // Envía el dígito que cambia rápido (unidades) al display de la izquierda.
    displayDigit(units, redDisplayPins);
    
    delay(1000);
  }
}

void displayDigit(int digit, const int displayPins[]) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(displayPins[i], segmentMap[digit][i]);
  }
}
