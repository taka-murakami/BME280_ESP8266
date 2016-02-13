#include "ambient.h"

#define _DEBUG 1

#if _DEBUG
#define DBG(...) { Serial.print(__VA_ARGS__); }
#define ERR(...) { Serial.print(__VA_ARGS__); }
#else
#define DBG(...)
#define ERR(...)
#endif /* _DBG */

const char* AMBIENT_HOST = "smaene.info";
int AMBIENT_PORT = 80;

AMBIENT::AMBIENT(const char * channelkey, WiFiClient * c) {
    if (sizeof(channelkey) > AMBIENT_CHANNEL_KEY_SIZE) {
        ERR("channelkey length > AMBIENT_CHANNEL_KEY_SIZE");
    }
    strcpy(this->channelkey, channelkey);
    
    if(NULL == c) {
        ERR("Socket Pointer is NULL, open a socket.");
    }
    this->client = c;
}

bool
AMBIENT::post(char * key1, char * value1, char * key2, char * value2, char * key3, char * value3) {

    if (key1 == NULL) {
        ERR("Need 1st key-value pair\r\n");
        return false;
    }

    int retry;
    for (retry = 0; retry < AMBIENT_MAX_RETRY; retry++) {
        int ret = this->client->connect(AMBIENT_HOST, AMBIENT_PORT);
        if(ret) {
            break ;
        }
    }
    if(retry == AMBIENT_MAX_RETRY) {
        ERR("Could not connect socket to host\r\n");
        return false;
    }

    char str[256] = {0};
    char header[64] = {0};
    char host[32] = {0};
    char contentLen[32] = {0};
    const char *contentType = "Content-Type: application/json\r\n\r\n";
    char valueData[128] = {0};
    char inChar;
    
    strcat(valueData, "{\"");
    strcat(valueData, key1);
    strcat(valueData, "\": ");
    strcat(valueData, value1);

    if (key2) {
        strcat(valueData, ", \"");
        strcat(valueData, key2);
        strcat(valueData, "\": ");
        strcat(valueData, value2);
    }

    if (key3) {
        strcat(valueData, ", \"");
        strcat(valueData, key3);
        strcat(valueData, "\": ");
        strcat(valueData, value3);
    }

    strcat(valueData, "}\r\n");

    sprintf(header, "POST /api/v1/%s HTTP/1.1\r\n", this->channelkey);
    sprintf(host, "Host: %s\r\n", AMBIENT_HOST);
    sprintf(contentLen, "Content-Length: %d\r\n", strlen(valueData));
    sprintf(str, "%s%s%s%s%s", header, host, contentLen, contentType, valueData);
    
    DBG("sending:\n");
    DBG(str);
    
    int ret;
    ret = this->client->print(str);
    if (ret == 0) {
        ERR("send failed\n");
        return false;
    }

    while (this->client->available()) {
      inChar = this->client->read();
#if _DEBUG
      Serial.write(inChar);
#endif
    }

    this->client->stop();
    
    return true;
}

