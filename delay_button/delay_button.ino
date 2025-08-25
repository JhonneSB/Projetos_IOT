
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

  bool btnAtual = digitalRead(PIN_BTN);

  if(btnAtual == 0 && btnOld==1){
    delay(20);
    if(ativar==true){
      ativar=false;
    }else{
      ativar=true;
    }
  }
  btnOld =btnAtual;


if(ativar == true){
digitalWrite(RGB_B, LOW);
Serial.println("RGB_B OFF");
digitalWrite(LED_R, HIGH);
Serial.println("LED_R ON");
digitalWrite(LED_G, LOW);
Serial.println("LED_G OFF");
digitalWrite(LED_B, LOW);
Serial.println("LED_B OFF");
delay(500);
///////////////
digitalWrite(LED_R, LOW);
Serial.println("LED_R OFF");
digitalWrite(LED_G, HIGH);
Serial.println("LED_G ON");
digitalWrite(LED_B, LOW);
Serial.println("LED_B OFF");
delay(500);
//////////////
digitalWrite(LED_R, LOW);
Serial.println("LED_R OFF");
digitalWrite(LED_G, LOW);
Serial.println("LED_G OFF");
digitalWrite(LED_B, HIGH);
Serial.println("LED_B ON");
delay(500);
////////////// V RED RGB
digitalWrite(LED_B, LOW);
Serial.println("LED_B OFF");
digitalWrite(RGB_R, HIGH);
Serial.println("RGB_R ON");
digitalWrite(RGB_G, LOW);
Serial.println("RGB_G OFF");
digitalWrite(RGB_B, LOW);
Serial.println("RGB_B OFF");
delay(500);
////////////// V GREEB RGB
digitalWrite(RGB_R, LOW);
Serial.println("RGB_R OFF");
digitalWrite(RGB_G, HIGH);
Serial.println("RGB_G ON");
digitalWrite(RGB_B, LOW);
Serial.println("RGB_B OFF");
delay(500);
////////////// V BLUE RGB
digitalWrite(RGB_R, LOW);
Serial.println("RGB_R OFF");
digitalWrite(RGB_G, LOW);
Serial.println("RGB_G OFF");
digitalWrite(RGB_B, HIGH);
Serial.println("RGB_B ON");
delay(500);
////////////// 
} else {
  digitalWrite(RGB_R, LOW);
Serial.println("RGB_R OFF");
digitalWrite(RGB_G, LOW);
Serial.println("RGB_G OFF");
digitalWrite(RGB_B, LOW);
Serial.println("RGB_B OFF");
digitalWrite(LED_R, LOW);
Serial.println("LED_R OFF");
digitalWrite(LED_G, LOW);
Serial.println("LED_G OFF");
digitalWrite(LED_B, LOW);
Serial.println("LED_B OFF");
} 
}

}