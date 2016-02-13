#include <Time.h>
#include "SensorCorpus.h"

#define _DEBUG 1

#if _DEBUG
#define DBG(...) { Serial.print(__VA_ARGS__); }
#define ERR(...) { Serial.print("SensorCorpus:");Serial.print(__VA_ARGS__); }
#else
#define DBG(...)
#define ERR(...)
#endif /* _DBG */


SensorCorpus::SensorCorpus(const char * sid, WiFiClient * c) {
    if (sizeof(sid) > SessionKeySize) {
        ERR("session length too long");
    }
    strcpy(this->session, sid);
    DBG("session id=");
    DBG(session); DBG("\n");
    
    if(NULL == c) {
        ERR("Socket Pointer is NULL, open a socket.");
    }
    this->client = c;
    
    data_buf = "";
}

bool
SensorCorpus::store(char *sensorid, char * key, char * value) {

//    DBG("Free heap:");
//    DBG(ESP.getFreeHeap(),DEC);
//    DBG("\n");

    if (data_buf.length() == 0) {
        data_buf = "session=";
        data_buf += session;
        data_buf += "&csv=";
    } else {
        data_buf += "%0A";
    }

    char now[32];
    sprintf(now, ",%d-%02d-%02dT%02d:%02d:%02d%%2B09:00,"
       , year(), month(), day(), hour(), minute(), second());

    data_buf += sensorid;
    data_buf += now;
    data_buf += key;
    data_buf += ",";
    data_buf += value;

//    DBG(data_buf);
//    DBG("\n");
    DBG("Free heap:");
    DBG(ESP.getFreeHeap(),DEC);
    DBG("\n");

    return true;
}

bool
SensorCorpus::send() {
  
    DBG("Free heap:");
    DBG(ESP.getFreeHeap(),DEC);
    DBG("\n");

    int retry;
    for (retry = 0; retry < MaxRetry; retry++) {
        int ret = this->client->connect(this->Host, this->Port);
        if(ret) {
            break ;
        }
    }
    if(retry == MaxRetry) {
        ERR("Could not connect socket to host\r\n");
        return false;
    }

    char host[32] = {0};
    char contentLen[32] = {0};
    String header = "POST /store/ HTTP/1.1\r\n";

    sprintf(host, "Host: %s\r\n", this->Host);
    sprintf(contentLen, "Content-Length: %d\r\n", data_buf.length());

    header += host;
    header += contentLen;
    header += contentType;
 
    DBG("sending:\n");
    DBG(header);
    DBG(data_buf);
    DBG("\n");

    int ret;
    ret = this->client->print(header);
    ret = this->client->print(data_buf);
//    ret = this->client->print("\r\n\r\n");
/*
    if (ret == 0) {
        ERR("send failed\n");
        return false;
    }
*/
    this->client->flush();
    
    DBG("reading response\n");
    if (this->client->available() == false) {
      DBG("waiting\n");
      delay(100);
    }
        if (this->client->available() == false) {
      DBG("waiting\n");
      delay(100);
    }
    if (this->client->available() == false) {
      DBG("waiting\n");
      delay(100);
    }

    while (this->client->available()) {
        String line = this->client->readStringUntil('\r');
#if _DEBUG
        Serial.print(line);
#endif
    }

    this->client->stop();

    data_buf = "";
    DBG("Free heap:");
    DBG(ESP.getFreeHeap(),DEC);
    DBG("\n");
    return true;
}

