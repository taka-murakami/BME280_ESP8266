#ifndef AMBIENT_H
#define AMBIENT_H

#include <ESP8266WiFi.h>

#define AMBIENT_CHANNEL_KEY_SIZE 40
#define AMBIENT_MAX_RETRY 5

class AMBIENT
{
public:
    
    AMBIENT(const char * channelkey, WiFiClient * c);
    
    bool post(char * key1 = NULL, char * val1 = NULL, char * key2 = NULL, char * val2 = NULL, char * key3 = NULL, char * val3 = NULL);

private:

    WiFiClient * client;
    char channelkey[AMBIENT_CHANNEL_KEY_SIZE];
    
};

#endif // AMBIENT_H

