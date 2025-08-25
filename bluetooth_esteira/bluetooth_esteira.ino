#include <LiquidCrystal.h>

#define RGB_R 25
#define RGB_G 26
#define RGB_B 27
#define PIN_BTN 17
#define PIN_BTN2 0
#define PIN_BTN3 4
#define PIN_BTN4 16
#define TRIG_PIN 13
#define ECHO_PIN 35

// Inclui a biblioteca necessária para comunicação via Bluetooth Serial no ESP32
#include "BluetoothSerial.h"

// Verifica se o Bluetooth está habilitado no ESP32
// Se não estiver, interrompe a compilação com uma mensagem de erro
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth não está habilitado! Por favor verifique as configurações.
#endif

// Cria um objeto SerialBT para controlar a comunicação Bluetooth Serial
BluetoothSerial SerialBT;


float distancia;
// Pinos do LCD: RS, E, D4, D5, D6, D7
//iquidCrystal(RS,EN,D4,D5,D6,D7);
LiquidCrystal lcd(32, 33, 23, 22, 21, 18);
bool Automatico = false;
int peca;
bool btnAtual = false;
bool btnOld = true;
bool btnAtual2;
bool btnAtual3;
bool btnAtual4;
bool primeiroLoop = true;
String comando = "";

void clearRow(int row) {
  lcd.setCursor(0, row);
  for (int i = 0; i < 16; i++) {  
    lcd.print(" ");
  }
}


void setup() {

  // Inicia a comunicação serial com o computador (via USB) na velocidade de 115200 bps
  Serial.begin(115200);
  // Inicia a comunicação Bluetooth com o nome "ESP32_TECDES_01"
  SerialBT.begin("ESP32-MIJHO");
  Serial.println("Bluetooth iniciado. Aguarde emparelhamento...");


  lcd.begin(16, 2);  // Inicializa o LCD com 16 colunas e 2 linhas
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIN_BTN, INPUT);
  pinMode(PIN_BTN2, INPUT);
  pinMode(PIN_BTN3, INPUT);
  pinMode(PIN_BTN4, INPUT);
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
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

void switchRecipiente(int peca) {
  switch (peca) {
    case 0:
      digitalWrite(RGB_R, HIGH);
      Serial.println("RGB_R ON");
      digitalWrite(RGB_B, LOW);
      digitalWrite(RGB_G, LOW);
      lcd.setCursor(0, 1);  
      lcd.print("Recipiente A    ");
      break;
    case 1:
      digitalWrite(RGB_G, HIGH);
      digitalWrite(RGB_R, LOW);
      digitalWrite(RGB_B, LOW);
      lcd.setCursor(0, 1);  
      lcd.print("Recipiente B    ");
      break;
    case 2:
      digitalWrite(RGB_B, HIGH);
      Serial.println("RGB_B ON");
      digitalWrite(RGB_R, LOW);
      digitalWrite(RGB_G, LOW);
      lcd.setCursor(0, 1);  
      lcd.print("Recipiente C    ");
      break;
  }
}

void loop() {


  // Se houver dados recebidos pela porta serial USB, encaminha-os para o Bluetooth
  if (Serial.available()) {
    SerialBT.write(Serial.read());  // Transfere o dado diretamente do Serial para o Bluetooth
  }

  // Verifica se há dados recebidos via Bluetooth
  if (SerialBT.available()) {
     comando = "";  // Variável para armazenar o comando recebido
  
  // Enquanto houver dados no buffer Bluetooth, armazena cada caractere na variável `comando`
    while (SerialBT.available()) {
      char c = SerialBT.read();  // Lê um caractere
      comando += c;              // Adiciona ao comando
      delay(2);                  // Pequeno atraso para garantir leitura completa
    }

    comando.trim();  // Remove espaços em branco antes/depois do texto
    Serial.print("Comando recebido: ");
    Serial.println(comando);

  if (comando == "MM" || comando == "M") {
    Automatico = !Automatico;
  }
}

  btnAtual = digitalRead(PIN_BTN);
  btnAtual2 = digitalRead(PIN_BTN2);
  btnAtual3 = digitalRead(PIN_BTN3);
  btnAtual4 = digitalRead(PIN_BTN4);

  if (primeiroLoop) {
    lcd.setCursor(0, 0);
    lcd.print("Seletor_de_Pecas");
    delay(500);
    primeiroLoop = false;
  }

  if (btnAtual == LOW && btnOld == HIGH) {
    delay(50);
    Automatico = !Automatico;
  }
  btnOld = btnAtual;

  if (!Automatico) {
    lcd.setCursor(0, 0);
    lcd.print("Modo Manual     ");
    if (btnAtual2 == LOW || comando == "A" ) {
      peca = 0;
      switchRecipiente(peca);
    }
    if (btnAtual3 == LOW || comando == "B" ) {
      peca = 1;
      switchRecipiente(peca);
    }
    if (btnAtual4 == LOW || comando == "C" ) {
      peca = 2;
      switchRecipiente(peca);
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Modo Automatico ");
    ultrasonico();
    if (distancia < 5) {
      peca = 0;
      switchRecipiente(peca);
    }
    if (distancia > 5 && distancia < 10) {
      peca = 1;
      switchRecipiente(peca);
    }
    if (distancia > 10 && distancia < 15) {
      peca = 2;
      switchRecipiente(peca);
    }
  }
}
