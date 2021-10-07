//mq sensörünü Analog için A0 pinine bağla (dijital için 13 pinide bağla)
//Dht11 için dijital  7 pinine bağla
//buzzer için dijital 12 pinine bağla
//flame sesörünün Dijitl için 11 pinine bağla (analog için A2 pinine bağla)
//MPU6050 sensörünün INT bacağı ardunio nun dijital 3 pinine bağlanmalı
//buzzer dijital  pin12 ye bağlanır.
//yukarıda parantez içinde olan kısımlar şuan deaktif (kapalı) buyuzden normal yazan şeklinde bağlama işlemini yapın.
// ntc a2 pinine bağlanır
// voltaj sensörü a1 pinine bağlanır
#include <Adafruit_Sensor.h>

#include <DHT.h>

#include <DHT_U.h>

#include <MPU6050_tockn.h>

#include <Wire.h>

#include <math.h>

#include <stdarg.h>

#define NTC_input A3 
#define voltageSensor A1
#define MQ2pin A0
#define MQ2pin_digital 13
#define DHTPIN 7
#define DHTTYPE DHT11
#define buzzerPin 5
#define flamePin_digital 11
#define SERIAL_PRINTF_MAX_BUFF 256

void serialPrintf(const char * fmt, ...);
void serialPrintlnf(const char * fmt, ...);

float A = 1.009249522e-03, B = 2.378405444e-04, C = 2.019202697e-07; // Stein-Hart denkleminde bulunan sabit değerleri tanımladık.
float voltInput = 0.0;
float voltValue = 0.0;
const int OUTPUT_PIN = 2;
int NTC_deger; //NTC değerini integer cinsinden tanımladık.
float Vout; // Vout değişkeni
float NTC_direnc, NTC_direnc_ln, Sicaklik; // Stein-Hart denkleminde kullanılacak olan değişkenler

MPU6050 mpu6050(Wire);
DHT dht(DHTPIN, DHTTYPE);
int flameSensorReading;
const int flameSensorMin = 0,
  flameSensorMax = 1024;
enum ids {
  
//0
  angleX,
//1
  angleY,
//2
  angleZ,
//3
  flameDigitalVar,
//4  
  flameAnalogVar,
//5  
  voltageAnalogINT,
//6  
  ntcAnalogINT,
//7  
  DHTDigitalHum,
//8  
  DHTDigitalTemp,
//9  
  dhdtError,
//10  
  mqDigitalInt,
//11  
  mqDigitalVar,
//12  
  mqAnalogInt,
//13  
  mqAnalogVar,
//14  
  carWarning
};

unsigned long eskiZaman = 0;
unsigned long yeniZaman;

const int esikDegerMQ = 170;
float sensorValueAnalog;
int sensorValueDigital;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(MQ2pin_digital, INPUT);
  pinMode(voltageSensor,INPUT);
  digitalWrite(OUTPUT_PIN, HIGH);
  pinMode(OUTPUT_PIN, OUTPUT);
  Serial.begin(9600);
  dht.begin();
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(false);
  buzzerdoubleBang();
}

void serialPrintf(const char * fmt, ...) {
  /* Buffer for storing the formatted data */
  char buff[SERIAL_PRINTF_MAX_BUFF];
  /* pointer to the variable arguments list */
  va_list pargs;
  /* Initialise pargs to point to the first optional argument */
  va_start(pargs, fmt);
  /* create the formatted data and store in buff */
  vsnprintf(buff, SERIAL_PRINTF_MAX_BUFF, fmt, pargs);
  va_end(pargs);
  Serial.print(buff);
}
void serialPrintlnf(const char * fmt, ...) {
  /* Buffer for storing the formatted data */
  char buff[SERIAL_PRINTF_MAX_BUFF];
  /* pointer to the variable arguments list */
  va_list pargs;
  /* Initialise pargs to point to the first optional argument */
  va_start(pargs, fmt);
  /* create the formatted data and store in buff */
  vsnprintf(buff, SERIAL_PRINTF_MAX_BUFF, fmt, pargs);
  va_end(pargs);
  Serial.println(buff);
}

void mpuDigital() {
  serialPrintf("%d ", angleX);
  Serial.println(mpu6050.getAngleX());
  serialPrintf("%d ", angleY);
  Serial.println(mpu6050.getAngleY());
  serialPrintf("%d ", angleZ);
  Serial.println(mpu6050.getAngleZ());
}
void flameDigital() {
  if (flameSensorReading == HIGH) {
    serialPrintlnf("%d No Fire", flameDigitalVar);
  } else {
    serialPrintlnf("%d **Fire**", flameDigitalVar);
  }
}
void flameAnalog() {
  // read the sensor on analog A2:
  int flameSensorReading = analogRead(A2);
  // map the sensor range (four options):
  // ex: 'long int map(long int, long int, long int, long int, long int)'
  int range = map(flameSensorReading, flameSensorMin, flameSensorMax, 0, 3);

  // range value:
  switch (range) {
  case 0: // A fire closer than 1.5 feet away.
    serialPrintlnf("%d Close Fire", flameAnalogVar);
    break;
  case 1: // A fire between 1-3 feet away.
    serialPrintlnf("%d Distant Fire", flameAnalogVar);
    break;
  case 2: // No fire detected.
    serialPrintlnf("%d No Fire", flameAnalogVar);
    break;
  default:
    break;
  }
}


void dhtDigital(float h, float t, float f) {
  if (isnan(h) || isnan(t) || isnan(f)) {
    serialPrintlnf("%d Failed to read from DHT sensor!",dhdtError);
    return;
  }
  serialPrintf("%d ", DHTDigitalHum);
  Serial.println(h);
  serialPrintf("%d ", DHTDigitalTemp);
  Serial.println(t);
}
void mqAnalog(int sensorValueAnalog) {
  serialPrintf("%d ", mqAnalogInt);
  Serial.println(sensorValueAnalog);

  if (sensorValueAnalog > esikDegerMQ) {
    serialPrintlnf("%d Smoke detected!", mqAnalogVar);
  } else {
    serialPrintlnf("%d No gas detected!", mqAnalogVar);
  }
}
void mqDigital(int sensorValueDigital) {

  serialPrintf("%d ", mqDigitalInt);
  Serial.println(sensorValueDigital);
  if (sensorValueDigital == HIGH) {

    serialPrintlnf("%d Smoke detected!", mqDigitalVar);
  } else {
    serialPrintlnf("%d No gas detected!", mqDigitalVar);
  }
}
void tersKontrol() {
  int x = mpu6050.getAngleX();
  int y = mpu6050.getAngleY();
  int alt = 155;
  int ust = -155;

  if (x >= alt || x <= ust) {
    if (y >= alt || y <= ust) {
      serialPrintlnf("%d Araç ters döndü!",carWarning);
      buzzerdoubleBang();
    }else{
      serialPrintlnf("%d Normal",carWarning);
    }
  }else{
      serialPrintlnf("%d Normal",carWarning);
    }
}
void buzzerdoubleBang() {
  for (int i = 0; i <= 2; i++) {
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);
    delay(500);
  }
}
void ntcAnalog(){
 Vout = ( (NTC_deger * 5.0) / 1023.0 ); //Vout hesabı (Voltaj Bölücü Formülü)
 NTC_direnc = ( ( 5 * ( 10.0 / Vout ) ) - 10 ); //KiloOhm Cinsinden direnc değerinin hesabı
 NTC_direnc = NTC_direnc * 1000 ; // Ohm cinsinden direnç değeri
 NTC_direnc_ln = log(NTC_direnc);
 Sicaklik = ( 1 / ( A + ( B * NTC_direnc_ln ) + ( C * NTC_direnc_ln * NTC_direnc_ln * NTC_direnc_ln ) ) ); //Sıcaklık değeri hesabı
 Sicaklik = Sicaklik - 273.15; //Sicaklik değerinin Celcius'a çevirimi 
   serialPrintf("%d ", ntcAnalogINT);
  Serial.println(Sicaklik);
 }

void voltageAnalog(float voltInput){ 
  voltValue = ((voltInput/190)*4);
  serialPrintf("%d ", voltageAnalogINT);
  Serial.println(voltValue);
}

void loop() {
  mpu6050.update();
  flameSensorReading = digitalRead(flamePin_digital);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(false);
  sensorValueAnalog = analogRead(MQ2pin);
  //sensorValueDigital = digitalRead(MQ2pin_digital);
  voltInput = analogRead(voltageSensor);
  yeniZaman = millis();
  NTC_deger = analogRead(NTC_input);
  
  if (yeniZaman - eskiZaman > 5000) {
    mpuDigital();
    flameDigital();
    flameAnalog();
    voltageAnalog(voltInput);
    ntcAnalog();
    dhtDigital(h, t, f);
    mqAnalog(sensorValueAnalog);
    //mqDigital(sensorValueDigital);
    tersKontrol();
    eskiZaman = yeniZaman;
  }

  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    Serial.println(data);
    if(data == "r") digitalWrite(OUTPUT_PIN, LOW);
  }
}
