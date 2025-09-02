#include <WiFi.h>
#include <WiFiManager.h> // Biblioteca WiFi Manager

//Definições de hardware - LED RGB
#define LED_RED 25    // Pino do LED Vermelho
#define LED_GREEN 26  // Pino do LED Verde  
#define LED_BLUE 27   // Pino do LED Azul
#define RESET_BUTTON 0 // BOTÃO DE RESET NO GPIO - SW1

//Instancia do WiFiManager
WiFiManager wm;

// Variáveis para controle do botão
unsigned long buttonPressTime = 0;
bool buttonPressed = false;
const unsigned long RESET_DELAY = 5000; // 5 segundos

// Protótipos das funções de callback
void configModeCallback(WiFiManager *myWiFiManager);
void saveConfigCallback();
void resetConfigCallback();

// Função para controlar cores do LED
void setLEDColor(int red, int green, int blue) {
  analogWrite(LED_RED, red);
  analogWrite(LED_GREEN, green);
  analogWrite(LED_BLUE, blue);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Iniciando ESP32 com WiFiManager ===");

  //Configuração dos pinos do LED RGB
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(RESET_BUTTON, INPUT_PULLUP);
  
  // Inicia com LED desligado
  setLEDColor(0, 0, 0);

  //Associa as funções de callback
  wm.setAPCallback(configModeCallback);
  wm.setSaveConfigCallback(saveConfigCallback);
  wm.setConfigResetCallback(resetConfigCallback);

  //Verifica se o botão está pressionado na inicialização por 5 segundos
  Serial.println("Aguardando 5 segundos para verificar botão reset...");
  unsigned long startTime = millis();
  
  while (millis() - startTime < RESET_DELAY) {
    if (digitalRead(RESET_BUTTON) == LOW) {
      // Botão pressionado, LED fica AZUL enquanto pressionado
      setLEDColor(0, 0, 255);
      delay(RESET_DELAY - (millis() - startTime));
      
      if (digitalRead(RESET_BUTTON) == LOW) {
        // Botão ainda pressionado após 5 segundos
        Serial.println("Botão pressionado por 5 segundos na inicialização!");
        Serial.println("Limpando credenciais WiFi...");
        wm.resetSettings();
        setLEDColor(0, 0, 0); // LED desligado
        delay(1000);
        break;
      }
    }
    delay(10);
  }

  // LED piscando em AZUL durante tentativa de conexão
  setLEDColor(0, 0, 100); // Azul fraco piscando

  //Tenta conectar ao último WiFi salvo
  if(!wm.autoConnect("ESP32_MIJHO", "12121212")) {
    Serial.println("Falha ao conectar e tempo limite esgotado.");
    Serial.println("Reiniciando ESP32...");
    ESP.restart();
    delay(1000);
  }

  Serial.println("Conectado ao WiFi com sucesso!");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  // LED AZUL fixo quando conectado
  setLEDColor(0, 0, 255);
  Serial.println("LED AZUL: Conectado ao WiFi!");
}

void loop() {
  // Se perder a conexão, pisca o LED em AZUL
  if(WiFi.status() != WL_CONNECTED) {
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 500) {
      static bool ledState = false;
      if (ledState) {
        setLEDColor(0, 0, 100); // Azul fraco
      } else {
        setLEDColor(0, 0, 0);   // Desligado
      }
      ledState = !ledState;
      lastBlink = millis();
      Serial.println("Tentando reconectar...");
    }
  } else {
    // Conectado - LED AZUL fixo
    setLEDColor(0, 0, 255);
  }
  
  //Verifica se o botão foi pressionado durante a execução
  if (digitalRead(RESET_BUTTON) == LOW) {
    if (!buttonPressed) {
      // Primeira vez que o botão é pressionado
      buttonPressed = true;
      buttonPressTime = millis();
      Serial.println("Botão pressionado. Mantenha pressionado por 5 segundos para resetar.");
      setLEDColor(0, 0, 100); // Azul fraco enquanto pressionado
    }
    
    // Verifica se o botão foi pressionado por 5 segundos
    if (buttonPressed && (millis() - buttonPressTime >= RESET_DELAY)) {
      Serial.println("Botão pressionado por 5 segundos!");
      Serial.println("Limpando credenciais e reiniciando...");
      wm.resetSettings();
      setLEDColor(0, 0, 255); // Azul forte antes de resetar
      delay(1000);
      ESP.restart();
    }
  } else {
    // Botão não está pressionado
    if (buttonPressed) {
      // Botão foi liberado antes de completar 5 segundos
      buttonPressed = false;
      Serial.println("Botão liberado antes de 5 segundos. Nada acontece.");
      // Restaura a cor do LED baseado na conexão WiFi
      if (WiFi.status() == WL_CONNECTED) {
        setLEDColor(0, 0, 255); // Azul se conectado
      } else {
        setLEDColor(0, 0, 0);   // Desligado se não conectado
      }
    }
  }
  
  delay(10);
}

void configModeCallback(WiFiManager *myWiFiManager){
  Serial.println("Entrou no modo AP (configuração)");
  Serial.print("SSID do AP: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.print("IP do AP: ");
  Serial.println(WiFi.softAPIP());
  // LED piscando em outra cor (ex: vermelho) no modo AP
  setLEDColor(100, 0, 0);
}

void saveConfigCallback() {
  Serial.println("Novas credenciais WiFi salvas na memória!");
}

void resetConfigCallback() {
  Serial.println("Credenciais WiFi apagadas (resetConfigCallback).");
}