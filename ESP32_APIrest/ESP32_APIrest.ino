#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <ESP32Servo.h>

// ----------- Configurações WiFi -----------
const char* ssid = "Smart 4.0 (3)";  // substitua pelo nome da sua rede WiFi
const char* password = "Smart4.0";   // substitua pela senha da sua rede WiFi

// ----------- Configuração do servidor -----------
WebServer server(80);  // Porta 80 padrão HTTP

// ----------- Configuração do DHT11 -----------
#define DHTPIN 15      // GPIO conectado ao DHT11
#define DHTTYPE DHT11  // Tipo do sensor
DHT dht(DHTPIN, DHTTYPE);
#define RGB_R 25
#define RGB_G 26
#define RGB_B 27


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

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);  // timeout 30ms
  if (duracao == 0) {
    distancia = -1;  // indicar erro
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

void handleAllData() {
  // Lê dados do DHT11
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
  
  // Lê distância do sensor ultrassônico
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duracao == 0) {
    distancia = -1;  // indicar erro
  } else {
    distancia = duracao * 0.034 / 2;
  }
  
  // Lê estado do LED (RGB_B)
  int estadoLed = digitalRead(RGB_B);
  String estadoLedStr = (estadoLed == HIGH) ? "LIGADO" : "DESLIGADO";
  
  // Verifica erros na leitura do DHT11
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Erro na leitura do DHT11!");
    server.send(500, "application/json", "{\"status\":\"Erro na leitura do DHT11\"}");
    return;
  }
  
  // Constrói o JSON com todos os dados
  String json = "{";
  json += "\"temperatura\":" + String(temperatura, 1) + ",";
  json += "\"umidade\":" + String(umidade, 1) + ",";
  json += "\"distancia\":" + String(distancia, 2) + ",";
  json += "\"led_estado\":\"" + estadoLedStr + "\"";
  json += "}";
  
  // Prints para validação no terminal
  Serial.println("=== DADOS DOS SENSORES ===");
  Serial.print("Temperatura: ");
  Serial.print(temperatura, 1);
  Serial.println(" °C");
  
  Serial.print("Umidade: ");
  Serial.print(umidade, 1);
  Serial.println(" %");
  
  Serial.print("Distância: ");
  Serial.print(distancia, 2);
  Serial.println(" cm");
  
  Serial.print("LED Status: ");
  Serial.print(estadoLed);
  Serial.print(" (");
  Serial.print(estadoLedStr);
  Serial.println(")");
  
  Serial.println("JSON Gerado:");
  Serial.println(json);
  Serial.println("==========================");
  
  server.send(200, "application/json", json);
}


void ultrasonico() {
  //Envio do pulso ultrasonico
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  //Cálculo da medida

  long duracao = pulseIn(ECHO_PIN, HIGH);
  distancia = duracao * 0.034 / 2;

  //int valor = analogRead(PIN_POT);  // Lê o valor do potenciômetro (0 a 1023)
  //float tensao = valor * (5.0 / 1023.0);  // Converte para tensão (0.00 a 5.00)

  //lcd.setCursor(0, 0);
  //lcd.print("DIGITAL:      ");
  //lcd.setCursor(9, 0);
  //lcd.print(valor);

  //lcd.setCursor(0, 1);
  //lcd.print("TENSAO: ");
  //lcd.print(tensao, 2);  
  //lcd.print(" V   ");     // Espaço extra para limpar sobras
}

// ----------- Setup -----------
void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);

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
  server.on("/ledOn", HTTP_GET, handleLedOn);
  server.on("/ledOff", HTTP_GET, handleLedOff);
  server.on("/alldata", HTTP_GET, handleAllData);
  //server.on("/servo", HTTP_POST, handleServo);

  // Inicia servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado!");
}

void loop() {
  server.handleClient();
}
