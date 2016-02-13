#ifndef SENSORCORPUS_H
#define SENSORCORPUS_H

#include <ESP8266WiFi.h>

class SensorCorpus
{
public:
    
    SensorCorpus(const char * session, WiFiClient * c);
    bool store(char *sensorid, char * key, char * val);
    bool send();

private:

    static const int SessionKeySize = 80;
    static const int MaxRetry = 5;

    WiFiClient * client;
    
    char session[SessionKeySize];
    
    const char* Host = "data.sensorcorpus.net";
    static const int Port = 80;
    const char *contentType = "Content-Type: application/x-www-form-urlencoded\r\n\r\n";

    String data_buf;    
};

#endif // SENSORCORPUS_H
