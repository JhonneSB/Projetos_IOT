#define LED_R 14
#define LED_G 33
#define LED_B 32
#define RGB_R 25
#define RGB_G 26
#define RGB_B 27
#define PIN_BTN 0


bool ativar = false;
bool btnAtual = false;
bool btnOld = true;
unsigned long tempAnterior = 0;
const long INTERVALO = 500;
int etapa = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);
}

void loop() {
  btnAtual = digitalRead(PIN_BTN);
  
  if (btnAtual == LOW && btnOld == HIGH) {
    delay(50); 
    ativar = !ativar;
    etapa = 0;
    desligaTudo();
  }
  btnOld = btnAtual;
  
  if (ativar) {
    cicloLEDs();
  } else {
    desligaTudo();
  }
}

void desligaTudo() {
  digitalWrite(RGB_R, LOW);
  digitalWrite(RGB_G, LOW);
  digitalWrite(RGB_B, LOW);
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}

void cicloLEDs() {
  unsigned long tempAtual = millis();
  
  if (tempAtual - tempAnterior >= INTERVALO) {
    tempAnterior = tempAtual;
    desligaTudo();
    switch(etapa) {
      case 0:
        digitalWrite(LED_R, HIGH);
        Serial.println("LED_R ON");
        break;
      case 1:
        digitalWrite(LED_G, HIGH);
        Serial.println("LED_G ON");
        break;
      case 2:
        digitalWrite(LED_B, HIGH);
        Serial.println("LED_B ON");
        break;
      case 3:
        digitalWrite(RGB_R, HIGH);
        Serial.println("RGB_R ON");
        break;
      case 4:
        digitalWrite(RGB_G, HIGH);
        Serial.println("RGB_G ON");
        break;
      case 5:
        digitalWrite(RGB_B, HIGH);
        Serial.println("RGB_B ON");
        break;
    }
    
    etapa = (etapa + 1) % 6;
  }
}