#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <LiquidCrystal.h>

// ----------- Configurações WiFi -----------
const char* ssid = "Smart 4.0 (3)";
const char* password = "Smart4.0";

// ----------- Configuração do servidor -----------
WebServer server(80);

// ----------- Configuração dos pinos -----------
#define RGB_R 25
#define RGB_G 26
#define RGB_B 27
#define PIN_BTN2 0
#define PIN_BTN3 4
#define PIN_BTN4 17
#define STEP = 5;
#define DIR = 22;

//LiquidCrystal lcd(32, 33, 23, 22, 21, 18);

bool posicao = false;
int passos;
unsigned long ultimoPasso = 0;
const unsigned long intervaloPasso = 500; // 500ms entre passos

// Variáveis para armazenar o último resultado
int lastPos = 0;
bool lastBtn2State = false;
bool lastBtn3State = false;
bool lastBtn4State = false;

void handleRoot() {
  server.send(200, "text/plain", "API REST ESP32 Ativa!");
}

void handleLastResult() {
  // Lê o estado atual dos botões
  bool btn2State = (digitalRead(PIN_BTN2) == LOW); // Se LOW = pressionado (1)
  bool btn3State = (digitalRead(PIN_BTN3) == LOW); // Se LOW = pressionado (1)
  bool btn4State = (digitalRead(PIN_BTN4) == LOW); // Se LOW = pressionado (1)
  
  // Determina qual posição está ativa
  int currentPos = 0;
  if (btn2State) currentPos = 1;
  else if (btn3State) currentPos = 2;
  else if (btn4State) currentPos = 3;
  
  lastPos = currentPos;
  lastBtn2State = btn2State;
  lastBtn3State = btn3State;
  lastBtn4State = btn4State;
  
    String jsonResponse = "{";
  jsonResponse += "\"status\": \"POS ATINGIDA\",";
  jsonResponse += "\"pos_solicitada\": " + String(currentPos) + ",";
  jsonResponse += "\"pos_atual\": " + String(currentPos) + ",";
  jsonResponse += "\"fim_curso1\": " + String(btn2State ? 1 : 0) + ",";
  jsonResponse += "\"fim_curso2\": " + String(btn3State ? 1 : 0) + ",";
  jsonResponse += "\"fim_curso3\": " + String(btn4State ? 1 : 0);
  jsonResponse += "}";
  
  server.send(200, "application/json", jsonResponse);
}

void handleMovePos() {
  if (server.hasArg("pos")) {
    String posStr = server.arg("pos");
    int pos = posStr.toInt();

    Serial.print("Recebi posicao: ");
    Serial.println(pos);

    int pinSelecionado;
    if (pos == 1) {
      pinSelecionado = PIN_BTN2;
    } else if (pos == 2) {
      pinSelecionado = PIN_BTN3;
    } else if (pos == 3) {
      pinSelecionado = PIN_BTN4;
    } else {
      server.send(400, "text/plain", "Posicao invalida");
      return;
    }

    server.send(200, "text/plain", "Posicao recebida: " + posStr);

    Serial.println("Esperando botão...");
    passos = 0;
    ultimoPasso = 0;
    
   // lcd.clear();
   // lcd.setCursor(0, 0);
   // lcd.print("STEP-MOTOR_MOVE ");
    
    bool botaoPressionado = false;
    bool executando = true;
    
    while (executando) {
      // Verifica continuamente o estado do botão
      if (digitalRead(pinSelecionado) == HIGH) {
        if (!botaoPressionado) {
          // Botão acabou de ser pressionado
          botaoPressionado = true;
          digitalWrite(RGB_B, LOW);
          digitalWrite(RGB_R, LOW);
          digitalWrite(RGB_G, HIGH);
        }
        
        // Verifica se é hora de incrementar o passo
        unsigned long tempoAtual = millis();
        if (tempoAtual - ultimoPasso >= intervaloPasso) {
          passos++;
          ultimoPasso = tempoAtual;
          
          Serial.print("Passo: ");
          Serial.println(passos);
          
   //       lcd.setCursor(0, 1);
    //      lcd.print("Passos: ");
     //     lcd.print(passos);
  //        lcd.print("   "); 
        }
      } else {
        if (botaoPressionado) {
          // Botão foi solto
          botaoPressionado = false;
          executando = false;
        }
      }
            delay(10);
            server.handleClient();
    }

    lcd.setCursor(0, 0);
    lcd.print("STEP-MOTOR_STOP ");
    digitalWrite(RGB_B, HIGH);
    digitalWrite(RGB_R, LOW);
    digitalWrite(RGB_G, LOW);
    Serial.println("chave pressionada");

    handleLastResult();

  } else {
    server.send(400, "text/plain", "Parametro nao enviado");
  }
}

void setup() {
    pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
  pinMode(PIN_BTN2, INPUT);
  pinMode(PIN_BTN3, INPUT);
  pinMode(PIN_BTN4, INPUT);
  Serial.begin(115200);

  // Inicia WiFi
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(RGB_B, LOW);
    digitalWrite(RGB_R, HIGH);
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());
  digitalWrite(RGB_R, LOW);
  digitalWrite(RGB_B, HIGH);

  lcd.begin(16, 2); 
  
  server.on("/", handleRoot);
  server.on("/api/move_pos", HTTP_POST, handleMovePos);
  server.on("/api/last_result", HTTP_GET, handleLastResult); // Nova rota

  server.begin();
  Serial.println("Servidor HTTP iniciado!");
}

void loop() {
  server.handleClient();
  
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 5000) {
    lastUpdate = millis();
    
    bool currentBtn2 = (digitalRead(PIN_BTN2) == LOW);
    bool currentBtn3 = (digitalRead(PIN_BTN3) == LOW);
    bool currentBtn4 = (digitalRead(PIN_BTN4) == LOW);
    
    if (currentBtn2 != lastBtn2State || currentBtn3 != lastBtn3State || currentBtn4 != lastBtn4State) {
      Serial.println("Mudança detectada nos botões - atualizando status...");
      lastBtn2State = currentBtn2;
      lastBtn3State = currentBtn3;
      lastBtn4State = currentBtn4;
    }
  }
}