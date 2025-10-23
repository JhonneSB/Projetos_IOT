#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal.h>
#include <WiFiManager.h>

// ======= CONFIGURAÇÕES DE HARDWARE =======
#define LED_RED       27
#define LED_GREEN     26
#define LED_BLUE      25

// Botões
#define SW1 0
#define SW2 4

// LCD paralelo
#define LCD_RS 32
#define LCD_EN 33
#define LCD_D4 23
#define LCD_D5 22
#define LCD_D6 21
#define LCD_D7 18

// ======= INSTÂNCIAS =======
WiFiManager wm;
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
WiFiClient espClient;
PubSubClient client(espClient);

// ======= VARIÁVEIS GLOBAIS =======
int paginaAtual = 0;
bool wifiConnected = false;
bool mqttConnected = false;

#define MAX_VARS 20
String history[MAX_VARS];
int varCount = 0;

// ======= CONFIGURAÇÃO SIMPLIFICADA =======

void setupLEDs() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  setLEDStatus();
}

void setLEDStatus() {
  if (!wifiConnected) {
    analogWrite(LED_RED, 255);
    analogWrite(LED_GREEN, 0);
    analogWrite(LED_BLUE, 0);
  } else if (!mqttConnected) {
    analogWrite(LED_RED, 0);
    analogWrite(LED_GREEN, 0);
    analogWrite(LED_BLUE, 255);
  } else {
    analogWrite(LED_RED, 0);
    analogWrite(LED_GREEN, 255);
    analogWrite(LED_BLUE, 0);
  }
}

void mostrarPagina(int pagina) {
  lcd.clear();
  
  if (varCount == 0) {
    lcd.setCursor(0,0);
    if (!wifiConnected) {
      lcd.print("Conect. WiFi...");
    } else if (!mqttConnected) {
      lcd.print("Conect. MQTT...");
    } else {
      lcd.print("Aguardando...");
    }
    lcd.setCursor(0,1);
    lcd.print("Dados MQTT");
    return;
  }
  
  int idx = pagina % varCount;
  lcd.setCursor(0,0);
  lcd.print("Var ");
  lcd.print(idx + 1);
  lcd.print("/");
  lcd.print(varCount);
  
  String displayLine = history[idx];
  if (displayLine.length() > 16) {
    displayLine = displayLine.substring(0, 16);
  }
  lcd.setCursor(0,1);
  lcd.print(displayLine);
}

// ======= WIFI MANAGER CONFIG =======

void setupWiFi() {
  Serial.println("Configurando WiFi...");
  
  // Configurações do WiFiManager
  wm.setConfigPortalTimeout(180); // 3 minutos timeout
  wm.setConnectTimeout(30); // 30 segundos para conectar
  wm.setDebugOutput(true);
  
  // Remove configurações anteriores para teste
  // wm.resetSettings();
  
  lcd.clear();
  lcd.print("Conectando WiFi");
  lcd.setCursor(0,1);
  lcd.print("Aguarde...");
  
  bool res;
  // Tenta conectar com credenciais salvas
  res = wm.autoConnect("ESP32_SMART", "12345678");
  
  if (!res) {
    Serial.println("Falha na conexão WiFi");
    lcd.clear();
    lcd.print("Modo AP Ativo");
    lcd.setCursor(0,1);
    lcd.print("ESP32_SMART");
    // Não reinicia - fica no modo AP
    wifiConnected = false;
  } else {
    Serial.println("WiFi conectado!");
    wifiConnected = true;
    lcd.clear();
    lcd.print("WiFi Conectado!");
    delay(2000);
  }
  
  setLEDStatus();
}

// ======= MQTT CONFIG =======

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("\n═══════════════════════════════════════");
  Serial.println("📨 NOVA MENSAGEM MQTT RECEBIDA");
  Serial.println("═══════════════════════════════════════");
  
  // Informações básicas
  Serial.print("📡 Tópico: ");
  Serial.println(topic);
  Serial.print("📏 Tamanho do payload: ");
  Serial.println(length);
  
  // Converte payload para string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  // Debug detalhado do payload
  Serial.print("📦 Payload (string): ");
  Serial.println(message);
  
  Serial.print("📦 Payload (hex): ");
  for (int i = 0; i < length; i++) {
    Serial.print(payload[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // Mostra cada caractere individualmente
  Serial.println("🔍 Análise caractere por caractere:");
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    Serial.print("Pos ");
    Serial.print(i);
    Serial.print(": '");
    Serial.print(c);
    Serial.print("' (ASCII: ");
    Serial.print((int)c);
    Serial.print(", HEX: ");
    Serial.print(payload[i], HEX);
    Serial.println(")");
  }
  
  // Tenta interpretar como JSON
  Serial.println("🔄 Tentando interpretar como JSON...");
  
  // Primeiro, tenta fazer parsing direto
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, message);
  
  if (error) {
    Serial.print("❌ Erro no parsing JSON: ");
    Serial.println(error.c_str());
    Serial.println("📋 Tentando análise manual...");
    
    // Análise manual do conteúdo
    if (message.length() == 0) {
      Serial.println("⚠️  Mensagem vazia!");
    } else if (message.startsWith("{") && message.endsWith("}")) {
      Serial.println("✅ Estrutura parece JSON (começa com { e termina com })");
      
      // Tenta corrigir JSON malformado
      String fixedMessage = message;
      fixedMessage.replace(" ", ""); // Remove espaços
      Serial.print("🛠️  JSON sem espaços: ");
      Serial.println(fixedMessage);
      
    } else {
      Serial.println("⚠️  Estrutura não parece JSON");
    }
    
    // Processamento simplificado para teste
    varCount = 1;
    history[0] = "Raw:" + message.substring(0, 14);
    
  } else {
    Serial.println("✅ JSON parseado com sucesso!");
    
    // Extrai variáveis do JSON
    varCount = 0;
    JsonObject obj = doc.as<JsonObject>();
    
    Serial.println("📊 Variáveis encontradas:");
    for (JsonPair kv : obj) {
      if (varCount < MAX_VARS) {
        String key = String(kv.key().c_str());
        String value;
        
        // Converte valor baseado no tipo
        if (kv.value().is<float>()) {
          value = String(kv.value().as<float>(), 2);
        } else if (kv.value().is<int>()) {
          value = String(kv.value().as<int>());
        } else if (kv.value().is<String>()) {
          value = kv.value().as<String>();
        } else {
          value = "null";
        }
        
        history[varCount] = key + ":" + value;
        
        Serial.print("  ");
        Serial.print(varCount + 1);
        Serial.print(". ");
        Serial.print(key);
        Serial.print(" = ");
        Serial.println(value);
        
        varCount++;
      }
    }
  }
  
  Serial.print("🎯 Total de variáveis processadas: ");
  Serial.println(varCount);
  Serial.println("═══════════════════════════════════════\n");
  
  mostrarPagina(paginaAtual);
}

void setupMQTT() {
  client.setServer("10.74.241.66", 1883);
  client.setCallback(callback);
}

void reconnectMQTT() {
  if (!wifiConnected) return;
  
  Serial.print("Conectando MQTT...");
  String clientId = "ESP32_Client_" + String(random(0xffff));
  
  if (client.connect(clientId.c_str())) {
    Serial.println("conectado!");
    client.subscribe("/smart");
    mqttConnected = true;
    lcd.clear();
    lcd.print("MQTT Conectado!");
    delay(1000);
    mostrarPagina(paginaAtual);
  } else {
    Serial.print("falha, rc=");
    Serial.println(client.state());
    mqttConnected = false;
  }
  setLEDStatus();
}

// ======= SETUP PRINCIPAL =======

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== INICIANDO ESP32 ===");
  
  // Hardware
  setupLEDs();
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  
  // LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Iniciando...");
  delay(1000);
  
  // WiFi
  setupWiFi();
  
  // MQTT (só se WiFi conectado)
  if (wifiConnected) {
    setupMQTT();
  }
  
  mostrarPagina(paginaAtual);
}

// ======= LOOP PRINCIPAL =======

void loop() {
  // Verifica WiFi
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    mqttConnected = false;
    setLEDStatus();
    
    // Tenta reconectar WiFi periodicamente
    static unsigned long lastWiFiReconnect = 0;
    if (millis() - lastWiFiReconnect > 10000) {
      Serial.println("Reconectando WiFi...");
      WiFi.reconnect();
      lastWiFiReconnect = millis();
    }
  } else {
    wifiConnected = true;
  }
  
  // MQTT (só se WiFi conectado)
  if (wifiConnected) {
    if (!client.connected()) {
      reconnectMQTT();
    }
    client.loop();
  }
  
  // Botões
  if (digitalRead(SW1) == LOW) {
    if (varCount > 0) {
      paginaAtual = (paginaAtual - 1 + varCount) % varCount;
      mostrarPagina(paginaAtual);
    }
    delay(300);
  }
  
  if (digitalRead(SW2) == LOW) {
    if (varCount > 0) {
      paginaAtual = (paginaAtual + 1) % varCount;
      mostrarPagina(paginaAtual);
    }
    delay(300);
  }
  
  // Reset WiFi se SW1 pressionado por 5s
  static unsigned long buttonTime = 0;
  static bool resetTriggered = false;
  
  if (digitalRead(SW1) == LOW && !resetTriggered) {
    if (buttonTime == 0) {
      buttonTime = millis();
    } else if (millis() - buttonTime > 5000) {
      Serial.println("Reset WiFi solicitado!");
      lcd.clear();
      lcd.print("Reset WiFi...");
      wm.resetSettings();
      delay(2000);
      ESP.restart();
    }
  } else {
    buttonTime = 0;
    resetTriggered = false;
  }
  
  delay(100);
}