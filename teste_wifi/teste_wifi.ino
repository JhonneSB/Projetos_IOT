/*
  Programa para comunicação Wifi
  SENAI - Timbó
  TEC - Desenvolvimento de Sistemas - Internet das Coisas
*/

#include <WiFi.h>

// Substitua pelo SSID e senha da sua rede WiFi
const char* ssid = "Smart 4.0 (3)";
const char* password = "Smart4.0";

void setup() {
  Serial.begin(115200);          // Inicializa comunicação serial
  WiFi.begin(ssid, password);    // Inicia conexão WiFi

  // Aguarda conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando ao WiFi...");
  }

  Serial.print("Conectado à rede WiFi: ");
  Serial.println(ssid);

  // Exibe informações de rede
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());     // Obtendo o endereço IP

  Serial.print("Endereço MAC: ");
  Serial.println(WiFi.macAddress());  // Obtendo o endereço MAC
}

void loop() {
  // Código principal (fica vazio por enquanto)
}
