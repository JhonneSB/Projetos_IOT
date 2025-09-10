#include <WiFi.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT11

// Configuração da rede WiFi
const char* ssid = "Smart 4.0 (3)";
const char* password = "Smart4.0";

// Servidor TCP na porta 555
WiFiServer server(555);
WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);

// Variáveis simulando sensores
float temp;
float umid;

void setup() {
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
  server.begin();
  Serial.println("Servidor TCP iniciado na porta 555.");
}

void loop() {
  float umid = dht.readHumidity();
  float temp = dht.readTemperature();
  
  // Aceita novos clientes se não houver nenhum conectado
  if (!client.connected()) {
    client = server.available();
  }

  // Se houver cliente conectado, envia TEMP e UMID continuamente
  if (client.connected()) {
    if (!isnan(umid) && !isnan(temp)) {
      client.println("TEMP=" + String(temp, 2));
      client.println("UMID=" + String(umid, 1));
      delay(2000); // envia a cada 2 segundos
    } else {
      client.println("ERROR:SENSOR_FAIL");
      Serial.println("Erro na leitura de um ou mais sensores");
    }
  }

  delay(10);
}