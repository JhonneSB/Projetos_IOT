#include <LiquidCrystal.h>

#define RGB_R 25
#define RGB_G 26
#define RGB_B 27
#define PIN_BTN2 0
#define PIN_BTN3 4

#include "BluetoothSerial.h"
#include "DHT.h"

#define DHTPIN 15
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
BluetoothSerial SerialBT;
LiquidCrystal lcd(32, 33, 23, 22, 21, 18);

int peca;
bool Automatico = false;
bool btnAtual = false;
bool btnOld = true;
bool btnAtual2;
bool btnAtual3;
bool primeiroLoop = false;
int contador = 1;
String AutoMan = "Man";

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32-MIJHO");
  Serial.println("Bluetooth iniciado. Aguarde emparelhamento...");
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
  lcd.begin(16, 2);
  lcd.print("Iniciando...");

  dht.begin();
  delay(1000);
  lcd.clear();
}

void switchRecipiente(int peca) {
  switch (peca) {
    case 0:
      digitalWrite(RGB_R, HIGH);
      digitalWrite(RGB_B, LOW);
      digitalWrite(RGB_G, LOW);
      lcd.setCursor(0, 1);
      lcd.print("Aletas:30o     ");
      atualizarDisplay();
      break;
    case 1:
      digitalWrite(RGB_G, HIGH);
      digitalWrite(RGB_R, LOW);
      digitalWrite(RGB_B, LOW);
      lcd.setCursor(0, 1);
      lcd.print("Aletas:60o     ");
      atualizarDisplay();
      break;
    case 2:
      digitalWrite(RGB_B, HIGH);
      digitalWrite(RGB_R, LOW);
      digitalWrite(RGB_G, LOW);
      lcd.setCursor(0, 1);
      lcd.print("Aletas:90o     ");
      atualizarDisplay();
      break;
  }
}

void botao() {
  btnAtual2 = digitalRead(PIN_BTN3);
  if (btnAtual2 == LOW) {
    delay(100);  // Debounce
    if (contador == 3) {
      contador = 1;
    } else {
      contador += 1;
    }
    atualizarDisplay();
  }
}

void isAutoMan() {
  if (Automatico) {
    AutoMan = "Aut";
  } else {
    AutoMan = "Man";
  }
}

void atualizarDisplay() {
  lcd.setCursor(0, 0);
  lcd.print(AutoMan);
  lcd.print(" ");

  lcd.setCursor(4, 0);
  float temperatura = dht.readTemperature();
  if (!isnan(temperatura)) {
    lcd.print("Temp: ");
    lcd.print(temperatura, 1);
    lcd.print("C  ");
  }
}

void loop() {
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();
  if (primeiroLoop == false) {
    lcd.setCursor(0, 0);
    lcd.print("Controle_Estufa ");
    delay(2500);
    primeiroLoop = true;
  }


if (isnan(umidade) || isnan(temperatura)) {
  Serial.println("Falha ao ler sensor!");
  delay(2000);
  return;
}

String dadosBT = "*T" + String(temperatura, 1) + "*";
SerialBT.print(dadosBT);
//Serial.println(dadosBT);
String dadosBTU = "*H" + String(umidade, 1) + "*";
SerialBT.print(dadosBTU);

btnAtual = digitalRead(PIN_BTN2);
if (btnAtual == LOW && btnOld == HIGH) {
  delay(50);
  Automatico = !Automatico;
  isAutoMan();
  atualizarDisplay();
}
btnOld = btnAtual;

if (!Automatico) {
  // Modo manual
  botao();
  if (contador == 1) {
    peca = 0;
    switchRecipiente(peca);
  } else if (contador == 2) {
    peca = 1;
    switchRecipiente(peca);
  } else if (contador == 3) {
    peca = 2;
    switchRecipiente(peca);
  }
} else {
  // Modo automático
  if (temperatura >= 24.1 && temperatura < 24.3) {
    peca = 0;
    switchRecipiente(peca);
  } else if (temperatura >= 24.3 && temperatura < 24.5) {
    peca = 1;
    switchRecipiente(peca);
  } else if (temperatura >= 24.5) {
    peca = 2;
    switchRecipiente(peca);
  }
}

if(temperatura>=30){
    lcd.setCursor(11, 1);
    lcd.print("ALTA");
} else{
    lcd.setCursor(11, 1);
    lcd.print("    ");
}
atualizarDisplay();
}