#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

// Configuración para SIM800L
#define RX 5
#define TX 4
SoftwareSerial sim800l(RX, TX);

// Configuración de APN
const char apn[] = "movistar.pe";

// Configuración WiFi
const char* ssid = "tu_ssid";
const char* password = "tu_contraseña";

// Configuración MQTT
const char* mqtt_server = "34.238.235.88";
const int mqtt_port = 1883;
const char* mqtt_user = "usuario"; // Si es necesario
const char* mqtt_pass = "contraseña"; // Si es necesario
const char* mqtt_topic = "test";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  sim800l.begin(9600);
  delay(3000);
  Serial.println("Iniciando...");

  setup_sim800l();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  // Actualizar la hora del RTC con la hora de la compilación
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // Inicializar módulo RTC DS3231
  if (!rtc.begin()) {
    Serial.println("No se encontró el módulo RTC DS3231!");
    while (1);
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    send_gprs_message();
  } else {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }
}

// obtener la hora actual del módulo RTC y formatearla en un string

String get_current_time() {
  DateTime now = rtc.now();
  char time_str[20];
  sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  return String(time_str);
}


void setup_sim800l() {
  Serial.println("Configurando SIM800L...");
  sim800l.println("AT+CGATT=1");
  delay(2000);
  sim800l.print("AT+CIPSHUT\r");
  delay(2000);
  sim800l.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  delay(2000);
  sim800l.print("AT+SAPBR=3,1,\"APN\",\"");
  sim800l.print(apn);
  sim800l.println("\"");
  delay(2000);
  sim800l.println("AT+SAPBR=1,1");
  delay(3000);
  sim800l.println("AT+SAPBR=2,1");
  delay(3000);
  sim800l.println("AT+CIPSTATUS");
  delay(2000);
}

void send_gprs_message() {
  sim800l.println("AT+CIPSTART=\"TCP\",\"" + String(mqtt_server) + "\",1883");
  delay(2000);
  String message = "Hola desde GPRS - " + get_current_time();
  String mqtt_payload = "10" + String(message.length() + 16, HEX) + "00044D5154540400003C000974657374";
  mqtt_payload += "0B" + String(message.length(), HEX) + message;
  sim800l.println("AT+CIPSEND=" + String(mqtt_payload.length() / 2));
  delay(2000);
  sim800l.print(mqtt_payload);
  delay(2000);
  sim800l.println("AT+CIPCLOSE");
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

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_pass)) {
      Serial.println("Conectado");
      send_wifi_message();
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5 segundos");
      delay(5000);
    }
  }
}

void send_wifi_message() {
  String message = "Hola desde WiFi - " + get_current_time();
  client.publish(mqtt_topic, message.c_str());
}
