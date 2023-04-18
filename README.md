# CONTROL_DE_BOMBA_RTC
CONTROL DE RIEGO CON RELOJ EN TIEMPO REAL Y ADMINISTRACIÓN DE ESTADOS RCT,ESP8266 DEV, SIM800L

#CODIGO PRINCIPAL 
Aquí está la explicación detallada del código:

Inclusión de librerías:

      #include <SoftwareSerial.h>
      #include <ESP8266WiFi.h>
      #include <PubSubClient.h>

Estas son las librerías necesarias para el funcionamiento del proyecto:

SoftwareSerial.h: permite utilizar pines digitales del ESP8266 para la comunicación serial con el módulo SIM800L.
ESP8266WiFi.h: proporciona las funciones necesarias para la conexión WiFi y manejo de redes en el ESP8266.
PubSubClient.h: facilita la comunicación con servidores MQTT y la publicación y suscripción de mensajes.


