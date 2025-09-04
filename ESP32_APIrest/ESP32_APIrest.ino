#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <ESP32Servo.h>

// ----------- Configurações WiFi -----------
const char* ssid = "Smart 4.0 (3)";       // substitua pelo nome da sua rede WiFi
const char* password = "Smart4.0";  // substitua pela senha da sua rede WiFi

// ----------- Configuração do servidor -----------
WebServer server(80);  // Porta 80 padrão HTTP

// ----------- Configuração do DHT11 -----------
#define DHTPIN 15      // GPIO conectado ao DHT11
#define DHTTYPE DHT11  // Tipo do sensor
DHT dht(DHTPIN, DHTTYPE);

// ----------- Configuração do Servo -----------
Servo myServo;
//#define SERVO_PIN 13  // Pino PWM para o servo

#define TRIG_PIN 13
#define ECHO_PIN 35


float distancia;
void handleRoot() {
  server.send(200, "text/plain", "API REST ESP32 Ativa!");
}

void handleDHT() {
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  // Tratamento de erro na leitura
  if (isnan(temperatura) || isnan(umidade)) {
    server.send(500, "application/json", "{\"status\":\"Erro na leitura do DHT11\"}");
    return;
  }

  String json = "{";
  json += "\"temperatura\":" + String(temperatura, 1) + ",";
  json += "\"umidade\":" + String(umidade, 1);
  json += "}";

  server.send(200, "application/json", json);
}

void handlePmt() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms
  if (duracao == 0) {
    distancia = -1; // indicar erro
  } else {
    distancia = duracao * 0.034 / 2;
  }

  Serial.println(distancia);

  String json = "{";
  json += "\"distancia\":" + String(distancia, 2);
  json += "}";

  server.send(200, "application/json", json);
}

// Rota POST para controle do servo
//void handleServo() {
  //if (server.hasArg("angle")) {
    //int angle = server.arg("angle").toInt();

    // Limitar ângulo entre 0 e 180
    //if (angle < 0) angle = 0;
    //if (angle > 180) angle = 180;

    //myServo.write(angle);

    //String json = "{";
    //json += "\"status\":\"OK\",";
    //json += "\"angle\":" + String(angle);
    //json += "}";

    //server.send(200, "application/json", json);
  //} else {
   // server.send(400, "application/json", "{\"status\":\"Erro\",\"mensagem\":\"Parametro angle faltando\"}");
  //}
//}

// ----------- Setup -----------
void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(115200);

  // Inicia WiFi
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  // Inicializa DHT
  dht.begin();

  // Inicializa Servo
  //myServo.attach(SERVO_PIN);
  //myServo.write(90);  // posição inicial

  // Configura rotas da API
  server.on("/", handleRoot);
  server.on("/dht", HTTP_GET, handleDHT);
  server.on("/pmt", HTTP_GET, handlePmt);
  //server.on("/servo", HTTP_POST, handleServo);

  // Inicia servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado!");
}

void loop() {
  server.handleClient();
}

