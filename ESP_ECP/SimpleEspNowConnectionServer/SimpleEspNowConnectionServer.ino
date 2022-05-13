#include "SimpleEspNowConnection.h"
#include "DHT.h"
#define Type DHT11

SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::SERVER);

String inputString;
String clientAddress;

int sensePin = 2;
DHT HT(sensePin, Type);

typedef struct dhtStruct {
  char type;
  float temp;
  float humidity;
} dhtStruct;


bool sendStruct() {
  dhtStruct dhtData;

  dhtData.type = '#';
  dhtData.temp = HT.readTemperature();
  dhtData.humidity = HT.readHumidity();
  sprintf ("Greetings from %s", simpleEspConnection.myAddress.c_str());
  
  return (simpleEspConnection.sendMessage((uint8_t *)&dhtData, sizeof(dhtData)));
}

void OnSendError(uint8_t* ad)
{
  Serial.println("SENDING TO '"+simpleEspConnection.macToStr(ad)+"' WAS NOT POSSIBLE!");
}

void OnMessage(uint8_t* ad, const uint8_t* message, size_t len)
{
  if((char)message[0] == '#')
  {
    dhtStruct dhtData;
  
    memcpy(&dhtData, message, len);   
    Serial.printf("%f,%f,Main Office\n", dhtData.temp, dhtData.humidity);
  }
  else
    Serial.printf("MESSAGE:[%d]%s from %s\n", len, (char *)message, simpleEspConnection.macToStr(ad).c_str());
}

void OnPaired(uint8_t *ga, String ad)
{
  Serial.println("EspNowConnection : Client '"+ad+"' paired! ");
  simpleEspConnection.endPairing();
  
  clientAddress = ad;
}

void OnConnected(uint8_t *ga, String ad)
{
  Serial.println("EspNowConnection : Client '"+ad+"' connected! ");

  simpleEspConnection.sendMessage((uint8_t *)"Message at OnConnected from Server", 34, ad);
}

void setup() 
{
  Serial.begin(115200);
  Serial.println();
  //clientAddress = "CC50E35B56B1"; // Test if you know the client

  simpleEspConnection.begin();
//   simpleEspConnection.setPairingBlinkPort(2);
  simpleEspConnection.onMessage(&OnMessage);  
  simpleEspConnection.onPaired(&OnPaired);  
  simpleEspConnection.onSendError(&OnSendError);
  simpleEspConnection.onConnected(&OnConnected);  

  Serial.println(WiFi.macAddress());    
}

void loop() 
{
  // needed to manage the communication in the background!
  simpleEspConnection.loop();


  if(!simpleEspConnection.startPairing(30))
  {
    simpleEspConnection.startPairing(30);
  }
  
  while (Serial.available()) 
  {
    char inChar = (char)Serial.read();
    if (inChar == '\n') 
    {
      Serial.println(inputString);

      if(inputString == "startpair")
      {
        simpleEspConnection.startPairing(30);
      }
    }
  }
}
