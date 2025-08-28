#include <LiquidCrystal.h>
#include <WiFi.h>
#include <WiFiServer.h>
#include <esp_wifi.h>
#include "DHT.h"

#define DHTPIN 15
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
#define LED_R 25
#define LED_G 26
#define LED_B 27


WiFiServer sv(555);

LiquidCrystal lcd(32, 33, 23, 22, 21, 18);
WiFiClient cl;
uint32_t Rparte, Gparte, Bparte;
bool reqTemp = false;
String Temp;

void setup() {

  Serial.begin(115200);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);

  WiFi.mode(WIFI_AP);
  dht.begin();

  WiFi.softAP("EspMiJHO", "");
  sv.begin();
  Serial.println("Servidor TCP iniciado. ");
  lcd.begin(16, 2);
}

void loop() {
  tcp();
  if (reqTemp == true) {
    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    Temp = String(Temp, 1);
    lcd.print(Temp);
  }
}
void tcp() {
  if (cl.connected()) {
    if (cl.available() > 0) {
      String req = "";
      while (cl.available() > 0)

      {

        char z = cl.read();
        req += z;
      }

      Serial.print("\nUm client enviou uma mensagem");
      Serial.print("\n...IP do cliente: ");
      Serial.print(cl.remoteIP());
      Serial.print("\n...IP do servidor: ");
      Serial.print(WiFi.softAPIP());
      Serial.print("\n...Mensagem do cliente: " + req + "\n");
      cl.print("\nO servidor recebeu sua mensagem");
      cl.print("\n...Seu IP: ");
      cl.print(cl.remoteIP());
      cl.print("\n...IP do seu Servidor: ");
      cl.print(WiFi.softAPIP());
      cl.print("\n...Sua mensagem: " + req + "\n");

      if (req.startsWith("LEDRGB")) {
        int firstComma = req.indexOf(',');
        int secondComma = req.indexOf(',', firstComma + 1);

        Rparte = req.substring(7, firstComma).toInt();
        Gparte = req.substring(firstComma + 1, secondComma).toInt();
        Bparte = req.substring(secondComma + 1).toInt();

        analogWrite(LED_R, Rparte);
        analogWrite(LED_G, Gparte);
        analogWrite(LED_B, Bparte);

        Serial.print("R: ");
        Serial.println(Rparte);
        Serial.print("G: ");
        Serial.println(Gparte);
        Serial.print("B: ");
        Serial.println(Bparte);

        lcd.setCursor(0, 0);
        lcd.print(req);
      }
      if (req.startsWith("Temperatura")) {
        float temperatura = dht.readTemperature();
        if (isnan(temperatura)) {
          Serial.println("Falha ao ler sensor!");
          delay(2000);
          return;
        }
        reqTemp = true;
        Temp=temperatura;
      }
    }
  } else {
    cl = sv.available();
    delay(1);
  }
}
