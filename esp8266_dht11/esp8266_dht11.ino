#include "DHT.h"

//khai báo thư viện cho dht 11
#define DHTPIN D3
//what pin we're connected to
#define DHTTYPE   21
DHT dht(DHTPIN, DHTTYPE);
//Khai báo thư viện cho LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
//blynk
#define BLYNK_TEMPLATE_ID "TMPL6yGrSKO3t"
#define BLYNK_TEMPLATE_NAME "esp8266dht11"
#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG
#define USE_NODE_MCU_BOARD

#include "BlynkEdgent.h"
//khai báo biến
float temp, humi, temp_old, humi_old;
const int buttonPin[3] = {3, 14, 2};   // the number of the pushbutton pin
const int ledPin[3] =  {D8, D7, D6};    // the number of the LED pin
int tt[3] = {0, 0, 0};
int buttonState[3] = {0, 0, 0};       // variable for reading the pushbutton status
boolean updateState[3] = {0, 0, 0};
unsigned long t = 0;
unsigned long timesDelay = millis();
unsigned long times = millis();
void setup() {
  pinMode(ledPin[0], OUTPUT);
  pinMode(buttonPin[0], INPUT);
  pinMode(ledPin[1], OUTPUT);
  pinMode(buttonPin[1], INPUT);
  pinMode(ledPin[2], OUTPUT);
  pinMode(buttonPin[2], INPUT);
  dht.begin();
  BlynkEdgent.begin();
  //Serial.begin(9600);
  t = millis();
  // LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("ESP 8266 ");

}

void loop() {
  if ( (unsigned long) (millis() - t) > 1000) {
    get_data_sensor();
    t = millis();
    print_lcd();
  }
  btn_out();
  read_blynk();
}
void get_data_sensor() {
  humi = dht.readHumidity();
  temp = dht.readTemperature();
  t = millis();
  //Serial.print("Nhiet do: ");
  //Serial.println(temp);
  //  Serial.print("Do am: ");
  //  Serial.println(humi);
  if (isnan(humi) || isnan(temp)) {
    //Serial.println("Failed to read from DHT sensor!");
  }
  else {
    temp_old = temp;
    humi_old = humi;
  }
}
void btn_out() {
  for (int i = 0; i < 3; i++) {
    buttonState[i] = digitalRead(buttonPin[i]);
    if (buttonState[i] == LOW ) {
      if (tt[i] == 0) {
        digitalWrite(ledPin[i], !digitalRead(ledPin[i]));
        tt[i] = 1;
        updateState[i] = 1;

      }
    } else {
      tt[i] = 0;
    }
  }
}
void read_blynk() {
  BlynkEdgent.run();
  Blynk.virtualWrite(V10, temp_old);
  Blynk.virtualWrite(V11, humi_old);
  if (updateState[0] == 1) {
    Blynk.virtualWrite(V1, digitalRead(ledPin[0]));
    updateState[0] = 0;
  }
  if (updateState[1] == 1) {
    Blynk.virtualWrite(V2, digitalRead(ledPin[1]));
    updateState[1] = 0;
  }
  if (updateState[2] == 1) {
    Blynk.virtualWrite(V3, digitalRead(ledPin[2]));
    updateState[2] = 0;
  }
}
BLYNK_CONNECTED() {
  // Request Blynk server to re-send latest values for all pins
  Blynk.syncAll();
}
BLYNK_WRITE(V1) {
  int p = param.asInt();
  digitalWrite(ledPin[0], p);
}
BLYNK_WRITE(V2) {
  int p = param.asInt();
  digitalWrite(ledPin[1], p);
}
BLYNK_WRITE(V3) {
  int p = param.asInt();
  digitalWrite(ledPin[2], p);
}
void print_lcd() {
  //Hiển thị lên LCD nhiệt độ và độ ẩm
  lcd.setCursor(0, 0);
  lcd.print("Temp  : ");
  lcd.print(temp_old);
  lcd.print((char)223);
  lcd.print("C ");
  lcd.setCursor(0, 1);
  lcd.print("Humid : ");
  lcd.print(humi_old);
  lcd.print("%H   ");
}
