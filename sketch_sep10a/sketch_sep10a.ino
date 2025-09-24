#include <WiFi.h>
#include <DHT.h>
#include <WebServer.h>
#define DHTPIN 15
#define DHTTYPE DHT11
#define RGB_R 25
#define RGB_G 26
#define RGB_B 27
// Configuração da rede WiFi
const char* ssid = "Smart 4.0 (3)";
const char* password = "Smart4.0";

WebServer server(80);  // Porta 80 padrão HTTP
WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);

void handleRoot() {
  server.send(200, "text/plain", "API REST ESP32 Ativa!");
}

void handleLedOn() {

  digitalWrite(RGB_B, HIGH);
  String json = "{";
  json += "\"Led ON\""
          ",";
  json += "}";
  server.send(200, "application/json", json);
}

void handleLedOff() {

  digitalWrite(RGB_B, LOW);
  String json = "{";
  json += "\"Led OFF\""
          ",";
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
  Serial.begin(115200);
  dht.begin();

  // Conecta o ESP32 como cliente de rede Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando-se à rede WiFi: ");
  Serial.println(ssid);

  // Aguarda conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado com sucesso!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Inicia o servidor TCP
  server.on("/", handleRoot);
  server.on("/ledOn", HTTP_GET, handleLedOn);
  server.on("/ledOff", HTTP_GET, handleLedOff);

  server.begin();
  Serial.println("Servidor TCP iniciado na porta 555.");
}


void loop() {

    server.handleClient();

}