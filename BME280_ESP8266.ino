#include <ESP8266WiFi.h>
#include <Wire.h>
#include <ESP.h>
#include <Time.h>
#include "BME280.h"
#include "ambient.h"
#include "SensorCorpus.h"
extern "C" {
#include "user_interface.h"
}
extern void ntp_setup();
extern void ntp_get();

#define LED 4
#define SDA 14
#define SCL 13

const char* ssid = "sakura001";
const char* password = "12345678";

const char* key1 = "6fea67c2-9e3d-4a51-bc35-4dfe82f63436";
const char* key2 = "f07e50be-c695-4e09-a054-69caf09c9334";

const char* session = "6fdf50247708ffec73ab58c0d920ac9e70b5bf008e28eeb5b8849f76c2e6507b";

WiFiClient client;
AMBIENT ambient1(key1, &client);
AMBIENT ambient2(key2, &client);

SensorCorpus *sensorcorpus;

BME280 bme280;

int interval = 0;

void setup()
{
  Serial.print("Free heap:");
  Serial.println(ESP.getFreeHeap(),DEC);

    Serial.begin(115200);
    delay(50);
    Serial.println("Start");
    pinMode(LED, OUTPUT);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED, HIGH);
      delay(125);
      digitalWrite(LED, LOW);
      delay(125);
      digitalWrite(LED, HIGH);
      delay(125);
      digitalWrite(LED, LOW);
      delay(125);
      Serial.print(".");
    }
    digitalWrite(LED, HIGH);
    delay(1500);
    digitalWrite(LED, LOW);
    
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    sensorcorpus = new SensorCorpus(session, &client);

    ntp_setup();
    ntp_get();
    
    bme280.begin(SDA, SCL);
    delay(200);
}


void loop()
{
    double temp = 0.0, pressure = 0.0, humid=0.0;
    char tempbuf[8];
    char humidbuf[8];
    char pressurebuf[8];
    char luxbuf[8];

    temp = bme280.readTemperature();
    humid = bme280.readHumidity();
    pressure = bme280.readPressure();

    Serial.print("temp: ");
    Serial.print(temp);
    Serial.print(" DegC,  humid: ");
    Serial.print(humid);
    Serial.print(" %, pressure: ");
    Serial.print(pressure);
    Serial.println(" hPa");

    ftoa(temp, tempbuf);
    ftoa(humid, humidbuf);
    ftoa(pressure, pressurebuf);

    digitalWrite(LED, HIGH);
    ambient1.post("temp", tempbuf, "humid", humidbuf, "pressure", pressurebuf);

    //read tout(analog) port (0-1023)
    uint adc = 0;
    adc = system_adc_read() * 5;
    itoa(adc, luxbuf, 10);
    Serial.println("Illuminance: " + String(adc) + " Lux");
    ambient2.post("temp", tempbuf, "humid", humidbuf, "lux", luxbuf);

    //Send data to SensorCorpus
    sensorcorpus->store("BM280_001_temp", "T", tempbuf);
    sensorcorpus->store("BM280_001_humid", "H", humidbuf);
    sensorcorpus->store("BM280_001_pressure", "PA", pressurebuf);
    sensorcorpus->store("NJL7502_001", "I", luxbuf);
    if (interval == 25) {
      sensorcorpus->send();
    }
    
    digitalWrite(LED, LOW);
    delay(5000);
    interval += 5;
    if (interval == 30) {
      interval = 0;
    }
}

void ftoa(double val, char *buf) {
  itoa((int)val, buf, 10);
  int i = strlen(buf);
  buf[i++] = '.';
  val = (int)(val * 10) % 10;
  itoa(val, &buf[i], 10);
}

