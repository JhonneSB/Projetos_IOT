/*
  Programa para comunicação Wifi
  SENAI - Timbó
  TEC - Desenvolvimento de Sistemas - Internet das Coisas
*/

#include <WiFi.h>

// Substitua pelo SSID e senha da sua rede WiFi
const char* ssid = "Smart 4.0 (3)";
const char* password = "Smart4.0";
int LED = 27;
WiFiServer server(80);

void setup() {
  Serial.begin(115200);  // Inicializa comunicação serial
  pinMode(LED, OUTPUT);

  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  // Aguarda conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }

  Serial.print(" ");
  Serial.println("WiFi conectada.");
  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.println("<!DOCTYPE html>");
            client.println("<html lang='pt-BR'>");
            client.println("<head>");
            client.println("<meta charset='UTF-8'>");
            client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
            client.println("<title>Controle de LED</title>");
            client.println("<style>");
            client.println("body { font-family: Arial, sans-serif; background-color: #f0f0f0; text-align: center; padding: 50px; }");
            client.println("h1 { color: #333; }");
            client.println(".button { display: inline-block; padding: 15px 30px; font-size: 18px; margin: 20px; cursor: pointer;");
            client.println("text-decoration: none; color: white; border: none; border-radius: 5px; transition: background-color 0.3s ease; }");
            client.println(".on { background-color: #28a745; }");
            client.println(".on:hover { background-color: #218838; }");
            client.println(".off { background-color: #dc3545; }");
            client.println(".off:hover { background-color: #c82333; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>Controle de LED - GPIO 27</h1>");
            client.println("<a href=\"/H\" class=\"button on\">Ligar LED</a>");
            client.println("<a href=\"/L\" class=\"button off\">Desligar LED</a>");
            client.println("</body>");
            client.println("</html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED, HIGH);
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED, LOW);
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
