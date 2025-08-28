#include <WiFi.h>
#include <WiFiServer.h>
#include <esp_wifi.h>

WiFiServer sv(555);
WiFiClient cl;

void setup() {

  Serial.begin(115200);
  WiFi.mode(WIFI_AP);

  WiFi.softAP("EspMiJHO", "");
  sv.begin();
  Serial.println("Servidor TCP iniciado. ");

}

void loop() {
  tcp();

}
void tcp(){
  if (cl.connected())
  {
    if (cl.available() > 0)
    {
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
    }
  }
  else
{
    cl = sv.available();
    delay(1);
}
}

