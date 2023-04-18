#include <ESP8266WiFi.h>
#include <Wire.h>
#include <RTClib.h>

// Configuración WiFi
const char* ssid = "tu_ssid";
const char* password = "tu_contraseña";

// Configuración de pines
const int relay1 = D1;
const int relay2 = D2;
const int relay3 = D3;
const int led1 = D4;
const int led2 = D5;
const int led3 = D6;
const int ledWifi = D7;

RTC_DS3231 rtc;

void setup() {
  Serial.begin(115200);

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(ledWifi, OUTPUT);

  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);

  setup_wifi();
  Wire.begin();
  rtc.begin();
}

void loop() {
  DateTime now = rtc.now();
  control_bombas(now);
  control_led_wifi();

  if (now.hour() == 22 && now.minute() == 0) {
    sleep_until(7, 0);
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void control_bombas(DateTime& now) {
  if (now.hour() == 8 && now.minute() >= 0 && now.minute() < 30) {
    digitalWrite(relay1, LOW);
    digitalWrite(led1, HIGH);
  } else if (now.hour() == 9 && now.minute() >= 30) {
    digitalWrite(relay1, HIGH);
    digitalWrite(led1, LOW);
  }

  if (now.hour() == 12 && now.minute() >= 0 && now.minute() < 60) {
    digitalWrite(relay2, LOW);
    digitalWrite(led2, HIGH);
  } else if (now.hour() == 13) {
    digitalWrite(relay2, HIGH);
    digitalWrite(led2, LOW);
  }

  if (now.hour() == 16 && now.minute() >= 0 && now.minute() < 60) {
    digitalWrite(relay3, LOW);
    digitalWrite(led3, HIGH);
  } else if (now.hour() == 17) {
    digitalWrite(relay3, HIGH);
    digitalWrite(led3, LOW);
  }
}

void control_led_wifi() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledWifi, HIGH);
  } else {
    analogWrite(ledWifi, 128); // 50% de brillo
    delay(50);
    analogWrite(ledWifi, 0);
    delay(50);
  }
}

void sleep_until(int wakeUpHour, int wakeUpMinute) {
  DateTime now = rtc.now();
  DateTime wakeUpTime(now.year(), now.month(), now.day(), wakeUpHour, wakeUpMinute, 0);
  
  if (now.hour() >= wakeUpHour) {
    wakeUpTime = wakeUpTime + TimeSpan(1, 0, 0, 0); // Añade un día si la hora actual es mayor o igual a la hora de despertar
  }

  TimeSpan sleepDuration = wakeUpTime - now;
  uint32_t sleepMillis = sleepDuration.totalseconds() * 1000UL;

  ESP.deepSleep(sleepMillis);
}

