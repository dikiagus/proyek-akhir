#include <EEPROM.h>
#include "GravityTDS.h"

#include <Wire.h>  // include library
#include<LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// deklarasi pin dan variabel
#define TdsSensorPin A0
GravityTDS gravityTds;

const int pH_pin = A1;
unsigned long int avg_pH;
int buf[10];
int tdsValue = 0;

// deklarasi pin button
#define switch_ph 2
#define switch_tds 3

// deklarasi pin vcc
#define vcc_ph 4
#define vcc_tds 5

// deklarasi untuk digital read
int dr_ph, dr_tds;

// membuat karakter panah bawah
byte customChar[] = {
  B00000,
  B00000,
  B00100,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100
};

void setup()
{
  Serial.begin(115200);

  pinMode(switch_ph, INPUT_PULLUP);
  pinMode(switch_tds, INPUT_PULLUP);
  pinMode(vcc_ph, OUTPUT);
  pinMode(vcc_tds, OUTPUT);

  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization

  lcd.init();
  // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();

}

void pH()
{

  digitalWrite(vcc_tds, LOW);
  digitalWrite(vcc_ph, HIGH);

  lcd.setCursor(0, 0);
  lcd.print("pH = ");
  lcd.setCursor(0, 1);
  lcd.print("Status = ");

  avg_pH = 0;
  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(pH_pin);
    avg_pH += buf[i];
    delay(100);
  }

  avg_pH = avg_pH / 10; // nilai rata-rata analog pH

  float pHVol = (float)avg_pH * 5.0 / 1024;
  float phValue = (-10.056 * pHVol) + 32.594; // kalibrasi pH

  lcd.setCursor(5, 0);
  lcd.print(phValue);

  if (phValue < 6.5) {
    lcd.setCursor(9, 0);
    lcd.print(" ");
    lcd.setCursor(9, 1);
    lcd.print("Asam");
  }

  else if (phValue > 8.5) {
    lcd.setCursor(9, 0);
    lcd.print(" ");
    lcd.setCursor(9, 1);
    lcd.print("Basa");
  }

  else {
    lcd.setCursor(9, 0);
    lcd.print(" ");
    lcd.setCursor(9, 1);
    lcd.print("Aman");
  }

  Serial.print(" Volt pH = "); Serial.print(pHVol);
  Serial.print(" PH Value = "); Serial.println(phValue);
  Serial.println(" ");
  delay(1000);

}

void TDS()
{

  digitalWrite(vcc_ph, LOW);
  digitalWrite(vcc_tds, HIGH);

  lcd.setCursor(9, 0);
  lcd.print(" ");
  lcd.setCursor(0, 0);
  lcd.print("TDS = ");
  lcd.setCursor(0, 1);
  lcd.print("Status = ");
  lcd.setCursor(10, 0);
  lcd.print("ppm");

  gravityTds.update();  //sample and calculate
  tdsValue = gravityTds.getTdsValue();  // then get the value

  int kalibrasi_TDS = (0.8065 * tdsValue) + 44.355; // kalibrasi TDS

  lcd.setCursor(8, 0);
  lcd.print(" ");
  lcd.setCursor(6, 0);
  lcd.print(kalibrasi_TDS);

  if (kalibrasi_TDS <= 500) {
    lcd.setCursor(13, 1);
    lcd.print(" ");
    lcd.setCursor(14, 1);
    lcd.print(" ");
    lcd.setCursor(9, 1);
    lcd.print("Aman");
  }

  else {
    lcd.setCursor(9, 1);
    lcd.print("Bahaya");
  }

  Serial.print(" TDS DFRobot = "); Serial.print(tdsValue);
  Serial.print(" TDS Value = "); Serial.print(kalibrasi_TDS);
  Serial.print(" ppm");
  Serial.println(" ");
  Serial.println(" ");
  delay(1000);

}

void loop()
{

  digitalWrite(vcc_ph, LOW);
  digitalWrite(vcc_tds, LOW);

  dr_ph = digitalRead(switch_ph);
  dr_tds = digitalRead(switch_tds);
  delay(10);

  if (dr_ph == 1 && dr_tds == 1) {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Sensor");
    lcd.setCursor(2, 1);
    lcd.print("Kualitas Air");
    delay(3000);
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("pH");
    lcd.setCursor(11, 0);
    lcd.print("TDS");
    lcd.createChar(0, customChar);
    lcd.home();
    lcd.setCursor(1, 1);
    lcd.write(0);
    lcd.createChar(0, customChar);
    lcd.home();
    lcd.setCursor(12, 1);
    lcd.write(0);
    delay(4000);
    lcd.clear();
  }

  if (dr_ph == 0 && dr_tds == 1) {
    pH();
  }

  if (dr_tds == 0 && dr_ph == 1) {
    TDS();
  }

  if (dr_ph == 0 && dr_tds == 0) {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Pilih");
    lcd.setCursor(5, 1);
    lcd.print("1 Menu");
    delay(1000);
  }

}
