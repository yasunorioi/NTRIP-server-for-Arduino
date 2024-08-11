//#pragma GCC optimize("O3")
//#include <ESP8266WiFi.h>  //Need for ESP8266
#include <WiFi.h>           //Need for ESP32 
//#include<HardwareSerial.h>
#include <M5Atom.h>
#define FASTLED_ESP32_SPI_BUS HSPI
#include "NTRIPServer.h"

const char* ssid     = "ssid";
const char* password = "pass";

//Sample (use rtk2go.com(ntrip softwere test server))
char* host = "rtk2go.com";    //ntrip caster host address 
int httpPort = 2101;          //port 2101 is default port of NTRIP caster
char* mntpnt = "";  //must change this string
char* psw   = "";     //enter password of ntrip caster for subscribing
char* srcSTR   = "";          //source table infomation string reference software.rtcm-ntrip.org/wiki : NTRIP sourcetable contents (https://software.rtcm-ntrip.org/wiki/Sourcetable)
NTRIPServer ntrip_s;

void setup() {
  // put your setup code here, to run once:
  M5.begin(true, false, true);
  pinMode(0,OUTPUT);
  digitalWrite(0,LOW);
  Serial.begin(115200);
  Serial2.begin(115200,SERIAL_8N1,26,32);
  //Serial2.begin(115200, SERIAL_8N1, 17, 16);
  delay(10);
  
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Subscribing MountPoint");
  if (!ntrip_s.subStation(host, httpPort, mntpnt,psw,srcSTR)) {
    delay(15000);
    ESP.restart();
  }
  Serial.println("Subscribing MountPoint is OK1");
}

char ch[512];
int readcount;
void loop() {
  // put your main code here, to run repeatedly:
  if (ntrip_s.connected()) {
    while (Serial2.available()) {
      readcount = 0;
      while (Serial2.available()) {
        ch[readcount] = Serial2.read();
        readcount++;
        if (readcount > 511)break;
      }//buffering
      ntrip_s.write((uint8_t*)ch, readcount);
    }
  }
  else {
    ntrip_s.stop();
    Serial.println("reconnect");
    Serial.println("Subscribing MountPoint");
    if (!ntrip_s.subStation(host, httpPort, mntpnt, psw, srcSTR)) {
      delay(100);
    }
    else {
      Serial.println("Subscribing MountPoint is OK");
      delay(10);
    }

  }
  delay(10);  //server cycle
}
